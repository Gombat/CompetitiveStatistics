#include "challongeimporter.h"

#include <QDebug>
#include <QMessageBox>
#include <QSet>
#include <QMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QInputDialog>

ChallongeImporter::ChallongeImporter(QObject* parent)
 : QObject(parent)
{
    m_mainwindow = qobject_cast<QWidget*>(parent);
}

void ChallongeImporter::errMsg( const QString& msg ) const
{
    qDebug() << msg;
    QMessageBox::warning(NULL, "Import Error", msg);
    return;
}

void ChallongeImporter::importFile( const QString& path, const QString& session_tournament_name) const
{
    QFile f(path);
    if ( !f.open(QFile::ReadOnly | QFile::Text)) return;
    QTextStream in(&f);
    in.setCodec("UTF-8");
    QString text = in.readAll();
    //qDebug() << f.size() << text;

    import( text, session_tournament_name );
}

void ChallongeImporter::import( const QString& data, const QString& session_tournament_name) const
{
    if ( QFile(data).exists() )
    {
        importFile( data, session_tournament_name );
        return;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(data.toUtf8());

    QMap<int,PlayerData> player_data;
    std::vector<MatchData> match_data;

    const QJsonObject root = jsonDoc.object();
    const QJsonObject tournament = root["tournament"].toObject();

    const QString session_state = tournament["state"].toString();
    //if ( session_state.compare("complete", Qt::CaseInsensitive) != 0 )
    //{ errMsg("Can only import completed tournaments, yet!"); return; }

    const QString session_date = tournament["started_at"].toString().left(10);
    const QString session_name = tournament["name"].toString();
    const QString session_url  = tournament["full_challonge_url"].toString();
    const int session_challonge_id = tournament["id"].toInt();

    if ( Database::tournamentID(session_tournament_name) < 0 )
    {
        if ( !Database::addTournament(session_tournament_name) )
        {
            QMessageBox::warning( m_mainwindow, "New Tournament", "Adding new tournament failed!");
            return;
        }
    }

    const int session_tournament_id = Database::tournamentID(session_tournament_name);
    if ( session_tournament_id < 0 )
    {
        QMessageBox::warning( m_mainwindow, "Selecting Tournament",
            "Could not find tournament \"" + session_tournament_name + "\"!");
        return;
    }

    if (Database::sessionExistsByChallongeID(session_challonge_id) )
    {
        QMessageBox::warning( m_mainwindow, "Challonge Import Failed",
                              "This challonge tournament has already been imported!" );
    }
    else
    {
        Database::addSession( session_name, session_challonge_id, session_tournament_id);
    }

    importMatches(tournament, session_date, session_name,
                  session_challonge_id, session_tournament_id,
                  session_url, player_data, match_data);
}

void ChallongeImporter::importMatches(const QJsonObject& tournament,
    const QString& session_date, const QString& session_name,
    const int session_challonge_id, const int session_tournament_id,
    const QString& session_url, QMap<int,PlayerData>& player_data,
    std::vector<MatchData>& match_data) const
{
    importParticipants_p( tournament, player_data );

    const QJsonArray matches = tournament["matches"].toArray();

    importMatches_p( matches, match_data, player_data );

    // export immediately?
    exportParticipants_p( player_data );

    if ( !Database::sessionExists(session_name) )
    {
        Database::addSession( session_name, session_challonge_id,
            session_tournament_id, session_date, session_url );
    }

    for ( size_t i = 0; i < match_data.size(); i++ )
    {
        const MatchData& match_date = match_data[i];
        Database::addOrUpdateMatch( session_name,
            session_challonge_id, session_tournament_id,
            match_date.p1.tag, match_date.p1.name,
            match_date.p2.tag, match_date.p2.name,
            match_date.round, match_date.suggested_play_order,
            match_date.p1_wins, match_date.p2_wins );
    }
}

void ChallongeImporter::importParticipants_p(const QJsonObject& tournament,
    QMap<int,PlayerData>& player_data) const
{
    const QJsonArray participants = tournament["participants"].toArray();
    for ( int i = 0; i < participants.count(); i++ )
    {
        const QJsonObject participant = participants.at(i).toObject()
            ["participant"].toObject();
        QString player_name = participant["name"].toString();
        QString player_tag = participant["name"].toString().toLower();

        player_name = Database::playerNameReplacements(player_name);
        player_tag = Database::playerNameReplacements(player_tag);
        if ( player_tag.compare( "bye", Qt::CaseInsensitive ) == 0 )
            continue;

        const int player_challonge_id = participant["id"].toInt();

        const QJsonArray group_player_ids_array =
            participant["group_player_ids"].toArray();
        QSet<int> group_player_ids;
        for ( int i = 0; i < group_player_ids_array.size(); i++ )
        { group_player_ids.insert(group_player_ids_array.at(0).toInt()); }

        if ( player_data.find(player_challonge_id) == player_data.end() )
        {
            player_data[player_challonge_id] =
                PlayerData(player_challonge_id,
                           player_tag,
                           player_name,
                           group_player_ids);
        }
    }
}

void ChallongeImporter::importMatches_p( const QJsonArray matches,
    std::vector<MatchData>& match_data,
    const QMap<int,PlayerData>& player_data ) const
{
    for ( int i = 0; i < matches.count(); i++ )
    {
        const QJsonObject match = matches.at(i).toObject()
            ["match"].toObject();
        const int winner_id = match["winner_id"].toInt();
        const int loser_id = match["loser_id"].toInt();
        const int p1_id = match["player1_id"].toInt();
        const int p2_id = match["player2_id"].toInt();
        const int round = match["round"].toInt();
        const int suggested_play_order = match["suggested_play_order"].toInt();
        const QString scores_csv = match["scores_csv"].toString();

        QStringList scores;

        QStringList scoresList = scores_csv.split(",",
            QString::SplitBehavior::SkipEmptyParts);
        if ( scoresList.size() > 1 )
        {
            QMessageBox::warning(0, "scores_csv invalid",
                QString("Die scores_csv von (id %1) vs (id %2) ist ungültig!")
                    .arg(p1_id).arg(p2_id) );
        }

        if ( scoresList.isEmpty() ) continue;
        else
        {
            scores = scoresList.at(scoresList.size()-1).split("-",
                QString::SplitBehavior::SkipEmptyParts);
            if ( scores.size() != 2 )
            {
                qDebug() << "Invalid scores entry detected";
                if ( !player_p(player_data, winner_id) &&
                     !player_p(player_data, loser_id) )
                {
                    qDebug() << " ..." << player_data[winner_id].tag
                             << " vs " << player_data[loser_id].tag;
                }
                continue;
            }
        }

        if ( scores.isEmpty() )
            continue;

        int p1wins = scores[0].toInt();
        int p2wins = scores[1].toInt();

        // ignore dequed players (byes)
        if ( p1wins < 0 || p2wins < 0 )
        { continue; }
        if ( player_data[p1_id].tag.compare( "bye", Qt::CaseInsensitive ) == 0 ||
             player_data[p2_id].tag.compare( "bye", Qt::CaseInsensitive ) == 0 )
        { continue; }

        if ( scores_csv.startsWith("-")) p1wins *= -1;
        if ( scores_csv.contains("--")) p2wins *= -1;

        if ( !player_p(player_data, p1_id) || !player_p(player_data, p2_id) )
        {
            //errMsg("Invalid player id entries detected!");
            qDebug() << "Invalid player id entries detected!";
            continue;
        }

        match_data.push_back( MatchData( round,
            suggested_play_order,
            player_p(player_data, p1_id),
            player_p(player_data, p2_id),
            p1wins, p2wins ) );
    }
}

void ChallongeImporter::exportParticipants_p(
        const QMap<int,PlayerData>& player_data ) const
{
    for ( QMap<int,PlayerData>::const_iterator it = player_data.begin();
          it != player_data.end(); ++it )
    {
        if ( !Database::playerExists(it.value().tag) )
        { Database::addPlayer(it.value().tag, it.value().name); }
    }
}

PlayerData ChallongeImporter::player_p(
    const QMap<int,PlayerData>& player_data, const int p_id ) const
{
    if ( player_data.find(p_id) != player_data.end() )
    { return *(player_data.find(p_id)); }

    for ( QMap<int,PlayerData>::const_iterator it = player_data.begin();
          it != player_data.end(); ++it )
    { if ( it->group_player_ids.contains(p_id) ){ return *it; } }

    return PlayerData(""); // if not found, return empty struct
}

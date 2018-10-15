#include "database.h"
#include <QApplication>
#include <QDir>

#include <QDebug>
#include <QDateTime>
#include <QMessageBox>

#include <iostream>

Database::Database(QObject* parent)
 : QObject(parent)
{
}

QString Database::gamerTagReplacements(const QMap<QString, QString> gamertag_replacements, const QString& player_name)
{
    if (gamertag_replacements.contains(player_name.toLower()))
    {
        std::cout << "Replacing " << player_name.toStdString() << " with " << gamertag_replacements[player_name.toLower()].toStdString() << std::endl;
        return gamertag_replacements[player_name.toLower()];
    }
    else
        return player_name;
}

QSqlDatabase& Database::db(const QString& db_path)
{
    static QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    if ( db.databaseName().isEmpty() )
    {
        db.setDatabaseName(db_path);
    }
    qDebug() << db.databaseName();
    if( !db.open() )
    {
        qDebug() << db.lastError().text();
        qFatal( "Failed to connect to database!" );
    }
    qDebug( "Connected!" );

    return db;
}

void Database::initDB(const QString& cfg_file)
{
    QString db_path;
    QFileInfo cfg_fileInfo(cfg_file);
    if (cfg_file.isEmpty() || !cfg_fileInfo.exists())
    {
        db_path = QApplication::instance()->applicationDirPath() +
                  QDir::separator() + "stats.sqlite";
    }
    else
    {
        db_path = cfg_fileInfo.absolutePath() +
                  QDir::separator() + "stats.sqlite";
    }
    db(db_path); // init db filepath

    createTableConfig();
    createTablePlayers();
    createTableTournaments();
    createTableElo();
    createTableSessions();
    createTableMatches();
}

void Database::createTableConfig()
{
    QSqlDatabase& db = Database::db();
    QSqlQuery query;
    if ( !db.tables().contains( "config" ) )
    {
        if ( !query.exec( "CREATE TABLE config ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "name varchar(255) NOT NULL UNIQUE, "
                    "val varchar(255) NOT NULL)" ) )
        { qDebug() << query.lastError().text(); }

        setConfigVal("eloK", 25);
    }
}

void Database::setConfigVal(const QString name, const QVariant val)
{
    QSqlQuery query;
    query.prepare("INSERT INTO config (name,val) VALUES(:name,:val)");
    query.bindValue(":name", name);
    query.bindValue(":val", val.toString());
    if ( !query.exec() ){ qDebug() << "setConfigVal failed!"; }
}

bool Database::getConfigVal(const QString name, QVariant &val)
{
    QSqlQuery query;
    query.prepare( "SELECT val FROM config WHERE name = :name" );
    query.bindValue( ":name", name );
    query.exec();
    if( query.next() ) { val = query.value(0); return true; }
    return false;
}

void Database::createTablePlayers()
{
    QSqlDatabase& db = Database::db();
    QSqlQuery query;
    if ( !db.tables().contains( "players" ) )
    {
        if ( !query.exec( "CREATE TABLE players ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "tag varchar(255) NOT NULL, "
                    "name varchar(255) )" ) )
        { qDebug() << query.lastError().text(); }
    }
}

void Database::createTableTournaments()
{
    QSqlDatabase& db = Database::db();
    QSqlQuery query;
    if ( !db.tables().contains( "tournaments" ) )
    {
        if ( !query.exec( "CREATE TABLE tournaments ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "name varchar(255) )" ) )
        { qDebug() << query.lastError().text(); }
    }
}

void Database::createTableElo()
{
    QSqlDatabase& db = Database::db();
    QSqlQuery query;
    if ( !db.tables().contains( "elo" ) )
    {
        if ( !query.exec( "CREATE TABLE elo ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "player_id INTEGER NOT NULL UNIQUE, "
                    "player_tag varchar(255) NOT NULL, "
                    "rating_start INTEGER NOT NULL, "
                    "rating INTEGER NOT NULL, "
                    "set_wins INTEGER NOT NULL, "
                    "set_losses INTEGER NOT NULL, "
                    "match_wins INTEGER NOT NULL, "
                    "match_losses INTEGER NOT NULL, "
                    "attendances INTEGER NOT NULL)"))//, "
                    //"FOREIGN KEY(player_id) REFERENCES players(id) ON UPDATE CASCADE )" ) )
        { qDebug() << query.lastError().text(); }
    }
}

void Database::createTableSessions()
{
    QSqlDatabase& db = Database::db();
    QSqlQuery query;
    if ( !db.tables().contains( "sessions" ) )
    {
        if ( !query.exec( "CREATE TABLE sessions ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "name varchar(255) NOT NULL, "
                    "challonge_id INTEGER NOT NULL, "
                    "tournament_id INTEGER NOT NULL, "
                    "date varchar(255) NOT NULL, "
                    "url varchar(1024) )" ))//,"
                    //"FOREIGN KEY(tournament_id) REFERENCES tournaments(id) ON UPDATE CASCADE" ) )
        { qDebug() << query.lastError().text(); }
    }
}

void Database::createTableMatches()
{
    QSqlDatabase& db = Database::db();
    QSqlQuery query;
    if ( !db.tables().contains( "matches" ) )
    {
        if ( !query.exec( "CREATE TABLE matches ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "session_id int NOT NULL, "
                    "round int NOT NULL, "
                    "suggested_play_order int NOT NULL, "
                    "p1 int NOT NULL, "
                    "p2 int NOT NULL, "
                    "p1_wins int NOT NULL, "
                    "p2_wins int NOT NULL )" ) ) //,"
                    //"FOREIGN KEY(session_id) REFERENCES sessions(id) ON UPDATE CASCADE, "
                    //"FOREIGN KEY(p1) REFERENCES players(id) ON UPDATE CASCADE, "
                    //"FOREIGN KEY(p1) REFERENCES players(id) ON UPDATE CASCADE )" ) )
        { qDebug() << query.lastError().text(); }
    }
}

bool Database::tournamentExists( const QString& name )
{
    QSqlQuery query;
    query.prepare( "SELECT name FROM tournaments WHERE name = :name COLLATE NOCASE" );
    query.bindValue( ":name", name );
    query.exec();
    if( query.next() ) { return true; }

    return false;
}

QStringList Database::tournaments( )
{
    QStringList list_tournaments;
    QSqlQuery query;
    if ( !query.exec( "SELECT name FROM tournaments" ) )
    { qDebug() << "tournaments() failed: " << query.lastError(); }
    while ( query.next() ){ list_tournaments << query.value(0).toString(); }
    return list_tournaments;
}

bool Database::addTournament( const QString& name )
{
    if ( tournamentExists( name ) ) { return false; }

    QSqlQuery query;
    query.prepare( "INSERT INTO tournaments (name) VALUES( :name)" );
    query.bindValue(":name", name );
    if ( !query.exec() )
    { qDebug() << "addTournament failed: " << query.lastError(); }
    return true;
}

int Database::tournamentID( const QString& name )
{
    QSqlQuery query;
    query.prepare( "SELECT id FROM tournaments WHERE name = :name COLLATE NOCASE" );
    query.bindValue( ":name", name );
    query.exec();
    if( query.next() ) { return query.value(0).toInt(); }
    return -1;
}

QString Database::tournamentName( const int id )
{
    QSqlQuery query;
    query.prepare( "SELECT name FROM tournaments WHERE id = :id COLLATE NOCASE" );
    query.bindValue( ":id", id );
    query.exec();
    if( query.next() ) { return query.value(0).toString(); }
    return "";
}

bool Database::addPlayerToEloRating(const int player_id)
{
    QSqlQuery query;
    query.prepare( "INSERT INTO elo "
                   "(player_id, player_tag, "
                   "rating_start, rating, "
                   "set_wins, set_losses, "
                   "match_wins, match_losses, attendances) "
                   "VALUES"
                   "(:player_id, :player_tag, "
                   ":rating_start, :rating, "
                   ":set_wins, :set_losses, "
                   ":match_wins, :match_losses, :attendances) " );
    query.bindValue(":player_id", player_id);
    query.bindValue(":player_tag", Database::playerTag(player_id));
    query.bindValue(":rating_start", 1500);
    query.bindValue(":rating", 1500);
    query.bindValue(":set_wins", 0);
    query.bindValue(":set_losses", 0);
    query.bindValue(":match_wins", 0);
    query.bindValue(":match_losses", 0);
    query.bindValue(":attendances", 0);
    if ( !query.exec() )
    {
        qDebug() << "addPlayerToEloRating failed: " << query.lastError();
        return false;
    }

    return true;
}

bool Database::clearEloRating()
{
    QSqlQuery query;
    if ( !query.exec( "UPDATE elo SET rating=rating_start") ) return false;
    if ( !query.exec( "UPDATE elo SET set_wins=0") ) return false;
    if ( !query.exec( "UPDATE elo SET set_losses=0") ) return false;
    if ( !query.exec( "UPDATE elo SET match_wins=0") ) return false;
    if ( !query.exec( "UPDATE elo SET match_losses=0") ) return false;
    if ( !query.exec( "UPDATE elo SET attendances=0") ) return false;

    return true;
}

bool Database::eloData(std::vector<EloData>& vecEloData)
{
    vecEloData.clear();

    QSqlQuery query;
    query.prepare( "SELECT player_id, rating_start, rating, set_wins, "
        "set_losses, match_wins, match_losses, attendances FROM elo" );
    query.exec();
    while( query.next() )
    {
        EloData eloData_;
        eloData_.player_id = query.value(0).toInt();
        eloData_.rating_start = query.value(1).toInt();
        eloData_.rating = query.value(2).toInt();
        eloData_.set_wins = query.value(3).toInt();
        eloData_.set_losses = query.value(4).toInt();
        eloData_.match_wins = query.value(5).toInt();
        eloData_.match_losses = query.value(6).toInt();
        eloData_.attendances = query.value(7).toInt();

        vecEloData.push_back(eloData_);
    }

    if ( vecEloData.empty() )
        return false;

    return true;
}

bool Database::eloData(std::map<int,EloData>& mapEloData)
{
    std::vector<EloData> vecEloData;
    if ( !Database::eloData(vecEloData) ) return false;

    mapEloData.clear();
    for ( int i = 0; i < static_cast<int>(vecEloData.size()); i++ )
    {
        mapEloData[vecEloData[i].player_id] = vecEloData[i];
    }

    if ( mapEloData.empty() ) return false;

    return true;
}

void Database::saveEloToTable( const std::map<int,EloData>& mapEloData )
{
    QSqlQuery query;
    for ( std::map<int,EloData>::const_iterator it = mapEloData.begin();
          it != mapEloData.end(); ++it )
    {
        query.prepare( "UPDATE elo SET rating = :rating, set_wins = :set_wins,"
                       "set_losses = :set_losses, match_wins = :match_wins, "
                       "match_losses = :match_losses, attendances = :attendances "
                       "WHERE player_id = :player_id");
        query.bindValue(":rating", it->second.rating);
        query.bindValue(":set_wins", it->second.set_wins);
        query.bindValue(":set_losses", it->second.set_losses);
        query.bindValue(":match_wins", it->second.match_wins);
        query.bindValue(":match_losses", it->second.match_losses);
        query.bindValue(":attendances", it->second.attendances);
        query.bindValue(":player_id", it->second.player_id);
        if ( !query.exec() )
        { qDebug() << "saveEloToTable failed: " << query.lastError(); }
    }
}

bool Database::playerExists( const QString& tag )
{
    QSqlQuery query;
    query.prepare( "SELECT tag FROM players WHERE tag = :tag COLLATE NOCASE" );
    query.bindValue( ":tag", tag.toLower() );
    query.exec();
    if( query.next() ) { return true; }

    return false;
}

QStringList Database::players( )
{
    QStringList players;
    QSqlQuery query;
    if ( !query.exec( "SELECT tag FROM players" ) )
    { qDebug() << "players() failed: " << query.lastError(); }
    while ( query.next() ){ players << query.value(0).toString(); }
    players.sort();
    return players;
}

QStringList Database::playerNames( )
{
    QStringList players;
    QSqlQuery query;
    if ( !query.exec( "SELECT name FROM players" ) )
    { qDebug() << "players() failed: " << query.lastError(); }
    while ( query.next() ){ players << query.value(0).toString(); }
    players.sort();
    return players;
}

QStringList Database::players( const QString& session_name )
{
    QStringList ret;
    if ( !sessionExists(session_name) ){ return ret; }
    QSet<QString> players_in_session;
    QSqlQuery query;
    query.prepare("SELECT p1, p2 FROM matches WHERE session_id = :session_id");
    query.bindValue( ":session_id", sessionID(session_name) );
    if ( !query.exec( ) )
    { qDebug() << "players(session_name) failed: " << query.lastError(); }
    while ( query.next() )
    {
        players_in_session.insert(query.value(0).toString());
        players_in_session.insert(query.value(1).toString());
    }
    ret = players_in_session.toList(); ret.sort();
    return ret;
}

QString Database::playerTag( const int player_id )
{
    QSqlQuery query;
    query.prepare( "SELECT tag FROM players where id = :player_id" );
    query.bindValue(":player_id", player_id);
    if ( !query.exec() )
    { qDebug() << "playerTag(player_id) failed: " << query.lastError(); }
    if ( query.next() ){ return query.value(0).toString(); }
    return "";
}

QString Database::playerName( const int player_id )
{
    QSqlQuery query;
    query.prepare( "SELECT name FROM players where id = :player_id" );
    query.bindValue(":player_id", player_id);
    if ( !query.exec() )
    { qDebug() << "playerName(player_id) failed: " << query.lastError(); }
    if ( query.next() ){ return query.value(0).toString(); }
    return "";
}

int Database::playerID( const QString& tag )
{
    QSqlQuery query;
    query.prepare( "SELECT id FROM players WHERE tag = :tag" );
    query.bindValue(":tag", tag.toLower());
    if ( !query.exec() )
    { qDebug() << "playerID() failed: " << query.lastError(); }
    if ( query.next() ){ return query.value(0).toInt(); }
    return -1;
}

bool Database::addPlayer( const QString& tag, const QString& name )
{
    if (tag.compare("angelos", Qt::CaseInsensitive) == 0)
        std::cout << "addPlayer: " << tag.toStdString() << std::endl;

    if ( playerExists( tag ) ) { return false; }

    QSqlQuery query;
    query.prepare( "INSERT INTO players (tag, name) VALUES( :tag, :name)" );
    query.bindValue(":tag", tag.toLower());
    query.bindValue(":name", name.isEmpty() ? tag : name );
    if ( !query.exec() )
    { qDebug() << "addPlayer failed: " << query.lastError(); }

    if ( !query.exec("SELECT last_insert_rowid()") )
    { qDebug() << "addPlayer SELECT last_insert_rowid() failed: " << query.lastError(); }
    int last_row_id = -1;
    if ( query.next() ){ last_row_id = query.value(0).toInt(); }

    if ( !addPlayerToEloRating(last_row_id) )
        return false;

    return true;
}

bool Database::sessionExists( const QString& name )
{
    QSqlQuery query;
    query.prepare( "SELECT name FROM sessions WHERE name = :name COLLATE NOCASE" );
    query.bindValue( ":name", name );
    query.exec();
    if( query.next() ) { return true; }

    return false;
}

bool Database::sessionExistsByChallongeID(const int challonge_id)
{
    QSqlQuery query;
    query.prepare( "SELECT id FROM sessions WHERE challonge_id = :challonge_id COLLATE NOCASE" );
    query.bindValue( ":challonge_id", challonge_id );
    query.exec();
    if( query.next() ) { return true; }

    return false;
}

QStringList Database::sessions( )
{
    QStringList sessions;
    QSqlQuery query;
    if ( !query.exec( "SELECT name FROM sessions" ) )
    { qDebug() << "sessions() failed: " << query.lastError(); }
    while ( query.next() ){ sessions << query.value(0).toString(); }
    return sessions;
}

QStringList Database::sessionsByTournamentID( const int tournament_id )
{
    QStringList sessions;
    QSqlQuery query;
    query.prepare( "SELECT name FROM sessions WHERE tournament_id = :tournament_id" );
    query.bindValue( ":tournament_id", tournament_id );
    if ( !query.exec() )
    { qDebug() << "sessionsByTournamentID() failed: " << query.lastError(); }
    while ( query.next() ){ sessions << query.value(0).toString(); }
    return sessions;
}

bool Database::addSession(
    const QString& name,
    const int challonge_id,
    const int tournament_id,
    const QString& date,
    const QString& url )
{
    if ( sessionExists(name) ) return false;
    if ( sessionExistsByChallongeID(challonge_id) )
    {
        QMessageBox::warning( nullptr, "Error", "Cannot import session, has been imported already!" );
        return false;
    }

    QString _date(date);
    if ( _date.isEmpty() )
    {
        _date = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    }

    QSqlQuery query;
    query.prepare( "INSERT INTO sessions (name, challonge_id, tournament_id, date, url) "
                   "VALUES( :name, :challonge_id, :tournament_id, :date, :url)" );
    query.bindValue(":name", name);
    query.bindValue(":challonge_id", challonge_id);
    query.bindValue(":tournament_id", tournament_id);
    query.bindValue(":date", _date);
    query.bindValue(":url", url);
    if ( !query.exec() )
    { qDebug() << "addSession failed: " << query.lastError(); return false; }

    return true;
}

bool Database::updateSession( const int id,
                              const QString& name,
                              const int challonge_id,
                              const int tournament_id,
                              const QString& date,
                              const QString& url)
{
    if ( id < 0 ) return false;
    if ( !sessionExists(name) ) return false;
    QSqlQuery query;
    query.prepare( "UPDATE sessions SET name = :name, "
                   "challonge_id = :challonge_id, "
                   "tournament_id = :tournament_id, "
                   "date = :date, url = :url "
                   "WHERE id = :id" );
    query.bindValue(":name", name);
    query.bindValue(":challonge_id", challonge_id);
    query.bindValue(":tournament_id", tournament_id);
    query.bindValue(":date", date);
    query.bindValue(":url", url);
    query.bindValue(":id", id );
    if ( !query.exec() )
    { qDebug() << "updateSession failed: " << query.lastError(); return false; }
    return true;
}

bool Database::deleteSession( const QString& name )
{
    if ( !sessionExists(name) ) return false;
    quint32 id = sessionID(name);
    QSqlQuery query;
    query.prepare( "Delete FROM sessions WHERE id = :id" );
    query.bindValue(":id", id);
    if ( !query.exec() )
    { qDebug() << "deleteSession failed: " << query.lastError(); return false; }

    query.prepare( "Delete FROM matches WHERE session_id = :id" );
    query.bindValue(":id", id);
    if ( !query.exec() )
    { qDebug() << "deleteSession failed: " << query.lastError(); return false; }

    return true;
}

int Database::sessionID( const QString& session_name )
{
    if ( !sessionExists(session_name) ) return -1;
    QSqlQuery query;
    query.prepare( "SELECT id FROM sessions WHERE name = :name" );
    query.bindValue(":name", session_name);
    if ( !query.exec() )
    { qDebug() << "sessionID failed: " << query.lastError(); }
    if ( query.next() ) return query.value(0).toInt();
    return -1;
}

int Database::sessionChallongeID( const QString& session_name )
{
    if ( !sessionExists(session_name) ) return -1;
    QSqlQuery query;
    query.prepare( "SELECT challonge_id FROM sessions WHERE name = :name" );
    query.bindValue(":name", session_name);
    if ( !query.exec() )
    { qDebug() << "sessionChallongeID failed: " << query.lastError(); }
    if ( query.next() ) return query.value(0).toInt();
    return -1;
}

int Database::sessionTournamentID( const QString& session_name )
{
    if ( !sessionExists(session_name) ) return -1;
    QSqlQuery query;
    query.prepare( "SELECT tournament_id FROM sessions WHERE name = :name" );
    query.bindValue(":name", session_name);
    if ( !query.exec() )
    { qDebug() << "sessionTournamentID failed: " << query.lastError(); }
    if ( query.next() ) return query.value(0).toInt();
    return -1;
}

QString Database::sessionDate( const QString& session_name )
{
    if ( !sessionExists(session_name) ) return "";
    QSqlQuery query;
    query.prepare( "SELECT date FROM sessions WHERE name = :name" );
    query.bindValue(":name", session_name);
    if ( !query.exec() )
    { qDebug() << "sessionDate failed: " << query.lastError(); }
    if ( query.next() ) return query.value(0).toString();
    return "";
}

QString Database::sessionURL( const QString& session_name )
{
    if ( !sessionExists(session_name) ) return "";
    QSqlQuery query;
    query.prepare( "SELECT url FROM sessions WHERE name = :name" );
    query.bindValue(":name", session_name);
    if ( !query.exec() )
    { qDebug() << "sessionURL failed: " << query.lastError(); }
    if ( query.next() ) return query.value(0).toString();
    return "";
}

bool Database::addOrUpdateMatch(
    const QString& session_name,
    const int challonge_id,
    const int session_tournament_id,
    const QString& p1_tag,
    const QString& p1_name,
    const QString& p2_tag,
    const QString& p2_name,
    const int round,
    const quint32 suggested_play_order,
    const int p1_wins,
    const int p2_wins)
{
    if ( !sessionExists(session_name) ){ addSession(session_name, session_tournament_id, challonge_id); }
    if ( !playerExists(p1_tag) ){ addPlayer(p1_tag, p1_name); }
    if ( !playerExists(p2_tag) ){ addPlayer(p2_tag, p2_name); }

    const int session_id = sessionID(session_name);
    if ( session_id < 0 )
    { qDebug() << "addOrUpdateMatch failed! Invalid session id."; return false; }

    if ( p1_wins == p2_wins )
    {
        qDebug() << "Adding or Updating Match failed!",
        "Cannot insert a tie for elo ranking! " + p1_tag + " vs " + p2_tag;
        return false;
    }
    if ( p1_wins < 0 || p2_wins < 0 )
    {
        qDebug() << "Adding or Updating Match failed!",
        "Cannot insert a DQ/Leave for elo ranking! " + p1_tag + " vs " + p2_tag;
        return false;
    }

    QSqlQuery query;

    quint32 match_id, suggested_play_order_(suggested_play_order);
    int p1_wins_, p2_wins_;
    int round_(round);
    bool matchExists = matchResults_p( session_name, p1_tag, p2_tag, round_,
        suggested_play_order_, p1_wins_, p2_wins_, match_id);

    query.clear();
    if ( matchExists )
    {
//        if ( !query.prepare( "UPDATE matches SET session_id = :session_id, round = :round, "
//            "suggested_play_order = :suggested_play_order, "
//            "p1 = :p1, p2 = :p2, p1_wins = :p1_wins, p2_wins = :p2_wins "
//            "WHERE id = :match_id" ) )
//        { qDebug() << "addorUpdateMatch failed: " << query.lastError(); return false; }
        if ( !query.prepare( "UPDATE matches SET p1_wins = :p1_wins, p2_wins = :p2_wins "
            "WHERE id = :match_id" ) )
        { qDebug() << "addorUpdateMatch failed: " << query.lastError(); return false; }
        query.bindValue(":p1_wins", p1_wins);
        query.bindValue(":p2_wins", p2_wins);
        query.bindValue(":match_id", match_id );
    }
    else
    {
        if ( !query.prepare( "INSERT INTO matches (session_id, round, suggested_play_order, p1, p2, p1_wins, p2_wins) "
            "VALUES( :session_id, :round, :suggested_play_order, :p1, :p2, :p1_wins, :p2_wins )" ) )
        { qDebug() << "addorUpdateMatch failed: " << query.lastError(); return false; }
        query.bindValue(":session_id", session_id);
        query.bindValue(":round", round);
        query.bindValue(":suggested_play_order", suggested_play_order);
        query.bindValue(":p1", p1_tag);
        query.bindValue(":p2", p2_tag);
        query.bindValue(":p1_wins", p1_wins);
        query.bindValue(":p2_wins", p2_wins);
    }
//    query.bindValue(":session_id", session_id);
//    query.bindValue(":round", round);
//    query.bindValue(":suggested_play_order", suggested_play_order);
//    query.bindValue(":p1", p1);
//    query.bindValue(":p2", p2);
//    query.bindValue(":p1_wins", p1_wins);
//    query.bindValue(":p2_wins", p2_wins);
//    if ( matchExists ) { query.bindValue(":match_id", match_id ); }
    if ( !query.exec() )
    { qDebug() << "addorUpdateMatch failed: " << query.lastError(); return false; }

    return true;
}

bool Database::matchResults( const QString& p1_tag,
                             const QString& p2_tag,
                             int& p1_set_wins,
                             int& p2_set_wins,
                             int& p1_match_wins,
                             int& p2_match_wins)
{
    p1_set_wins = 0;
    p2_set_wins = 0;
    p1_match_wins = 0;
    p2_match_wins = 0;

    {
        QSqlQuery query;
        query.prepare("SELECT p1_wins, p2_wins FROM matches WHERE "
            "p1 LIKE :p1 AND p2 LIKE :p2");
        query.bindValue( ":p1", p1_tag );
        query.bindValue( ":p2", p2_tag );
        if ( !query.exec( ) )
        { qDebug() << "matchResults_p() failed: " << query.lastError(); return false; }
        while ( query.next() )
        {
            int p1_wins  = query.value(0).toInt();
            int p2_wins  = query.value(1).toInt();

            if ( p1_wins > p2_wins )
                p1_set_wins++;
            else
                p2_set_wins++;

            p1_match_wins += p1_wins;
            p2_match_wins += p2_wins;
        }
    }
    {
        QSqlQuery query;
        query.prepare("SELECT p1_wins, p2_wins FROM matches WHERE "
            "p1 LIKE :p1 AND p2 LIKE :p2");
        query.bindValue( ":p2", p1_tag );
        query.bindValue( ":p1", p2_tag );
        if ( !query.exec( ) )
        { qDebug() << "matchResults_p() failed: " << query.lastError(); return false; }
        while ( query.next() )
        {
            int p1_wins  = query.value(1).toInt();
            int p2_wins  = query.value(0).toInt();

            if ( p1_wins > p2_wins )
                p1_set_wins++;
            else
                p2_set_wins++;

            p1_match_wins += p1_wins;
            p2_match_wins += p2_wins;
        }
    }
    return true;
}

bool Database::matchResults( const QString& session_name,
                             const QString& p1_tag,
                             const QString& p2_tag,
                             int& round,
                             quint32& suggested_play_order,
                             int& p1_wins,
                             int& p2_wins)
{
    if ( !sessionExists(session_name) || !playerExists(p1_tag) || !playerExists(p2_tag) )
        return false;
    quint32 match_id = -1;
    if ( !matchResults_p(session_name, p1_tag, p2_tag, round,
                         suggested_play_order, p1_wins, p2_wins, match_id ) )
    { return false; }
    return true;
}

bool Database::matchResults_p( const QString& session_name,
                               const QString& p1,
                               const QString& p2,
                               int& round,
                               quint32& suggested_play_order,
                               int& p1_wins,
                               int& p2_wins,
                               quint32& match_id )
{
    QSqlQuery query;
    query.prepare("SELECT id, round, suggested_play_order, p1_wins, p2_wins FROM matches WHERE "
        "session_id = :session_id AND p1 = :p1 AND p2 = :p2 AND "
        "round = :round AND "
        "suggested_play_order = :suggested_play_order");
    query.bindValue( ":session_id", sessionID(session_name) );
    query.bindValue( ":p1", p1 );
    query.bindValue( ":p2", p2 );
    query.bindValue( ":round", round );
    query.bindValue( ":suggested_play_order", suggested_play_order );
    if ( !query.exec( ) )
    { qDebug() << "matchResults_p() failed: " << query.lastError(); return false; }
    if ( query.next() )
    {
        match_id = query.value(0).toInt();
        round    = query.value(1).toInt();
        suggested_play_order = query.value(2).toInt();
        p1_wins  = query.value(3).toInt();
        p2_wins  = query.value(4).toInt();
        return true;
    }

    // try another p1 / p2 combination

    query.prepare("SELECT id, round, suggested_play_order, p1_wins, p2_wins FROM matches WHERE "
        "session_id = :session_id AND p1 = :p1 AND p2 = :p2 AND "
        "round = :round AND "
        "suggested_play_order = :suggested_play_order");
    query.bindValue( ":session_id", sessionID(session_name) );
    query.bindValue( ":p1", p2 );
    query.bindValue( ":p2", p1 );
    query.bindValue( ":round", round );
    query.bindValue( ":suggested_play_order", suggested_play_order );
    if ( !query.exec( ) )
    { qDebug() << "matchResults_p() failed: " << query.lastError(); return false; }
    if ( query.next() )
    {
        match_id = query.value(0).toInt();
        round    = query.value(1).toInt();
        suggested_play_order = query.value(2).toInt();
        p2_wins  = query.value(3).toInt();
        p1_wins  = query.value(4).toInt();
        return true;
    }

    return false;
}

bool Database::matchResults( const QString& session_name,
                             std::vector<MatchData>& match_data)
{
    match_data.clear();
    if ( !sessionExists(session_name) ){ return false; }
    QSqlQuery query;
    query.prepare("SELECT id, round, suggested_play_order, p1, p2, p1_wins, p2_wins FROM matches WHERE "
        "session_id = :session_id");
    query.bindValue( ":session_id", sessionID(session_name) );
    if ( !query.exec( ) )
    { qDebug() << "matchResults() failed: " << query.lastError(); return false; }
    while ( query.next() )
    {
        //int match_id = query.value(0).toInt();
        MatchData matchDate(
            query.value(1).toInt(),
            query.value(2).toInt(),
            PlayerData( Database::playerID(query.value(3).toString()), query.value(3).toString().toLower(), query.value(3).toString() ),
            PlayerData( Database::playerID(query.value(4).toString()), query.value(4).toString().toLower(), query.value(4).toString() ),
            query.value(5).toInt(),
            query.value(6).toInt() );
        match_data.push_back( matchDate );
    }
    return true;
}

//void Database::obtainPools( const QString& session_name,
//    std::vector< std::vector< StandingData > >& poolStandings )
//{
//    poolStandings.clear();
//    if ( Database::sessionType(session_name) != SessionType::Pools ) return;

//    std::vector<MatchData> match_data;
//    matchResults( session_name, match_data );

//    QStringList player_data = players( session_name );

//    // construct pools
//    std::vector< QSet< QString > > pools, pools_;

//    // add to "pools_"
//    for ( size_t i = 0; i < match_data.size(); i++ )
//    {
//        const MatchData& match_date = match_data[i];

//        QString p1_name = match_date.p1.tag;
//        QString p2_name = match_date.p2.tag;
//        int p1_pool(-1), p2_pool(-1);
//        for ( size_t i = 0; i < pools_.size(); i++ )
//        {
//            const QSet<QString>& pool = pools_[i];
//            if      ( pool.contains(p1_name) ) { p1_pool = i; }
//            else if ( pool.contains(p2_name) ) { p2_pool = i; }
//        }
//        if ( p1_pool < 0 && p2_pool < 0 )
//        {
//            pools_.push_back( QSet<QString>() );
//            pools_.back().insert( p1_name );
//            pools_.back().insert( p2_name );
//        }
//        else if ( p1_pool >= 0 ){ pools_[p1_pool].insert( p2_name ); }
//        else if ( p2_pool >= 0 ){ pools_[p1_pool].insert( p1_name ); }
//    }

//    // merge "pools"
//    for ( size_t i = 0; i < pools_.size(); i++ )
//    {
//        int pool_found_id = -1;
//        const QSet< QString >& pool_ = pools_[i];
//        for ( QSet< QString >::const_iterator it = pool_.begin();
//              it != pool_.end(); ++it )
//        {
//            const QString& player_ = *it;
//            for ( size_t k = 0; k < pools.size(); k++ )
//            {
//                if ( pools[k].contains(player_) )
//                { pool_found_id = k; break; }
//            }
//        }
//        if ( pool_found_id < 0 )
//        {
//            pools.push_back( pool_ );
//        }
//        else
//        {
//            for ( QSet< QString >::const_iterator it = pool_.begin();
//                  it != pool_.end(); ++it )
//            { pools[pool_found_id].insert( *it ); }
//        }
//    }

//    QMap< QString, StandingData > standings_map;
//    for ( size_t i = 0; i < match_data.size(); i++ )
//    {
//        const MatchData& matchDate = match_data[i];
//        {
//            StandingData& standingDate = standings_map[ matchDate.p1.tag ];
//            standingDate.player = matchDate.p1.tag;
//            if ( matchDate.p1_wins > matchDate.p2_wins )
//            {
//                standingDate.won_against.insert( matchDate.p2.tag );
//                standingDate.set_wins++;
//            }
//            else { standingDate.set_losses++; }
//            standingDate.game_wins += matchDate.p1_wins;
//            standingDate.game_losses += matchDate.p2_wins;
//        }
//        {
//            StandingData& standingDate = standings_map[ matchDate.p2.tag ];
//            standingDate.player = matchDate.p2.tag;
//            if ( matchDate.p2_wins > matchDate.p1_wins )
//            {
//                standingDate.won_against.insert( matchDate.p1.tag );
//                standingDate.set_wins++;
//            }
//            else { standingDate.set_losses++; }
//            standingDate.game_wins += matchDate.p2_wins;
//            standingDate.game_losses += matchDate.p1_wins;
//        }
//    }

//    poolStandings.resize(pools.size());
//    for ( QMap< QString, StandingData >::const_iterator it = standings_map.begin();
//          it != standings_map.end(); ++it )
//    {
//        int pool_idx = 0;
//        for ( size_t j = 0; j < pools.size(); j++ )
//        {
//            if ( pools[j].contains( it.key() ) )
//            { pool_idx = j; break; }
//        }
//        poolStandings[pool_idx].push_back( it.value() );
//    }
//}

//bool Database::eloRating(const int player_id, int& elo)
//{
//    QSqlQuery query;
//    query.prepare( "SELECT rating_start, rating FROM elo WHERE player_id = :player_id");
//    query.bindValue(":player_id", player_id);
//    if ( !query.exec() )
//    { qDebug() << "eloRating() failed: " << query.lastError(); return false; }

//    return true;
//}

void Database::updateEloRating()
{
    // TODO
    //QStringList listSessions = Database::sessions( );

    // clear table elo
    clearEloRating();

    std::map<int,EloData> mapEloData;
    std::map<int,EloData> mapEloDataWork;
    std::map<int,EloData> mapEloDataLast;

    if ( !Database::eloData(mapEloData) ) return;
    mapEloDataWork = mapEloData;

    int k = 25;
    {
        QVariant eloK;
        if ( getConfigVal("eloK", eloK) )
            k = eloK.toInt();
    }


    // recompute elo from scratch
    QStringList listTournaments = Database::tournaments( );

    for ( int t = 0; t < listTournaments.size(); t++ )
    {
        int tournament_id = Database::tournamentID( listTournaments.at(t) );
        QStringList listSessions = Database::sessionsByTournamentID( tournament_id );

        for ( int s = 0; s < listSessions.size(); s++ )
        {
            std::vector<MatchData> matches;
            if ( !Database::matchResults( listSessions.at(s), matches ) ) continue;

            for ( int m = 0; m < static_cast<int>(matches.size()); m++ )
            {
                if ( !updateEloRating(k, mapEloData, matches[m], mapEloDataWork) )
                    continue;
            }
        }

        for ( std::map<int,EloData>::iterator it = mapEloDataWork.begin();
              it != mapEloDataWork.end(); ++it )
        {
            EloData& eloDataWork = it->second;
            const EloData& eloDataPrev = mapEloData[it->first];
            if ( eloDataWork.rating != eloDataPrev.rating ||
                 eloDataWork.set_wins != eloDataPrev.set_wins ||
                 eloDataWork.set_losses != eloDataPrev.set_losses ||
                 eloDataWork.match_wins != eloDataPrev.match_wins ||
                 eloDataWork.match_losses != eloDataPrev.match_losses )
            {
                eloDataWork.attendances += 1;
            }
        }

        // update elo between tournaments
        mapEloDataLast = mapEloData;
        mapEloData = mapEloDataWork;
    }

    saveEloToTable( mapEloData );
}

bool Database::updateEloRating(const int k,
                               const std::map<int,EloData>& mapEloData,
                               const MatchData match,
                               std::map<int,EloData> &mapEloDataWork)
{
    if ( match.p1_wins < 0 || match.p2_wins < 0 ||
         match.p1_wins == match.p2_wins )
    { return false; }

    const int p1_id = Database::playerID(match.p1.tag);
    const int p2_id = Database::playerID(match.p2.tag);

    int p1_elo_diff = 0;
    int p2_elo_diff = 0;

    const std::map<int,EloData>::const_iterator it_orig_p1 =
        mapEloData.find(p1_id);
    const std::map<int,EloData>::const_iterator it_orig_p2 =
        mapEloData.find(p2_id);

    if ( it_orig_p1 == mapEloData.end() ||
         it_orig_p2 == mapEloData.end() )
    { return false; }

    const int elo_p1_prev = it_orig_p1->second.rating;
    const int elo_p2_prev = it_orig_p2->second.rating;

    if ( match.p1_wins > match.p2_wins )
    {
        calcEloDiffs(k,
                     elo_p1_prev,
                     elo_p2_prev,
                     p1_elo_diff,
                     1.0,
                     p2_elo_diff,
                     0.0);

        mapEloDataWork[p1_id].set_wins += 1;
        mapEloDataWork[p2_id].set_losses += 1;
    }
    else if ( match.p2_wins > match.p1_wins )
    {
        calcEloDiffs(k,
                     elo_p1_prev,
                     elo_p2_prev,
                     p1_elo_diff,
                     0.0,
                     p2_elo_diff,
                     1.0);

        mapEloDataWork[p2_id].set_wins += 1;
        mapEloDataWork[p1_id].set_losses += 1;
    }
    else
        return false;

    mapEloDataWork[p1_id].match_wins += match.p1_wins;
    mapEloDataWork[p1_id].match_losses += match.p2_wins;

    mapEloDataWork[p2_id].match_wins += match.p2_wins;
    mapEloDataWork[p2_id].match_losses += match.p1_wins;

    mapEloDataWork[p1_id].rating += p1_elo_diff;
    mapEloDataWork[p2_id].rating += p2_elo_diff;

    return true;
}

void Database::calcEloDiffs(const int k,
                            const int p1_elo, const int p2_elo,
                            int& p1_elo_diff, double p1_win,
                            int& p2_elo_diff, double p2_win)
{
    double E_p1 = 1.0 / ( 1.0 + pow(10.0, (p2_elo - p1_elo)/400.0 ) );
    double E_p2 = 1.0 - E_p1;

    //winner_elo_diff = static_cast<int>( 0.5 + static_cast<double>(k) * (1.0 - E_winner) );
    //loser_elo_diff = -(k - winner_elo_diff);

    //winner_elo_diff = static_cast<int>( 0.5 + static_cast<double>(k) * (1.0 - E_winner) );
    //loser_elo_diff = -(k - winner_elo_diff);

    p1_elo_diff = static_cast<double>(k) * ( p1_win - E_p1 );
    p2_elo_diff = static_cast<double>(k) * ( p2_win - E_p2 );
}

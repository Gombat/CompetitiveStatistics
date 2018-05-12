#include "sessioneditpagematches.h"
#include "sessioneditwizard.h"
#include "database.h"

#include <QStringList>
#include <QVariant>
#include <QMessageBox>

SessionEditPageMatches::SessionEditPageMatches(QWidget *parent, QString session)
 : QWizardPage(parent), m_session_name_init(session)
{ }

void SessionEditPageMatches::initializePage()
{
    setTitle( "Edit Session - Matches" );

    m_matches = new QTableWidget(this);
    m_matches->setColumnCount(6);
    m_matches->setHorizontalHeaderLabels(QStringList() <<
        "round" <<  "order" << "p1" << "p2" << "p1_wins" << "p2_wins");
    m_matches->setColumnHidden(0,true);
    m_matches->setColumnHidden(1,true);

    m_layout = new QGridLayout(this);
    m_layout->addWidget(m_matches,0,0);

    fillTable(m_session_name_init);
}

void SessionEditPageMatches::fillTable(const QString& session_name)
{
    SessionEditPagePlayers* pagePlayers = qobject_cast<SessionEditPagePlayers*>(
        wizard()->page(SessionEditWizard::PageID::Players));
    if ( !pagePlayers ) return;

    SessionEditPageSession* pageSession = qobject_cast<SessionEditPageSession*>(
        wizard()->page(SessionEditWizard::PageID::Session));
    if ( !pageSession ) return;

    QStringList players = pagePlayers->players();

//    // delete old rows if player does not exist anymore
//    for ( int i = 0; i < m_matches->rowCount(); i++ )
//    {
//        MatchData matchDate(
//            m_matches->item(i,0)->text().toInt(), // round
//            m_matches->item(i,1)->text().toInt(), // suggested play order
//            m_matches->item(i,2)->text(),
//            m_matches->item(i,3)->text(),
//            m_matches->item(i,4)->text().toInt(),
//            m_matches->item(i,5)->text().toInt() );
//        if ( !players.contains(matchDate.p1.name,Qt::CaseInsensitive) ||
//             !players.contains(matchDate.p2.name,Qt::CaseInsensitive) )
//        { m_matches->removeRow(i); i--; }
//    }

//    // actually fill table with existing players and database match data if exists
//    //if ( pageSession->sessionType() == SessionType::RoundRobin )
//    {
//        int count = 0;
//        for ( int i = 0; i < players.size(); i++ )
//        {
//            const QString p1 = players.at(i);
//            for ( int j = i + 1; j < players.size(); j++ )
//            {
//                int round = 0;
//                quint32 suggested_play_order = 0;
//                const QString p2 = players.at(j);
//                int p1_wins = 0;
//                int p2_wins = 0;
//                bool bMatchExists = Database::matchResults(
//                    session_name, p1, p2, round, suggested_play_order, p1_wins, p2_wins );
//                if ( !bMatchExists && pageSession->sessionType() != SessionType::RoundRobin )
//                { continue; }

//                if ( m_matches->rowCount() <= count )
//                { m_matches->insertRow(m_matches->rowCount()); }
//                m_matches->setItem(count,0,new QTableWidgetItem(QString("%1").arg(round)));
//                m_matches->setItem(count,1,new QTableWidgetItem(QString("%1").arg(suggested_play_order)));
//                m_matches->setItem(count,2,new QTableWidgetItem(p1));
//                m_matches->setItem(count,3,new QTableWidgetItem(p2));
//                m_matches->setItem(count,4,new QTableWidgetItem(QString("%1").arg(p1_wins)));
//                m_matches->item(count,4)->setFlags(
//                    m_matches->item(count,4)->flags() | Qt::ItemIsEditable);
//                m_matches->setItem(count,5,new QTableWidgetItem(QString("%1").arg(p2_wins)));
//                m_matches->item(count,5)->setFlags(
//                    m_matches->item(count,5)->flags() | Qt::ItemIsEditable);
//                count++;
//            }
//        }
//    }
//    //else if ( )
//    //bool Database::matchResults( const QString& session_name,
//    //                             std::vector<MatchData>& match_data)

    std::vector<MatchData> match_data;
    if ( !Database::matchResults( session_name, match_data) )
    {
        qDebug() << "Could not obtain matchResults for Session " << session_name;
        return;
    }
    for ( int i = 0; i < match_data.size(); i++ )
    {
        m_matches->insertRow(m_matches->rowCount());
        m_matches->setItem(i,0,new QTableWidgetItem(QString("%1").arg(match_data[i].round)));
        m_matches->setItem(i,1,new QTableWidgetItem(QString("%1").arg(match_data[i].suggested_play_order)));
        m_matches->setItem(i,2,new QTableWidgetItem(match_data[i].p1.name));
        m_matches->setItem(i,3,new QTableWidgetItem(match_data[i].p2.name));
        m_matches->setItem(i,4,new QTableWidgetItem(QString("%1").arg(match_data[i].p1_wins)));
        m_matches->item(i,4)->setFlags(
            m_matches->item(i,4)->flags() | Qt::ItemIsEditable);
        m_matches->setItem(i,5,new QTableWidgetItem(QString("%1").arg(match_data[i].p2_wins)));
        m_matches->item(i,5)->setFlags(
            m_matches->item(i,5)->flags() | Qt::ItemIsEditable);
    }
}

std::vector<MatchData> SessionEditPageMatches::matchData() const
{
    std::vector<MatchData> matchData;
    for ( int i = 0; i < m_matches->rowCount(); i++ )
    {
        MatchData matchDate(
            m_matches->item(i,0)->text().toInt(), // round
            m_matches->item(i,1)->text().toInt(), // suggested_play_order
            m_matches->item(i,2)->text(),
            m_matches->item(i,3)->text(),
            m_matches->item(i,4)->text().toInt(),
            m_matches->item(i,5)->text().toInt() );
        matchData.push_back(matchDate);
    }
    return matchData;
}

bool SessionEditPageMatches::validatePage()
{
    SessionEditPageSession* pageSession = qobject_cast<SessionEditPageSession*>(
        wizard()->page( SessionEditWizard::PageID::Session ) );

    if ( !pageSession )
    { QMessageBox::critical(this, "Internal Error", "Session Page invalid!" ); return false; }

    bool completed = true;

    for ( int i = 0; i < m_matches->rowCount(); i++ )
    {
        MatchData matchDate(
            m_matches->item(i,0)->text().toInt(), // round
            m_matches->item(i,1)->text().toInt(), // suggested_play_order
            m_matches->item(i,2)->text(),
            m_matches->item(i,3)->text(),
            m_matches->item(i,4)->text().toInt(),
            m_matches->item(i,5)->text().toInt() );

        m_matches->item(i,4)->setBackgroundColor(QColor(Qt::green).lighter());
        m_matches->item(i,5)->setBackgroundColor(QColor(Qt::green).lighter());

//        int winsCount = matchDate.p1_wins + matchDate.p2_wins;
//        if ( winsCount == 0 )
//        {
//            completed = false;
//            m_matches->item(i,4)->setBackgroundColor(QColor(Qt::red).lighter());
//            m_matches->item(i,5)->setBackgroundColor(QColor(Qt::red).lighter());
//        }
    }
    if( !completed )
    {
        QMessageBox::StandardButton button = QMessageBox::question( this, "Incomplete Matches Left",
            "Are you sure you want to continue? There are incomplete matches left.",
            QMessageBox::Yes | QMessageBox::No );
        if ( button == QMessageBox::No ) return false;
    }
    return true;
}

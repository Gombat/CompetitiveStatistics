#include "sessioneditpageoverview.h"
#include "sessioneditwizard.h"

#include "database.h"

#include <QMessageBox>

SessionEditPageOverview::SessionEditPageOverview(QWidget *parent, QString session)
 : QWizardPage(parent), m_session_name_init(session)
{ }

void SessionEditPageOverview::initializePage()
{
    setTitle( "Edit Session - Complete" );
    setFinalPage(true);

    m_label = new QLabel("Editing session finished!");
    m_layout = new QGridLayout(this);
    m_layout->addWidget(m_label);

    SessionEditPageSession* pageSession = qobject_cast<SessionEditPageSession*>(
        wizard()->page(SessionEditWizard::PageID::Session));
    QString session_name = m_session_name_init;
    if ( pageSession ){ session_name = pageSession->sessionName(); }
    m_label->setText(QString("Editing session \"%1\" finished!").arg(session_name));
}

bool SessionEditPageOverview::validatePage()
{
    // TODO export to database

    SessionEditPageSession* pageSession = qobject_cast<SessionEditPageSession*>(
        wizard()->page(SessionEditWizard::PageID::Session));
    SessionEditPagePlayers* pagePlayers = qobject_cast<SessionEditPagePlayers*>(
                wizard()->page(SessionEditWizard::PageID::Players));
    SessionEditPageMatches* pageMatches = qobject_cast<SessionEditPageMatches*>(
                wizard()->page(SessionEditWizard::PageID::Matches));

    if ( !pageSession || !pagePlayers || !pageMatches )
    { QMessageBox::critical(this, "Internal Error", "Session Page invalid!" ); return false; }

    //Database::db().transaction();

    // add session if not existed
    int session_id = pageSession->sessionID();
    const QString session_name = pageSession->sessionName();
    const int session_challonge_id = pageSession->sessionChallongeID();
    const int session_tournament_id = pageSession->sessionTournamentID();
    if ( pageSession->sessionExisted() )
    {
        if ( !Database::updateSession( session_id,
            session_name, session_challonge_id, session_tournament_id,
            pageSession->sessionDate(), pageSession->sessionURL() ) )
        {
            QMessageBox::critical(this,"Internal Error","Cannot update session data!");
            //Database::db().rollback();
            return false;
        }
    }
    else
    {
        if ( !Database::addSession( session_name,
            session_challonge_id, session_tournament_id,
            pageSession->sessionDate(), pageSession->sessionURL() ) )
        {
            QMessageBox::critical(this,"Internal Error","Cannot add session data!");
            //Database::db().rollback();
            return false;
        }
        session_id = Database::sessionID( pageSession->sessionName() );
    }

    //add players if available
    QStringList players = pagePlayers->players();
    for ( int i = 0; i < players.count(); i++ )
    {
        const QString& player = players.at(i);
        if ( !Database::playerExists(player) )
        {
            if ( !Database::addPlayer(player) )
            {
                QMessageBox::critical(this,"Internal Error","Cannot add player data!");
                //Database::db().rollback();
                return false;
            }
        }
    }

    // add or update match data
    const std::vector<MatchData>& matches = pageMatches->matchData();
    for ( size_t i = 0; i < matches.size(); i++ )
    {
        const MatchData& matchDate = matches[i];
        if ( !Database::addOrUpdateMatch(session_name,
            session_challonge_id, session_tournament_id,
            matchDate.p1.tag, matchDate.p1.name, matchDate.p2.tag, matchDate.p2.name,
            matchDate.round, matchDate.suggested_play_order, matchDate.p1_wins, matchDate.p2_wins) )
        {
//            QMessageBox::critical(this,"Internal Error","Cannot add match data!");
//            //Database::db().rollback();
//            return false;
            qDebug() << "Not adding match of " <<
                        matchDate.p1.name << " and " << matchDate.p2.name;
        }
    }

    Database::db().commit();

    return true;
}

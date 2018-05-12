#include "sessioneditpagesession.h"
#include "database.h"

#include <QMessageBox>
#include <limits>

SessionEditPageSession::SessionEditPageSession(QWidget *parent, QString session)
 : QWizardPage(parent), m_session_id_init(-1), m_session_name_init(session),
   m_session_challonge_id_init(-1), m_session_tournament_id_init(-1)
{
    if ( !m_session_name_init.isEmpty() )
    {
        m_session_id_init = Database::sessionID(m_session_name_init);
        m_session_challonge_id_init = Database::sessionChallongeID(m_session_name_init);
        m_session_tournament_id_init = Database::sessionTournamentID(m_session_name_init);
    }
}

void SessionEditPageSession::initializePage()
{
    setTitle( "Edit Session - Session" );

    m_label_session_name = new QLabel("Session Name",this);
    m_label_session_challonge_id = new QLabel("Session Challonge ID",this);
    m_label_session_tournament_name = new QLabel("Session Tournament Name",this);
    m_label_calendar = new QLabel("Session Date",this);
    m_label_url = new QLabel("Session URL",this);

    m_edit_session_name = new QLineEdit("",this);

    m_edit_session_challonge_id = new QSpinBox(this);
    m_edit_session_challonge_id->setMinimum(0);
    m_edit_session_challonge_id->setMaximum(std::numeric_limits<int>::max());
    m_edit_session_challonge_id->setValue(m_session_challonge_id_init);

    m_edit_session_tournament_name = new QComboBox(this);
    QStringList list_tournaments = Database::tournaments();
    list_tournaments.push_front("new...");
    m_edit_session_tournament_name->addItems(list_tournaments);

    QString session_tournament_name = Database::tournamentName(m_session_tournament_id_init);
    int tournament_name_index = m_edit_session_tournament_name->findText( session_tournament_name );
    if ( tournament_name_index >= 0 )
        m_edit_session_tournament_name->setCurrentIndex(tournament_name_index);

    m_edit_url = new QLineEdit("",this);
    //m_calendar = new QCalendarWidget(this);
    m_dateEdit = new QDateEdit(this);

    //m_calendar->setDateTextFormat("");

    if ( !m_session_name_init.isEmpty() )
    {
        m_edit_session_name->setText(m_session_name_init);

//        QTextCharFormat currentDayHighlight;
//        currentDayHighlight.setBackground(Qt::cyan);
//        m_date = QDate::fromString(
//            Database::sessionDate(session_name), "yyyy-MM-dd");
//        m_calendar->setDateTextFormat(m_date, currentDayHighlight);
        m_dateEdit->setDate(QDate::fromString(
            Database::sessionDate(m_session_name_init), "yyyy-MM-dd"));

        m_edit_url->setText(Database::sessionURL(m_session_name_init));
    }
    else
    {
        m_dateEdit->setDate(QDate::currentDate());
    }

    m_layout = new QGridLayout(this);
    m_layout->addWidget(m_label_session_name,0,0);
    m_layout->addWidget(m_label_session_challonge_id,1,0);
    m_layout->addWidget(m_label_session_tournament_name,2,0);
    m_layout->addWidget(m_label_calendar,3,0);
    m_layout->addWidget(m_label_url,4,0);
    m_layout->addWidget(m_edit_session_name,0,1);
    m_layout->addWidget(m_edit_session_challonge_id,1,1);
    m_layout->addWidget(m_edit_session_tournament_name,2,1);
    //m_layout->addWidget(m_calendar,3,1);
    m_layout->addWidget(m_dateEdit,3,1);
    m_layout->addWidget(m_edit_url,4,1);

    //this->setLayout(m_layout);

//    m_calendar->setDateEditEnabled(true);
//    QObject::connect(m_calendar,SIGNAL(selectionChanged()),
//                     this,SLOT(onSelectionChanged()));
}

int SessionEditPageSession::sessionID() const
{
    return m_session_id_init;
}

QString SessionEditPageSession::sessionName() const
{
    return m_edit_session_name->text();
}

int SessionEditPageSession::sessionChallongeID() const
{
    return m_edit_session_challonge_id->value();
}
int SessionEditPageSession::sessionTournamentID() const
{
    return Database::tournamentID(m_edit_session_tournament_name->currentText());
}

QString SessionEditPageSession::sessionDate() const
{
    //return m_calendar->selectedDate().toString("yyyy-MM-dd");
    return m_dateEdit->date().toString("yyyy-MM-dd");
}

QString SessionEditPageSession::sessionURL() const
{
    return m_edit_url->text();
}

//void SessionEditPageSession::onSelectionChanged()
//{
//    QTextCharFormat defaultDateFormat;
//    m_calendar->setDateTextFormat(m_date, defaultDateFormat);

//    m_date = m_calendar->selectedDate();
//    QTextCharFormat currentDayHighlight;
//    currentDayHighlight.setBackground(Qt::cyan);
//    m_calendar->setDateTextFormat(m_date, currentDayHighlight);
//}

bool SessionEditPageSession::validatePage()
{
    if ( sessionName().compare(m_session_name_init,Qt::CaseInsensitive) != 0 )
    {
        if ( Database::sessionExists( sessionName()) )
        {
            QMessageBox::warning(this, "Session Name",
                QString("Session \"%1\" already exists! "
                "Select the existing session and edit if you want to. ")
                .arg(sessionName() ) );
            return false;
        }
    }
    return true;
}

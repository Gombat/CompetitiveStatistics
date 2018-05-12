#ifndef SESSIONEDITPAGESESSION_H
#define SESSIONEDITPAGESESSION_H

#include <QWizardPage>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QCalendarWidget>
#include <QDateEdit>

#include "data.h"

class SessionEditPageSession : public QWizardPage
{
    Q_OBJECT
public:
    explicit SessionEditPageSession(QWidget *parent = 0, QString session = "");

    bool sessionExisted() const { return sessionID() >= 0; }
    int sessionID() const;
    QString sessionName() const;
    int sessionChallongeID() const;
    int sessionTournamentID() const;
    QString sessionDate() const;
    QString sessionURL() const;

signals:

private slots:
//    void onSelectionChanged();

protected:
    virtual void initializePage();
    virtual bool validatePage();

private:
    //QDate m_date;
    int m_session_id_init;
    int m_session_challonge_id_init;
    int m_session_tournament_id_init;

    QString m_session_name_init;
    QGridLayout* m_layout;
    QLabel* m_label_session_name;
    QLineEdit* m_edit_session_name;

    QLabel* m_label_session_challonge_id;
    QSpinBox* m_edit_session_challonge_id;

    QLabel* m_label_session_tournament_name;
    QComboBox* m_edit_session_tournament_name;

    QLabel* m_label_calendar;
    QCalendarWidget *m_calendar;
    QDateEdit* m_dateEdit;
    QLabel* m_label_url;
    QLineEdit* m_edit_url;
};

#endif // SESSIONEDITPAGESESSION_H

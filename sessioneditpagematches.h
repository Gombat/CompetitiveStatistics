#ifndef SESSIONEDITPAGEMATCHES_H
#define SESSIONEDITPAGEMATCHES_H

#include <QWizardPage>
#include <QTableWidget>
#include <QGridLayout>
#include "data.h"

class SessionEditPageMatches : public QWizardPage
{
    Q_OBJECT
public:
    explicit SessionEditPageMatches(QWidget *parent = 0, QString session = "");

    std::vector<MatchData> matchData() const;

signals:

public slots:

    void fillTable(const QString& session_name);

protected:
    virtual void initializePage();
    virtual bool validatePage();

private:

    QString m_session_name_init;
    QTableWidget* m_matches;
    QGridLayout* m_layout;
};

#endif // SESSIONEDITPAGEMATCHES_H

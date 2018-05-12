#ifndef SESSIONEDITPAGEOVERVIEW_H
#define SESSIONEDITPAGEOVERVIEW_H

#include <QWizardPage>
#include <QGridLayout>
#include <QLabel>

class SessionEditPageOverview : public QWizardPage
{
    Q_OBJECT
public:
    explicit SessionEditPageOverview(QWidget *parent = 0, QString session = "");

signals:

public slots:

protected:
    virtual void initializePage();
    virtual bool validatePage();

private:
    QString m_session_name_init;
    QGridLayout* m_layout;
    QLabel* m_label;
};

#endif // SESSIONEDITPAGEOVERVIEW_H

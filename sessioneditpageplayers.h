#ifndef SESSIONEDITPAGEPLAYERS_H
#define SESSIONEDITPAGEPLAYERS_H

#include <QWizardPage>
#include <QListWidget>
#include <QGridLayout>
#include <QPushButton>

class SessionEditPagePlayers : public QWizardPage
{
    Q_OBJECT
public:
    explicit SessionEditPagePlayers(QWidget *parent = 0, QString session = "");

    QStringList players() const;

signals:

public slots:

private slots:
    void onAdd(bool);
    void onDel(bool);

protected:
    virtual void initializePage();
    virtual bool validatePage();

private:
    QString m_session_name_init;
    QGridLayout* m_layout;
    QListWidget* m_list;
    QPushButton* m_add;
    QPushButton* m_del;
};

#endif // SESSIONEDITPAGEPLAYERS_H

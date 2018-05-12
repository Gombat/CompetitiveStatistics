#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "database.h"
#include "challongeimporter.h"
#include "sessioneditwizard.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void importFile();

    void editSession_p(const QString& session_name);
    void onEditSession();
    void onAddSession();
    void onDeleteSession();
    void onExport();
    void onFinishedSession();

    //void updateStandings(QString session_name);
    void updateEloTable();
    void updatePVPTable(const std::vector<EloData>& vecEloData);

private:
    Ui::MainWindow *ui;

    void createActions();
    void createMenu();

    void selectNewSession(const QStringList& sessions_before,
                          const QStringList& sessions_after);

    void importChallongeFile( const QString& file );

    QMenu* fileMenu;
    QAction *importAct;

    ChallongeImporter* m_challongeImporter;
    SessionEditWizard* m_sessionEditWizard;
};

#endif // MAINWINDOW_H

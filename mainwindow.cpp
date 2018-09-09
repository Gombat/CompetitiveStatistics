#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QTextStream>
#include <QKeySequence>
#include <QMessageBox>
#include <QInputDialog>
#include <fstream>

MainWindow::MainWindow(QWidget *parent)
 : QMainWindow(parent)
 , ui(new Ui::MainWindow)
 , m_challongeImporter(new ChallongeImporter(this))
// , m_sessionEditWizard(0)
 {
    ui->setupUi(this);

    this->setWindowTitle( "CompitetiveStatistics" );
    this->setWindowIcon(QIcon(":/res/Symbol.jpg"));

    ui->tabWidget->setTabText(0, "Elo");
    ui->tabWidget->setTabText(1, "PVP");

    createActions();
    createMenu();

    Database::initDB();

    // testing
    QString path = "C:/Users/Micha/workspace/Smash/CompitetiveStatistics/challonge/";

//    m_challongeImporter->importFile( path + "dd62_amateur.json", "dd62" );
//    m_challongeImporter->importFile( path + "dd62_pool_redfield.json", "dd62" );
//    m_challongeImporter->importFile( path + "dd62_pool_wesker.json", "dd62" );
//    m_challongeImporter->importFile( path + "dd62_pro.json", "dd62" );

//    m_challongeImporter->importFile( path + "dd63_amateur.json", "dd63" );
//    m_challongeImporter->importFile( path + "dd63_cyclops.json", "dd63" );
//    m_challongeImporter->importFile( path + "dd63_pro.json", "dd63" );
//    m_challongeImporter->importFile( path + "dd63_wolverine.json", "dd63" );

//    m_challongeImporter->importFile( path + "dd64_pro.json", "dd64" );

//    m_challongeImporter->importFile( path + "dd65_AmA.json", "dd65" );
//    m_challongeImporter->importFile( path + "dd65_Knochentrocken.json", "dd65" );
//    m_challongeImporter->importFile( path + "dd65_pro.json", "dd65" );
//    m_challongeImporter->importFile( path + "dd65_SuperGoomba.json", "dd65" );

//    m_challongeImporter->importFile( path + "dd66_singles.json", "dd66" );

//    m_challongeImporter->importFile( path + "dd67_ama.json", "dd67" );
//    m_challongeImporter->importFile( path + "dd67_pools_pro.json", "dd67" );

//    m_challongeImporter->importFile( path + "dd68_Amateur.json", "dd68" );
//    m_challongeImporter->importFile( path + "dd68_buuhuu.json", "dd68" );
//    m_challongeImporter->importFile( path + "dd68_gkoopa.json", "dd68" );
//    m_challongeImporter->importFile( path + "dd68_knochentrocken.json", "dd68" );
//    m_challongeImporter->importFile( path + "dd68_pro.json", "dd68" );
//    m_challongeImporter->importFile( path + "dd68_womp.json", "dd68" );

    //QStringList test = Database::sessions();
    //ui->comboBoxSession->addItem("new Session...");
    ui->comboBoxSession->addItems(Database::sessions());

    updateEloTable();

//    if ( ui->comboBoxSession->count() > 0)
//    { updateStandings(ui->comboBoxSession->currentText()); }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createActions()
{
    importAct = new QAction(tr("&Import"), this);
    importAct->setShortcut(QKeySequence("Ctrl+I"));
    importAct->setShortcut(QKeySequence("Ctrl+I"));
    importAct->setStatusTip(tr("Create a new file"));
    connect(importAct, &QAction::triggered, this, &MainWindow::importFile);
}

void MainWindow::createMenu()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(importAct);
}

void MainWindow::importFile()
{
    QString challongeFile = QFileDialog::getOpenFileName(
        this, "Import challonge export", QApplication::instance()->applicationDirPath() );

    QStringList sessions_before = Database::sessions();
    importChallongeFile(challongeFile);
    QStringList sessions_after  = Database::sessions();

    selectNewSession( sessions_before, sessions_after );
}

void MainWindow::selectNewSession(const QStringList& sessions_before,
                                  const QStringList& sessions_after)
{
    if ( sessions_after.count() > sessions_before.count() )
    {
        QString session_new("");
        for ( int i = 0; i < sessions_after.count(); i++ )
        {
            const QString& session_cur = sessions_after.at(i);
            if ( !sessions_before.contains(session_cur) )
            { session_new = session_cur; break; }
        }
        if ( !session_new.isEmpty() )
        {
            ui->comboBoxSession->clear();
            ui->comboBoxSession->addItems(Database::sessions());
            //updateStandings(session_new);
        }
    }
}

void MainWindow::importChallongeFile( const QString& file )
{
    QFile f(file);
    if ( !f.open(QFile::ReadOnly | QFile::Text)) return;
    QTextStream in(&f);
    in.setCodec("UTF-8");
    QString text = in.readAll();
    //qDebug() << f.size() << text;

    QStringList list_tournaments = Database::tournaments();
    list_tournaments.push_front("new...");

    QString session_tournament_name = QInputDialog::getItem( this,
        "Tournament", "Choose Session Tournament",
        list_tournaments, list_tournaments.size()-1 );

    if ( session_tournament_name.compare( "new..." ) == 0 )
    {
        session_tournament_name = QInputDialog::getText(this, "New Tournament", "New Tournament Name");
        if ( !Database::addTournament(session_tournament_name) )
        {
            QMessageBox::warning( this, "New Tournament", "Adding new tournament failed!");
            return;
        }
    }

    m_challongeImporter->import( text, session_tournament_name );
}

void MainWindow::editSession_p(const QString& session_name)
{
    m_sessionEditWizard =
        new SessionEditWizard(this, session_name);
    m_sessionEditWizard->setWindowTitle("Edit Session Wizard");
    m_sessionEditWizard->initialize();
    QObject::connect(m_sessionEditWizard,SIGNAL(accepted()),
                     this,SLOT(onFinishedSession()));
    QStringList sessions_before = Database::sessions();
    m_sessionEditWizard->exec();
    QStringList sessions_after = Database::sessions();
    selectNewSession( sessions_before, sessions_after );
}

void MainWindow::onEditSession()
{
    if ( ui->comboBoxSession->count() > 0)
    { editSession_p(ui->comboBoxSession->currentText()); }

    updateEloTable();
}

void MainWindow::onAddSession()
{
    //editSession_p("");

    QString file = QFileDialog::getOpenFileName(this, "Challonge Export File");

    if ( file.isEmpty() ) return;

    importChallongeFile(file);
    ui->comboBoxSession->clear();
    ui->comboBoxSession->addItems(Database::sessions());

    updateEloTable();
}

void MainWindow::onDeleteSession()
{
    QString session_name = ui->comboBoxSession->currentText();
    if ( ui->comboBoxSession->count() > 0)
    { Database::deleteSession(session_name); }
    ui->comboBoxSession->removeItem(ui->comboBoxSession->currentIndex());
//    if ( ui->comboBoxSession->count() == 0 )
//    { ui->tableWidgetStandings->clear(); }

    updateEloTable();
}

void MainWindow::onExport()
{
    if (ui->tabWidget->currentIndex() == 0)
    {
        QString exportFile = QFileDialog::getSaveFileName(
            this, "Export elo table as CSV",
            QApplication::instance()->applicationDirPath(), "*.csv");

        std::ofstream ofs(exportFile.toStdString().c_str(), std::ofstream::out);
        ofs << "Rank;Name;Elo;Set Wins;Set Losses;Set Win Rate;Game Wins;Game Losses;Game Win Rate;Attendances" << std::endl;
        for( int i = 0; i < ui->tableWidgetElo->rowCount(); i++ )
        {
            QString sets = ui->tableWidgetElo->item(i,3)->text();
            QStringList sets_list = sets.split("-");
            int set_wins = sets_list.at(0).toInt();
            int set_losses = sets_list.at(1).toInt();
            QString matches = ui->tableWidgetElo->item(i,5)->text();
            QStringList matches_list = matches.split("-");
            int match_wins = matches_list.at(0).toInt();
            int match_losses = matches_list.at(1).toInt();

            ofs << ui->tableWidgetElo->item(i,0)->text().toStdString() << ";" // rank
                << ui->tableWidgetElo->item(i,1)->text().toStdString() << ";" // name
                << ui->tableWidgetElo->item(i,2)->text().toStdString() << ";" // elo
                << set_wins << ";"
                << set_losses << ";"
                << ui->tableWidgetElo->item(i,4)->text().toStdString() << ";" // set win rate
                << match_wins << ";"
                << match_losses << ";"
                << ui->tableWidgetElo->item(i,6)->text().toStdString() << ";" // match win rate
                << ui->tableWidgetElo->item(i,7)->text().toStdString() << std::endl; // attendances

        }
        ofs.close();
    }
    else if (ui->tabWidget->currentIndex() == 1)
    {
        QString exportFile = QFileDialog::getSaveFileName(
            this, "Export PVP table as CSV",
            QApplication::instance()->applicationDirPath(), "*.csv");

        std::ofstream ofs(exportFile.toStdString().c_str(), std::ofstream::out);
        for( int i = 0; i < ui->tableWidgetElo->rowCount(); i++ )
        {
            ofs << ";" << ui->tableWidgetElo->item(i,1)->text().toStdString();
        }
//        for ( int i = 0; i < ui->tableWidgetPVP->columnCount(); i++ )
//        {
//            ofs << ui->tableWidgetPVP->horizontalHeaderItem(i)->text().toStdString() << ";";
//        }

        ofs << std::endl;

        for( int i = 0; i < ui->tableWidgetPVP->rowCount(); i++ )
        {
            ofs << ui->tableWidgetElo->item(i,1)->text().toStdString() << ";";

            for ( int j = 0; j < ui->tableWidgetPVP->columnCount(); j++ )
            {
                if ( ui->tableWidgetPVP->item(i,j) )
                {
                    ofs << ui->tableWidgetPVP->item(i,j)->text().replace("\n"," | ").toStdString() << ";";
                }
                else
                    ofs << ";";
            }
            ofs << std::endl;
        }
        ofs.close();
    }
    else
        QMessageBox::critical(this, "Not Available", "Export for this tab is not yet available!");
}

void MainWindow::onFinishedSession()
{
//    if ( ui->comboBoxSession->count() )
//    { updateStandings(ui->comboBoxSession->currentText()); }

    updateEloTable();
}

void MainWindow::updateEloTable()
{
    Database::updateEloRating();

    if ( ui->comboBoxSession->count() == 0 )
    { ui->tableWidgetElo->clear(); return; }

    std::vector<EloData> vecEloData;
    if ( !Database::eloData(vecEloData) )
        return;

    std::sort(vecEloData.begin(), vecEloData.end(), &EloData::sortingHelper);

    ui->tableWidgetElo->clear();
    ui->tableWidgetElo->setColumnCount(8);
    ui->tableWidgetElo->setHorizontalHeaderLabels(
        QStringList() << "Rank" << "Participant" << "Elo"
            << "Set W-L" << "Set Win Rate"
            << "Match W-L" << "Match Win Rate"
            << "attendances" );

    for ( size_t i = 0; i < vecEloData.size(); i++ )
    {
        const EloData& eloData = vecEloData[i];
        if ( eloData.set_wins + eloData.set_losses == 0 )
        {
            vecEloData.erase(vecEloData.begin()+i);
            i--;
        }
    }

    for ( size_t i = 0; i < vecEloData.size(); i++ )
    {
        const EloData& eloData = vecEloData[i];

        if ( static_cast<size_t>(ui->tableWidgetElo->rowCount()) <= i )
        { ui->tableWidgetElo->insertRow(ui->tableWidgetElo->rowCount()); }

        int rank = i+1;
        for( int j = i-1; j >= 0; j-- )
        {
            const EloData& prevEloData = vecEloData[j];
            if ( prevEloData.rating != eloData.rating )
                break;
            else
                rank--;
        }

        ui->tableWidgetElo->setItem(i,0,new QTableWidgetItem(QString("%1").arg(rank)));//i+1)));

        QString player_name = Database::playerName(eloData.player_id);
        ui->tableWidgetElo->setItem(i,1,new QTableWidgetItem(player_name));

        ui->tableWidgetElo->setItem(i,2,new QTableWidgetItem(
            QString("%1").arg(eloData.rating)));

        ui->tableWidgetElo->setItem(i,3,new QTableWidgetItem(
            QString("%1-%2").arg(eloData.set_wins).arg(eloData.set_losses)));

        float set_win_rate = 0.f;
        if ( eloData.set_wins + eloData.set_losses > 0 )
        {
            set_win_rate = static_cast<float>(eloData.set_wins) /
                static_cast<float>(eloData.set_wins + eloData.set_losses);
        }
        QString set_win_rate_str; set_win_rate_str.sprintf("%.2f%", set_win_rate * 100.f);
        ui->tableWidgetElo->setItem(i,4,new QTableWidgetItem(set_win_rate_str));

        ui->tableWidgetElo->setItem(i,5,new QTableWidgetItem(
            QString("%1-%2").arg(eloData.match_wins).arg(eloData.match_losses)));

        float match_win_rate = 0.f;
        if ( eloData.match_wins + eloData.match_losses > 0 )
        {
            match_win_rate = static_cast<float>(eloData.match_wins) /
                static_cast<float>(eloData.match_wins + eloData.match_losses);
        }
        QString match_win_rate_str; match_win_rate_str.sprintf("%.2f%", match_win_rate * 100.f);
        ui->tableWidgetElo->setItem(i,6,new QTableWidgetItem(match_win_rate_str));

        ui->tableWidgetElo->setItem(i,7,new QTableWidgetItem(
            QString("%1").arg(eloData.attendances)));
    }

    updatePVPTable(vecEloData);

}

void MainWindow::updatePVPTable(const std::vector<EloData>& vecEloData)
{
    // ui->tableWidgetPVP

    ui->tableWidgetPVP->clear();
    ui->tableWidgetPVP->setColumnCount(vecEloData.size());
    ui->tableWidgetPVP->setRowCount(vecEloData.size());

    QStringList tableLabels;
    for ( size_t i = 0; i < vecEloData.size(); i++ )
    {
        const EloData& eloData = vecEloData[i];
        tableLabels << Database::playerName(eloData.player_id);
    }
    ui->tableWidgetPVP->setHorizontalHeaderLabels(tableLabels);
    ui->tableWidgetPVP->setVerticalHeaderLabels(tableLabels);


    int p1_set_wins;
    int p2_set_wins;
    int p1_match_wins;
    int p2_match_wins;

    //ui->tableWidgetPVP->setItem(0,0, new QTableWidgetItem(QString("AHA")));
    for ( size_t i = 0; i < vecEloData.size(); i++ )
    {
        for ( size_t j = 0; j < vecEloData.size(); j++ )
        {
            if ( i == j )
            {
                ui->tableWidgetPVP->setItem(i,j, new QTableWidgetItem(QString("X")));
                continue;
            }
            if ( j > i )
                continue;

            QString p1_tag = Database::playerName(vecEloData[i].player_id);
            QString p2_tag = Database::playerName(vecEloData[j].player_id);

            if ( Database::matchResults( p1_tag,
                                         p2_tag,
                                         p1_set_wins,
                                         p2_set_wins,
                                         p1_match_wins,
                                         p2_match_wins) )
            {
                if ( p1_set_wins > 0 || p2_set_wins > 0 || p1_match_wins > 0 || p2_match_wins > 0 )
                {
                    ui->tableWidgetPVP->setItem(i,j, new QTableWidgetItem(QString("S: %1 - %2\nG: %3 - %4")
                                                                          .arg(p1_set_wins).arg(p2_set_wins)
                                                                          .arg(p1_match_wins).arg(p2_match_wins)));
                    ui->tableWidgetPVP->setItem(j,i, new QTableWidgetItem(QString("S: %2 - %1\nG: %4 - %3")
                                                                          .arg(p1_set_wins).arg(p2_set_wins)
                                                                          .arg(p1_match_wins).arg(p2_match_wins)));
                }
            }
        }
    }

    ui->tableWidgetPVP->verticalHeader()->resizeSections(QHeaderView::ResizeToContents);
    ui->tableWidgetPVP->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}

//void MainWindow::updateStandings(QString session_name)
//{
//    int a = 0;
//    if ( ui->comboBoxSession->count() == 0 )
//    { ui->tableWidgetStandings->clear(); return; }

//    //QStringList() << "Rank" << "Participant" << "Set W-L" << "Match W-L" );
//    ui->tableWidgetStandings->clear();
//    ui->tableWidgetStandings->setHorizontalHeaderLabels(
//        QStringList() << "Rank" << "Participant" << "Set W-L" << "Match W-L" );
//    ui->tableWidgetStandings->setColumnCount(4);

//    // ----------------------------------------------------------------------------------
//    if ( Database::sessionType(session_name) == SessionType::RoundRobin )
//    {
//        std::vector<MatchData> match_data;
//        if ( !Database::matchResults( session_name, match_data ) )
//        {
//            QMessageBox::critical(this, "Update Standings Failed",
//                "Somehow looking up the single matches failed!" );
//            return;
//        }

//        QMap<QString,StandingData> standings_map;
//        fillStandingsMap( match_data, standings_map );

//        std::vector<StandingData> standings;
//        for ( QMap<QString,StandingData>::const_iterator it = standings_map.begin();
//              it != standings_map.end(); ++it )
//        { standings.push_back(*it); }
//        std::sort(standings.begin(), standings.end(), &StandingData::standingsSortingHelperRoundRobin);

//        for ( size_t i = 0; i < standings.size(); i++ )
//        {
//            const StandingData& standing = standings[i];
//            if ( static_cast<size_t>(ui->tableWidgetStandings->rowCount()) <= i )
//            { ui->tableWidgetStandings->insertRow(ui->tableWidgetStandings->rowCount()); }
//            ui->tableWidgetStandings->setItem(i,0,new QTableWidgetItem(QString("%1").arg(i+1)));
//            ui->tableWidgetStandings->setItem(i,1,new QTableWidgetItem(standing.player));
//            ui->tableWidgetStandings->setItem(i,2,new QTableWidgetItem(
//                QString("%1-%2").arg(standing.set_wins).arg(standing.set_losses)));
//            ui->tableWidgetStandings->setItem(i,3,new QTableWidgetItem(
//                QString("%1-%2").arg(standing.game_wins).arg(standing.game_losses)));
//        }
//    }
//    // ----------------------------------------------------------------------------------
//    else if ( Database::sessionType(session_name) == SessionType::Pools )
//    {
//        std::vector< std::vector< StandingData > > standings;

//        Database::obtainPools( session_name, standings );

//        for ( size_t i = 0; i < standings.size(); i++ )
//        {
//            std::vector< StandingData >& poolStanding = standings[i];
//            std::sort(poolStanding.begin(), poolStanding.end(),
//                      &StandingData::standingsSortingHelperRoundRobin);
//        }

//        ui->tableWidgetStandings->clear();
//        int count = 0;
//        for ( size_t i = 0; i < standings.size(); i++ )
//        {
//            const std::vector< StandingData >& poolStanding = standings[i];
//            for ( size_t j = 0; j < poolStanding.size(); j++ )
//            {
//                const StandingData& standing = poolStanding[j];
//                //if ( static_cast<size_t>(ui->tableWidgetStandings->rowCount()) <= i )
//                if ( ui->tableWidgetStandings->rowCount() < count )
//                { ui->tableWidgetStandings->insertRow(ui->tableWidgetStandings->rowCount()); }
//                ui->tableWidgetStandings->setItem(count,0,new QTableWidgetItem(QString("%1").arg(i+1)));
//                ui->tableWidgetStandings->setItem(count,1,new QTableWidgetItem(standing.player));
//                ui->tableWidgetStandings->setItem(count,2,new QTableWidgetItem(
//                    QString("%1-%2").arg(standing.set_wins).arg(standing.set_losses)));
//                ui->tableWidgetStandings->setItem(count,3,new QTableWidgetItem(
//                    QString("%1-%2").arg(standing.game_wins).arg(standing.game_losses)));
//                count++;
//            }
//            if ( ui->tableWidgetStandings->rowCount() < count )
//            {
//                ui->tableWidgetStandings->insertRow(ui->tableWidgetStandings->rowCount());
//                //QFrame* frame = new QFrame(this);
//                //frame->setFrameShape(QFrame::HLine);
//                QTableWidgetItem* item = new QTableWidgetItem();
//                item->setFlags( item->flags() & ~Qt::ItemIsEditable );
//                ui->tableWidgetStandings->setItem(count,0,item);
//                ui->tableWidgetStandings->setItem(count,1,item);
//                ui->tableWidgetStandings->setItem(count,2,item);
//                ui->tableWidgetStandings->setItem(count,3,item);
//            }
//            count++;
//        }


//    }
//    // ----------------------------------------------------------------------------------
//    else if ( Database::sessionType(session_name) == SessionType::SingleElimination )
//    {
//        // TODO
//    }
//    // ----------------------------------------------------------------------------------
//    else if ( Database::sessionType(session_name) == SessionType::DoubleElimination )
//    {
//        std::vector<MatchData> match_data;
//        if ( !Database::matchResults( session_name, match_data ) )
//        {
//            QMessageBox::critical(this, "Update Standings Failed",
//                "Somehow looking up the single matches failed!" );
//            return;
//        }

//        QMap<QString,StandingData> standings_map;
//        fillStandingsMap( match_data, standings_map );

//        std::vector<std::pair<size_t,StandingData> > standings;
//        {
////            for ( QMap<QString,StandingData>::const_iterator it = standings_map.begin();
////                  it != standings_map.end(); ++it )
////            { standings.push_back( std::make_pair( standings_map.size(), *it) ); }

//            // find last matches (gf and losers final)
//            int highest_winners_round = - std::numeric_limits<int>::max(),
//                highest_losers_round  = + std::numeric_limits<int>::max();
//            std::vector<int> highest_winners_round_indices;
//            int highest_winners_round_idx(-1), highest_losers_round_idx(-1);
//            for ( size_t i = 0; i < match_data.size(); i++ )
//            {
//                const MatchData& match_date = match_data[i];
//                if ( match_date.round > highest_winners_round )
//                {
//                    highest_winners_round = match_date.round;
//                }
//                if ( match_date.round < highest_losers_round  )
//                {
//                    highest_losers_round_idx = i;
//                    highest_losers_round  = match_date.round;
//                }
//            }

//            // check if there are 2 gf matches
//            for ( size_t j = 0; j < match_data.size(); j++ )
//            {
//                const MatchData& match_date = match_data[j];
//                if ( match_date.round == highest_winners_round )
//                { highest_winners_round_indices.push_back( j ); }
//            }
//            if ( highest_winners_round_indices.size() == 1 )
//            { highest_winners_round_idx = highest_winners_round_indices[0]; }
//            else
//            {
//                size_t suggested_play_order = 0;
//                for ( size_t k = 0; k < highest_winners_round_indices.size(); k++ )
//                {
//                    if ( match_data[highest_winners_round_indices[k]].suggested_play_order >
//                         suggested_play_order)
//                    {
//                        suggested_play_order =
//                            match_data[highest_winners_round_indices[k]].suggested_play_order;
//                        highest_winners_round_idx = highest_winners_round_indices[k];
//                    }
//                }
//            }

//            // fill first 3 placings
//            if ( highest_winners_round_idx >= 0 && highest_winners_round_idx < static_cast<int>(match_data.size()) )
//            {
//                standings.push_back( std::make_pair( 1,
//                    standings_map[match_data[highest_winners_round_idx].winner().name] ) );
//                standings.push_back( std::make_pair( 2,
//                    standings_map[match_data[highest_winners_round_idx].loser().name] ) );
//            }
//            if ( highest_losers_round_idx >= 0 && highest_losers_round_idx < static_cast<int>(match_data.size()) )
//            {
//                standings.push_back( std::make_pair( 3,
//                    standings_map[match_data[highest_losers_round_idx].loser().name] ) );
//            }

//            // find next placings from losers bracket
//            int next_round = highest_losers_round + 1;
//            int next_placing = 4;
//            for ( int i = 0; i < standings_map.size(); i++ )
//            {
//                std::vector<size_t> cur_round_indices;
//                for ( size_t j = 0; j < match_data.size(); j++ )
//                {
//                    const MatchData& match_date = match_data[j];
//                    if ( match_date.round == next_round )
//                    { cur_round_indices.push_back( j ); }
//                }

//                for ( size_t j = 0; j < cur_round_indices.size(); j++ )
//                {
//                    const size_t cur_idx = cur_round_indices[j];
//                    standings.push_back( std::make_pair( next_placing,
//                        standings_map[match_data[cur_idx].loser().name] ) );
//                }

//                next_placing = standings.size() + 1;
//                next_round++;
//                if ( next_round >= 0 ) break;
//            }
//        }

//        ui->tableWidgetStandings->clear();
//        int count = 0;
//        for ( size_t i = 0; i < standings.size(); i++ )
//        {
//            const std::pair<size_t,StandingData>& standing = standings[i];

//            //if ( static_cast<size_t>(ui->tableWidgetStandings->rowCount()) <= i )
//            if ( ui->tableWidgetStandings->rowCount() <= count )
//            { ui->tableWidgetStandings->insertRow(ui->tableWidgetStandings->rowCount()); }
//            ui->tableWidgetStandings->setItem(count,0,new QTableWidgetItem(
//                QString("%1").arg(static_cast<int>(standing.first))));
//            ui->tableWidgetStandings->setItem(count,1,new QTableWidgetItem(standing.second.player));
//            ui->tableWidgetStandings->setItem(count,2,new QTableWidgetItem(
//                QString("%1-%2").arg(standing.second.set_wins).arg(standing.second.set_losses)));
//            ui->tableWidgetStandings->setItem(count,3,new QTableWidgetItem(
//                QString("%1-%2").arg(standing.second.game_wins).arg(standing.second.game_losses)));

//            count++;
//        }
//    }
//    // ----------------------------------------------------------------------------------
//    else if ( Database::sessionType(session_name) == SessionType::PoolsAndSingleElimination )
//    {
//        // TODO
//    }
//    // ----------------------------------------------------------------------------------
//    else if ( Database::sessionType(session_name) == SessionType::PoolsAndDoubleElimination )
//    {
//        // TODO
//    }
//}


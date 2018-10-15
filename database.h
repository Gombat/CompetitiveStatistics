#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include "data.h"

class Database : public QObject
{
    Q_OBJECT
public:

    Database(QObject* parent = NULL);
    virtual ~Database(){}

    static QString gamerTagReplacements(const QMap<QString, QString> gamertag_replacements, const QString& player_name);

    static QSqlDatabase& db(const QString& db_path = "");

    static void initDB(const QString& db_path);

    static bool tournamentExists( const QString& name );
    static QStringList tournaments( );
    static bool addTournament( const QString& name );
    static int tournamentID( const QString& name );
    static QString tournamentName( const int id );

    static bool addPlayerToEloRating(const int player_id);
    static bool clearEloRating();
    static bool eloData(std::vector<EloData>& vecEloData);
    static bool eloData(std::map<int,EloData>& mapEloData);
    static void saveEloToTable( const std::map<int,EloData>& mapEloData );

    static bool playerExists( const QString& tag );
    static QStringList players( );
    static QStringList playerNames( );
    static QStringList players( const QString& session_name );
    static QString playerTag( const int player_id );
    static QString playerName( const int player_id );
    static int playerID( const QString& tag );

    // Returns false if a player with the same tag already exists
    static bool addPlayer( const QString& tag, const QString& name = "" );

    static bool sessionExists( const QString& name );
    static bool sessionExistsByChallongeID(const int challonge_id);
    static QStringList sessions( );
    static QStringList sessionsByTournamentID( const int tournament_id );

    // If date is left empty, current date will be used
    static bool addSession( const QString& name,
                            const int challonge_id,
                            const int tournament_id,
                            const QString& date = "",
                            const QString& url = "" );
    static bool updateSession( const int id,
                               const QString& name,
                               const int challonge_id,
                               const int tournament_id,
                               const QString& date,
                               const QString& url);
    static bool deleteSession( const QString& name );
    static int sessionChallongeID( const QString& session_name );
    static int sessionTournamentID( const QString& session_name );
    static int sessionID( const QString& session_name );
    static QString sessionDate( const QString& session_name );
    static QString sessionURL( const QString& session_name );

    static bool addOrUpdateMatch( const QString& session_name,
                                  const int challonge_id,
                                  const int session_tournament_id,
                                  const QString& p1_tag,
                                  const QString& p1_name,
                                  const QString& p2_tag,
                                  const QString& p2_name,
                                  const int round,
                                  const quint32 suggested_play_order,
                                  const int p1_wins,
                                  const int p2_wins);

    static bool matchResults( const QString& p1_tag,
                              const QString& p2_tag,
                              int& p1_set_wins,
                              int& p2_set_wins,
                              int& p1_match_wins,
                              int& p2_match_wins);

    static bool matchResults( const QString& session_name,
                              const QString& p1_tag,
                              const QString& p2_tag,
                              int& round,
                              quint32& suggested_play_order,
                              int& p1_wins,
                              int& p2_wins);
    static bool matchResults( const QString& session_name,
                              std::vector<MatchData>& match_data);

//    static void obtainPools( const QString& session_name,
//        std::vector< std::vector< StandingData > >& poolStandings );

    //static bool eloRating(const int player_id, int& elo);
    static void updateEloRating();

private:

    static void createTableConfig();
    static void createTablePlayers();
    static void createTableTournaments();
    static void createTableElo();
    static void createTableSessions();
    static void createTableMatches();

    static void setConfigVal(const QString name, const QVariant val);
    static bool getConfigVal(const QString name, QVariant& val);

    static bool updateEloRating(const int k,
                                const std::map<int,EloData>& mapEloData,
                                const MatchData match,
                                std::map<int,EloData> &mapEloDataWork);

    static void calcEloDiffs(const int k,
                             const int p1_elo, const int p2_elo,
                             int& p1_elo_diff, double p1_win,
                             int& p2_elo_diff, double p2_win);

    static bool matchResults_p( const QString& session_name,
                                const QString& p1,
                                const QString& p2,
                                int& round,
                                quint32& suggested_play_order,
                                int& p1_wins,
                                int& p2_wins,
                                quint32& match_id);
};

#endif // DATABASE_H

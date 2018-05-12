#ifndef DATA_H
#define DATA_H

#include <QObject>
#include <QString>
#include <QDebug>

struct PlayerData
{
    PlayerData() : id(-1)
    { }
    PlayerData(const QString tag_)
     : id(0), tag(tag_)
    { }
    PlayerData(const int id_,
               const QString tag_,
               const QString name_,
               const QSet<int> group_player_ids_ = QSet<int>())
     : id(id_), tag(tag_.toLower()), name(name_), group_player_ids(group_player_ids_)
    { }

    operator bool() const { return !tag.isEmpty(); }

    int id;
    QString tag;
    QString name;
    QSet< int > group_player_ids;
};

struct MatchData
{
    MatchData(const int round_,
              const quint32 suggested_play_order_,
              const PlayerData& p1_,
              const PlayerData& p2_,
              const int p1_wins_,
              const int p2_wins_ )
     : round(round_), suggested_play_order(suggested_play_order_),
       p1(p1_), p2(p2_),
       p1_wins(p1_wins_), p2_wins(p2_wins_) { }
    int round;
    quint32 suggested_play_order;
    PlayerData p1, p2;
    int p1_wins, p2_wins;

    const PlayerData& winner() const
    {
        if ( p1_wins > p2_wins) return p1;
        return p2;
    }
    const PlayerData& loser() const
    {
        if ( p1_wins < p2_wins) return p1;
        return p2;
    }
};

struct EloData
{
    int player_id;
    int rating_start;
    int rating;
    int set_wins;
    int set_losses;
    int match_wins;
    int match_losses;
    int attendances;

    static bool sortingHelper(const EloData& lhs, const EloData& rhs);
};

//struct StandingData
//{
//    StandingData()
//     : player(""), set_wins(0), set_losses(0),
//       game_wins(0), game_losses(0) {}
//    StandingData( const QString& player_,
//                    const QSet<QString>& won_against_,
//                    const quint32 set_wins_,
//                    const quint32 set_losses_,
//                    const quint32 game_wins_,
//                    const quint32 game_losses_)//,
//                    //const QSet<int> group_player_ids_)
//     : player(player_), won_against(won_against_),
//       set_wins(set_wins_), set_losses(set_losses_),
//       game_wins(game_wins_), game_losses(game_losses_)//,
//       //group_player_ids(group_player_ids_)
//    { }

//    QString player;
//    QSet<QString> won_against;
//    quint32 set_wins;
//    quint32 set_losses;
//    quint32 game_wins;
//    quint32 game_losses;
//    //QSet<int> group_player_ids;

//    static bool standingsSortingHelperRoundRobin(const StandingData& lhs, const StandingData& rhs );
//};

//void fillStandingsMap( const std::vector<MatchData>& match_data,
//                       QMap<QString,StandingData>& standings_map );



#endif // DATA_H

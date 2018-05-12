#include "data.h"
#include "database.h"

bool EloData::sortingHelper(const EloData& lhs, const EloData& rhs)
{
    return lhs.rating > rhs.rating;
}

//bool StandingData::standingsSortingHelperRoundRobin(
//    const StandingData& lhs, const StandingData& rhs )
//{
//    if      ( lhs.set_wins > rhs.set_wins ){ return true; }
//    else if ( lhs.set_wins < rhs.set_wins ){ return false; }

//    int lhs_games_diff = static_cast<int>(lhs.game_wins) - static_cast<int>(lhs.game_losses);
//    int rhs_games_diff = static_cast<int>(rhs.game_wins) - static_cast<int>(rhs.game_losses);
//    if      ( lhs_games_diff > rhs_games_diff ){ return true; }
//    else if ( lhs_games_diff < rhs_games_diff ){ return false; }

//    if ( lhs.won_against.contains(rhs.player) )
//    { return true; }
//    //else
//    return false;
//}

//void fillStandingsMap( const std::vector<MatchData>& match_data,
//                       QMap<QString,StandingData>& standings_map )
//{
//    for ( size_t i = 0; i < match_data.size(); i++ )
//    {
//        const MatchData& match_date = match_data[i];
//        standings_map[match_date.p1.name].player = match_date.p1.name;
//        standings_map[match_date.p2.name].player = match_date.p2.name;
//        standings_map[match_date.p1.name].game_wins += match_date.p1_wins;
//        standings_map[match_date.p2.name].game_wins += match_date.p2_wins;
//        standings_map[match_date.p1.name].game_losses += match_date.p2_wins;
//        standings_map[match_date.p2.name].game_losses += match_date.p1_wins;
//        //standings_map[match_date.p2].group_player_ids += match_date.group_player_ids;
//        if ( match_date.p1_wins > match_date.p2_wins )
//        { standings_map[match_date.p1.name].won_against.insert(match_date.p2.name); }
//        if ( match_date.p1_wins > match_date.p2_wins )
//        {
//            standings_map[match_date.p1.name].set_wins++;
//            standings_map[match_date.p2.name].set_losses++;
//        }
//        else
//        {
//            standings_map[match_date.p1.name].set_losses++;
//            standings_map[match_date.p2.name].set_wins++;
//        }
//    }
//}

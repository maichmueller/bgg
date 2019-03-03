//
// Created by Michael on 22/02/2019.
//

#ifndef STRATEGO_CPP_GAMEDECLARATIONS_H
#define STRATEGO_CPP_GAMEDECLARATIONS_H

#include "array"
#include "vector"
#include "map"


namespace GameTypes {
    // should be of length 2 only however!
    using pos_type = std::vector<int>;
}

using namespace std;
using namespace GameTypes;

struct key_comp {
    bool operator() (const pos_type& lhs, const pos_type& rhs) const
    {
        if(lhs[0] < rhs[0]) {
            return true;
        }
        else if(lhs[0] == rhs[0]) {
            if(lhs[1] < rhs[1]) {
                return true;
            }
            else {
                // on equivalence == or greater > case it has to return false
                return false;
            }
        }
        else {
            return false;
        }
    }
};

struct ActionRep {

    static const vector<int> available_types_s;
    static const vector<int> available_types_m;
    static const vector<int> available_types_l;

    static const vector<vector<int>> action_ar_s;
    static const vector<vector<int>> action_ar_m;
    static const vector<vector<int>> action_ar_l;

    static const map<vector<int>, vector<int>> piece_act_map_s;
    static const map<vector<int>, vector<int>> piece_act_map_m;
    static const map<vector<int>, vector<int>> piece_act_map_l;

    static int fill_act_vector(
            vector<vector<int>>& action_ar,
            vector<int> const & available_types,
            map<vector<int>, vector<int>>& piece_act_map
    )
    {
        int curr_idx = 0;
        int curr_type = -1;
        int curr_version = -1;
        for(auto& type : available_types) {
            if(0 < type && type < 11) {
                if(curr_type != type) {
                    curr_type = type;
                    curr_version = 0;
                }
                else
                    curr_version += 1;

                int temp_idx = curr_idx;

                int stop_len;
                if(type != 2)
                    stop_len = 2;
                else
                    stop_len = 5;

                for(int i = 1; i < stop_len; ++i) {
                    action_ar[curr_idx] = {0, i};
                    action_ar[curr_idx + 1] = {i, 0};
                    action_ar[curr_idx + 2] = {-i, 0};
                    action_ar[curr_idx + 3] = {0, -i};
                    curr_idx += 4;
                }

                vector<int> this_asign(static_cast<unsigned long> (curr_idx - temp_idx));
                for(int i = 0; i < this_asign.size(); ++i)
                    this_asign[i] = temp_idx + i;
                piece_act_map[{curr_type, curr_version}] = this_asign;
            }
        }
        return 0;
    }

    static auto const & get_available_types(int game_len) {
        if(game_len == 5) return available_types_s;
        else if(game_len == 7) return available_types_m;
        else if(game_len == 10) return available_types_l;
        else throw invalid_argument("Game len not in [5, 7, 10].");
    }

};




#endif //STRATEGO_CPP_GAMEDECLARATIONS_H
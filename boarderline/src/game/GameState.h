//
// Created by Michael on 21/02/2019.
//

#pragma once

#include "../board/Board.h"
#include "../board/Piece.h"
#include "../board/Move.h"
#include "GameUtilsStratego.h"
#include "../logic/StrategoLogic.h"
#include "unordered_map"
#include "torch/torch.h"


template <template <class, class> class Board, typename Piece, typename Position>
class GameState {

public:
    using board_type = Board<Piece, Position>;
    using move_type = Move<Position>;
    using cond_type = std::tuple<int, int, int, bool>;

protected:
    board_type board;
    std::array<std::map<int, int>, 2> dead_pieces;

    int terminal;
    bool terminal_checked;

    int move_count;

    using ii_key = std::tuple<int, int>;
    using ii_hasher = hash_tuple::hash<ii_key >;
    using ii_eq_comp = eqcomp_tuple::eqcomp<ii_key >;
    using ii_ptr_map = std::unordered_map<ii_key, std::shared_ptr<Piece>, ii_hasher, ii_eq_comp>;
    std::array<ii_ptr_map, 2> actors{ii_ptr_map{}, ii_ptr_map{}};

    std::vector<cond_type> conditions_torch_rep;
    bool conditions_set = false;

    std::vector<move_type> move_history;
    std::vector<std::array<std::shared_ptr<Piece>, 2>> piece_history;
    std::vector<bool> move_equals_prev_move;
    unsigned int rounds_without_fight;

    bool canonical_teams;

    void assign_actors(const board_type & board);

public:
    explicit GameState(int game_len);
    explicit GameState(board_type board, int move_count=0);
    GameState(board_type board, std::array<std::map<int, int>, 2>& dead_pieces, int move_count);
    GameState(int len, const std::map<Position, int>& setup_0, const std::map<Position, int>& setup_1);
    void check_terminal(bool flag_only=false, int turn=0);
    int do_move(Move<Position>& move);
    int fight(Piece& attacker, Piece& defender);
    int is_terminal(bool force_check=false, int turn=0);

    torch::Tensor torch_represent(int player);
    move_type action_to_move(int action, int player) const;

    void restore_to_round(int round);
    void undo_last_rounds(int n=1);

    int get_canonical_team(Piece& piece);
    Position get_canonical_pos(Piece& piece);
    int get_move_count() {return move_count;}
    bool is_canonical() {return canonical_teams;}

    void canonical_board(int player);
    void set_board(board_type brd) {this->board = std::move(brd);}
    board_type const * get_board() const {return &board;}
};

template <template <class, class> class Board, typename Piece, typename Position>
GameState<Board, Piece, Position>::GameState(int game_len)
        : board(game_len), terminal(404), terminal_checked(true),
          move_count(0), canonical_teams(true), rounds_without_fight(0),
          move_equals_prev_move(0), move_history(0)
{
    std::map<int, int> team0_dead;
    std::map<int, int> team1_dead;
    dead_pieces = {team0_dead, team1_dead};
    assign_actors(this->board);
}

template <template <class, class> class Board, typename Piece, typename Position>
GameState<Board, Piece, Position>::GameState(board_type board, int move_count)
        : board(std::move(board)), move_count(move_count), terminal(404), terminal_checked(false),
          canonical_teams(true), rounds_without_fight(0), move_equals_prev_move(0),
          move_history(0)
{
    std::map<int, int> team_0_dead;
    std::map<int, int> team_1_dead;
    dead_pieces = std::array<std::map<int, int>, 2> {team_0_dead, team_1_dead};
    int len = board.get_shape();
    std::vector<int> avail_types;
    // copy the available types
    avail_types = GameDeclarations::get_available_types(len);
    for(auto type: avail_types) {
        team_0_dead[type] += 1;
        team_1_dead[type] += 1;
    }
    for(const auto& piece : board) {
        if( !(piece.second->is_null()) ) {
            if(piece.second->get_team() == 0)
                team_0_dead[piece.second->get_type()] -= 1;
            else
                team_1_dead[piece.second->get_type()] -= 1;
        }
    }
    assign_actors(this->board);
}

template <template <class, class> class Board, typename Piece, typename Position>
GameState<Board, Piece, Position>::GameState(board_type board, std::array<std::map<int, int>, 2>& dead_pieces, int move_count)
        : board(std::move(board)), dead_pieces(dead_pieces), move_count(move_count),
          terminal_checked(false), terminal(404), canonical_teams(true), rounds_without_fight(0),
          move_equals_prev_move(0), move_history(0)
{
    assign_actors(this->board);
}

template <template <class, class> class Board, typename Piece, typename Position>
GameState<Board, Piece, Position>::GameState(int len, const std::map<Position, int>& setup_0, const std::map<Position, int>& setup_1)
        : board(len, setup_0, setup_1), dead_pieces(), move_count(0),
          terminal_checked(false), terminal(404), canonical_teams(true), rounds_without_fight(0),
          move_equals_prev_move(0), move_history(0)
{
    std::map<int, int> team0_dead;
    std::map<int, int> team1_dead;
    dead_pieces = {team0_dead, team1_dead};
    assign_actors(board);
}

template <template <class, class> class Board, typename Piece, typename Position>
void GameState<Board, Piece, Position>::assign_actors(const board_type &board) {
    for(const auto& entry: board) {
        const auto& piece = entry.second;
        if(!piece->is_null() && piece->get_type() != 99)
            actors[piece->get_team()][std::make_tuple(piece->get_type(), piece->get_version())] = piece;
    }
}

template <template <class, class> class Board, typename Piece, typename Position>
void GameState<Board, Piece, Position>::check_terminal(bool flag_only, int turn) {
    if(dead_pieces[0][0] == 1) {
        terminal = -1;
        return;
    }
    else if(dead_pieces[1][0] == 1) {
        terminal = 1;
        return;
    }

    if(!flag_only) {
        if (!StrategoLogic::has_poss_moves(board, turn)) {
            terminal = ((turn == 1) ? 1 : -1) * 2;
            return;
        } else if (!StrategoLogic::has_poss_moves(board, (turn + 1) % 2)) {
            terminal = ((turn == 0) ? 1 : -1) * 2;
            return;
        }
    }
    // committing draw rules here
    // Rule 1: If the moves of both players have been repeated 3 times.
    if(move_history.size() >= 6) {
        auto move_end = move_equals_prev_move.end();
        // check if the moves of the one player are equal for the past 3 rounds
        bool all_equal_0 = *(move_end - 2) && *(move_end - 4) && *(move_end - 6);
        // now check it for the other player
        bool all_equal_1 = *(move_end - 1) && *(move_end - 3)&& *(move_end - 5);
        if(all_equal_0 && all_equal_1)
            // players simply repeated their last 3 moves -> draw
            terminal = 0;
    }
    // Rule 2: If no fight has happened for 50 rounds in a row.
    if(rounds_without_fight > 49) {
        terminal = 0;
    }
    terminal_checked = true;
}

template <template <class, class> class Board, typename Piece, typename Position>
int GameState<Board, Piece, Position>::is_terminal(bool force_check, int turn) {
    if(!terminal_checked || force_check)
        check_terminal(false, turn);
    return terminal;
}

template <template <class, class> class Board, typename Piece, typename Position>
void GameState<Board, Piece, Position>::canonical_board(int player) {
    // if the 0 player is team 1, then canonical is false,
    // if it is 0 otherwise, then the teams are canonical
    canonical_teams = bool(1 - player);
}

template <template <class, class> class Board, typename Piece, typename Position>
int GameState<Board, Piece, Position>::get_canonical_team(Piece& piece){
    if(canonical_teams) {
        return piece.get_team();
    }
    else {
        return 1 - piece.get_team();
    }
}

template <template <class, class> class Board, typename Piece, typename Position>
Position GameState<Board, Piece, Position>::get_canonical_pos(Piece& piece){
    if(canonical_teams) {
        return piece.get_position();
    }
    else {
        int len = board.get_shape();
        Position pos = piece.get_position();
        pos[0] = len-1-pos[0];
        pos[1] = len-1-pos[1];
        return pos;
    }
}

template <template <class, class> class Board, typename Piece, typename Position>
int GameState<Board, Piece, Position>::fight(Piece &attacker, Piece &defender) {
    return StrategoLogic::fight_outcome(attacker.get_type(), defender.get_type());
}

template <template <class, class> class Board, typename Piece, typename Position>
int GameState<Board, Piece, Position>::do_move(move_type &move) {
    // preliminaries
    Position from = move[0];
    Position to = move[1];
    int fight_outcome = 404;

    // save the access to the pieces in question
    // (removes redundant searching in board later)
    std::shared_ptr<Piece> piece_from = board[from];
    std::shared_ptr<Piece> piece_to = board[to];
    piece_from->set_flag_has_moved();

    // save all info to the history
    if(move_equals_prev_move.empty())
        move_equals_prev_move.push_back(false);
    else {
        auto& last_move = move_history.back();
        move_equals_prev_move.push_back((move[0] == last_move[0]) && (move[1] == last_move[1]));
    }
    move_history.push_back(move);
    // copying the pieces here, bc this way they can be fully restored later on
    // (especially when flags have been altered - needed e.g. in undoing last rounds)
    piece_history.push_back({std::make_shared<Piece>(*piece_from), std::make_shared<Piece>(*piece_to)});

    // enact the move
    if(!piece_to->is_null()) {
        // unhide participant pieces
        piece_from->set_flag_unhidden();
        piece_to->set_flag_unhidden();

        rounds_without_fight = 0;

        // engage in fight, since piece_to is not a null piece
        fight_outcome = fight(*piece_from, *piece_to);
        if(fight_outcome == 1) {
            // 1 means attacker won, defender died
            board.update_board(to, piece_from);
            auto null_piece = std::make_shared<Piece> (from);
            board.update_board(from, null_piece);

            dead_pieces[piece_to->get_team()][piece_to->get_type()] += 1;
        }
        else if(fight_outcome == 0) {
            // 0 means stalemate, both die
            auto null_piece_from = std::make_shared<Piece> (from);
            board.update_board(from, null_piece_from);
            auto null_piece_to = std::make_shared<Piece> (to);
            board.update_board(to, null_piece_to);

            dead_pieces[piece_from->get_team()][piece_from->get_type()] += 1;
            dead_pieces[piece_to->get_team()][piece_to->get_type()] += 1;
        }
        else {
            // -1 means defender won, attacker died
            auto null_piece = std::make_shared<Piece> (from);
            board.update_board(from, null_piece);

            dead_pieces[piece_from->get_team()][piece_from->get_type()] += 1;
        }
    }
    else {
        // no fight happened, simply move piece_from onto new position
        auto null_piece = std::make_shared<Piece> (from);
        board.update_board(from, null_piece);
        board.update_board(to, piece_from);

        rounds_without_fight += 1;
    }
    move_count += 1;
    terminal_checked = false;
    return fight_outcome;
}

template <template <class, class> class Board, typename Piece, typename Position>
void GameState<Board, Piece, Position>::undo_last_rounds(int n) {
    for(int i = 0; i < n; ++i) {
        move_type move = move_history.back();
        auto move_pieces = piece_history.back();

        move_history.pop_back();
        piece_history.pop_back();
        move_equals_prev_move.pop_back();

        Position from = move[0];
        Position to = move[1];
        board.update_board(from, move_pieces[0]);
        board.update_board(to, move_pieces[1]);
        for(auto& piece: move_pieces) {
            int type = piece->get_type();
            int version = piece->get_version();
            if(0 < type && type < 11 && version != -1)
                actors[piece->get_team()][{piece->get_type(), piece->get_version()}] = piece;
        }
    }
    move_count -= n;
}

template <template <class, class> class Board, typename Piece, typename Position>
void GameState<Board, Piece, Position>::restore_to_round(int round) {
    undo_last_rounds(move_count - round);
}

template <template <class, class> class Board, typename Piece, typename Position>
torch::Tensor GameState<Board, Piece, Position>::torch_represent(int player) {
    if(!conditions_set) {
        auto type_counter = utils::counter(GameDeclarations::get_available_types(board.get_shape()));
        conditions_torch_rep = StateRepresentation::create_conditions(type_counter, 0);
        conditions_set = true;
    }

    return StateRepresentation::b2s_cond_check(
            board,
            conditions_torch_rep,
            player);
}

template <template <class, class> class Board, typename Piece, typename Position>
typename GameState<Board, Piece, Position>::move_type
GameState<Board, Piece, Position>::action_to_move(int action, int player) const {
    int board_len = board.get_shape();
    int action_dim = ActionRep::get_act_rep(board_len).size();
    return ActionRep::action_to_move(action, action_dim, board_len, actors.at(player), player);

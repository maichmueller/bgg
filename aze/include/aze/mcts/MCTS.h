#pragma once

#include <memory>
#include <random>
#include <string>
#include <unordered_map>

#include "aze/game/State.h"
#include "aze/logic/Logic.h"
#include "aze/nn/model/NeuralNet.h"
#include "aze/nn/representation/Representer.h"
#include "aze/utils/utils.h"
#include "aze/types.h"

/**
 * The Monte-Carlo Tree Search class provides a tree search mechanism to
 * iteratively evaluate states using the evaluation function of a neural
 * network.
 *
 * By iteratively training the neural network with these evaluated turns, the
 * overall evaluation function of the game can be fully trained. This class
 * doesn't put a restriction on the types of games that can be evaluated. Any
 * state type with a board, a string representation, and certain utilty methods
 * can be provided.
 */
class MCTS {
   /// Data container for:
   /// - visit count
   /// - terminal value
   /// - policy vector
   /// - valid actions
   /// per: (state s)
   struct StateData {
      size_t count = 0;
      int terminal_value;
      std::vector< double > policy;
      std::vector< unsigned int > validity_mask;

      bool operator==(const StateData &other)
      {
         return count == other.count && terminal_value == other.terminal_value
                && policy == other.policy && validity_mask == other.validity_mask;
      }

      void operator+=(size_t c) { count += c; }
   };

   /// Data container for:
   /// - visit count
   /// - Q-values
   /// - policy vector
   /// - valid actions
   /// per: (state s, action a)
   struct StateActionData {
      size_t count = 0;
      double qvalue;

      bool operator==(const StateActionData &other)
      {
         return count == other.count && qvalue == other.qvalue;
      }

      void operator+=(size_t c) { count += c; }
   };

   /// The neural network is provided via a shared_ptr on construction.
   /// It is assumed the network is allocated and deallocated outside.
   sptr< NetworkWrapper > m_nnet_sptr;
   /// a parameter determining the amount of exploration (needs to be checked in
   /// paper)
   double m_cpuct;
   /// the number of monte carlo tree searches to do for state evaluation
   int m_num_mcts_sims;
   /// the current depth of the tree search. Meant as a temporary counter of the
   /// search
   size_t search_depth = 0;
   /// numeric stabilizer (minimum value to add to a function undef at 0)
   constexpr static const double m_EPS = 1e-10;
   /// the map holding the state-action-specific information.
   std::unordered_map< std::string, StateData > m_NTPVs;
   /// the map holding the state-action-specific information.
   std::unordered_map< std::tuple< std::string, int >, StateActionData > m_NQsa;

   /**
    * Samples dirichlet noise of a provided distribution dimension.
    * @param size the dimension of the dirichlet noise.
    * @return vector of `size` many dirichlet samples.
    */
   static std::vector< double > _sample_dirichlet(size_t size);

   /**
    * Method that performs one step of the tree search.
    *
    * In every step the evaluated turn will be stored and the chosen best action
    * applied to the state. If the state is not terminal, then the search will
    * call itself anew for the next player's state to evaluate.
    *
    * @tparam StateType type that holds and updates the game state.
    * @tparam ActionRepresenterType this type needs to be compatible the
    * provided state type in order to translate the state into a torch tensor.
    * @param state the current game state.
    * @param player the player whose turn is being evaluated.
    * @param action_repper the reference to the action representer translating
    * the state to tensor.
    * @param root a flag to decide whether this search step is the initial one.
    * @return the value of the initially passed state and active player at root
    * level.
    */
   template < typename StateType, typename ActionRepresenterType >
   double _search(
      StateType &state,
      int player,
      RepresenterBase< StateType, ActionRepresenterType > &action_repper,
      bool root = false);

   /**
    * Private method that evaluates a current state tensor through the neural
    * network for active player.
    *
    * @tparam StateType type that holds and updates the game state.
    * @tparam ActionRepresenterType this type needs to be compatible the
    * provided state type in order to translate the state into a torch tensor.
    * @param state the current game state.
    * @param player the player whose turn is being evaluated.
    * @param action_repper the reference to the action representer translating
    * the state to tensor.
    * @return
    */
   template < typename StateType, typename ActionRepresenterType >
   std::tuple< std::vector< double >, std::vector< unsigned int >, double > _evaluate_new_state(
      StateType &state,
      int player,
      RepresenterBase< StateType, ActionRepresenterType > &action_repper);

  public:
   /**
    * Standard constructor for this class. At least the neural network pointer
    * needs to be provided.
    *
    * @param nnet_sptr the neural network for evaluation of the states.
    * @param num_mcts_sims the number of search runs that should be made for the
    * tree search.
    * @param cpuct a parameter influencing the strength of exploration.
    */
   MCTS(sptr< NetworkWrapper > nnet_sptr, int num_mcts_sims = 100, double cpuct = 4);

   template < typename StateType, typename ActionRepresenterType >
   std::vector< double > get_policy_vec(
      sptr< StateType > &state,
      int player,
      RepresenterBase< StateType, ActionRepresenterType > &action_repper,
      double expl_rate = 1.);
};

template < typename StateType, typename ActionRepresenterType >
std::vector< double > MCTS::get_policy_vec(
   sptr< StateType > &state,
   int player,
   RepresenterBase< StateType, ActionRepresenterType > &action_repper,
   double expl_rate)
{
   //   tqdm bar;
   for(int i = 0; i < m_num_mcts_sims; ++i) {
      //      bar.progress(i, m_num_mcts_sims);
      search_depth = -1;
      _search(*state, player, action_repper, /*root=*/true);
   }
   //   bar.finish();

   std::string state_rep = state->string_representation(player, true);

   std::vector< int > counts(action_repper.get_actions().size());

   // we need to find the best action
   double sum_counts = 0;
   double highest_count = 0;
   int best_act = 0;
   for(size_t a = 0; a < counts.size(); ++a) {
      if(auto count_entry = m_NQsa.find(std::make_tuple(state_rep, a));
         count_entry == m_NQsa.end()) {
         // not found
         counts[a] = 0;
      } else {
         // found
         // we are keeping track of the best action (by counts) for the case of
         // exploration_rate == 0
         size_t count = count_entry->second.count;
         counts[a] = count;
         sum_counts += count;
         if(count > highest_count) {
            best_act = a;
            highest_count = count;
         }
      }
   }
   if(sum_counts == 0) {
      // when all counts are 0 then there must have been something going wrong
      // in the tree search!
      throw std::logic_error(
         "All action probability values after search are 0. This means "
         "either the game is already over (calling this method from a "
         "terminal state is a logic error) or something has gone wrong "
         "in the tree search/model output (e.g. NaNs output).");
   }

   std::vector< double > probabilities(counts.size(), 0.0);
   if(expl_rate == 0) {
      probabilities[best_act] = 1;
      return probabilities;
   }

   // rescale the probabilities by the exploration rate. This will basically
   // even out the probabilities further to not let a single action dominate
   // the probabilities too much for further exploration later on.
   sum_counts = 0;
   for(auto &count : counts) {
      auto val = std::pow(count, 1 / expl_rate);
      count = val;
      sum_counts += val;
   }

   // now we need to normalize the probabilities again to sum to 1
   for(size_t i = 0; i < counts.size(); ++i) {
      probabilities[i] = counts[i] / sum_counts;
   }

   return probabilities;
}

template < typename StateType, typename ActionRepresenterType >
double MCTS::_search(
   StateType &state,
   int player,
   RepresenterBase< StateType, ActionRepresenterType > &action_repper,
   bool root)
{
   search_depth += 1;
   // for the state rep we flip the board if player == 1 and we dont if
   // player == 0! all the hidden enemy pieces wont be printed out
   // -> unknown pieces are also hidden for the neural net
   std::string s = state.string_representation(player, true);
   auto state_data_iter = m_NTPVs.find(s);
   if(state_data_iter == m_NTPVs.end()) {
      auto [Ps_filtered, action_mask, v] = std::move(
         _evaluate_new_state(state, player, action_repper));
      m_NTPVs.emplace(s, StateData{0, state.is_terminal(), Ps_filtered, action_mask});
      return -v;
   } else if(int terminal_value = state_data_iter->second.terminal_value; terminal_value != 404) {
      // note that we cant reuse the originally computed
      // terminality value for this state as the game may
      // have reached this position again by repeating the
      // same steps over and over again, which should
      // culminate in a draw.
      return -terminal_value;
   }

   StateData &state_data = state_data_iter->second;
   // the mask never changes -> const ref
   const std::vector< unsigned int > &validity_mask = state_data.validity_mask;
   // the policy may be adapted though -> copy
   std::vector< double > Ps = state_data.policy;

   if(root) {
      std::vector< double > dirichlet = _sample_dirichlet(Ps.size());
      double new_sum_val = 0;
      for(size_t i = 0; i < Ps.size(); ++i) {
         // Check the alphazero paper for adding dirichlet
         // noise to the priors.
         double pi = (0.75 * Ps[i] + 0.25 * dirichlet[i]) * validity_mask[i];
         Ps[i] = pi;
         new_sum_val += pi;
      }
      // Normalize
      for(auto &p : Ps) {
         p /= new_sum_val;
      }
   }

   // setting up the initial values for action selection
   double best_utility = -std::numeric_limits< double >::infinity();
   int best_action = -1;

   size_t nr_actions = Ps.size();

   std::vector< decltype(m_NQsa)::iterator > state_action_iters(nr_actions);
   const auto state_action_iter_end = m_NQsa.end();
   for(size_t a = 0; a < nr_actions; ++a) {
      if(validity_mask[a]) {
         double utility_val;
         const auto state_action = std::make_tuple(s, a);
         const auto state_action_iter = m_NQsa.find(state_action);
         state_action_iters[a] = state_action_iter;
         if(state_action_iter != state_action_iter_end) {
            auto &qn = state_action_iter->second;
            utility_val = qn.qvalue
                          + m_cpuct * Ps[a] * sqrt(state_data.count)
                               / static_cast< double >((1 + qn.count));
         } else {
            utility_val = m_cpuct * Ps[a] * sqrt(state_data.count + m_EPS);
         }
         if(utility_val > best_utility) {
            best_utility = utility_val;
            best_action = a;
         }
      }
   }

   auto move = action_repper.action_to_move(state, best_action, player);
   state.do_move(move);

   double v = _search(
      state,
      (player + 1) % 2,
      action_repper,
      /*root=*/false);

   state.undo_last_rounds();

   // now update the count and q-values for each tuple of (state, action) or
   // emplace an unseen such tuple in the Data map.
   // This loop will update the utility function U(s,a).
   if(state_action_iters[best_action] != state_action_iter_end) {
      auto &container = state_action_iters[best_action]->second;
      size_t &n_sa = container.count;
      double &q_sa = container.qvalue;

      q_sa = (n_sa * q_sa + v) / static_cast< double >(n_sa + 1);
      n_sa += 1;
   } else {
      m_NQsa.emplace(std::make_tuple(s, best_action), StateActionData{1, v});
   }

   state_data += 1;
   return -v;
}

template < typename StateType, typename ActionRepresenterType >
std::tuple< std::vector< double >, std::vector< unsigned int >, double > MCTS::_evaluate_new_state(
   StateType &state, int player, RepresenterBase< StateType, ActionRepresenterType > &action_repper)
{
   auto board = state.get_board();
   // convert State to torch tensor representation
   const torch::Tensor state_tensor = action_repper.state_representation(state, player);

   auto [log_Ps, v] = m_nnet_sptr->evaluate(state_tensor);
   // move to cpu (as we need to work with it) and flatten the tensor.
   // First dim is batch size (== 1 here)
   log_Ps = log_Ps.cpu().view(-1);

   const auto action_mask = action_repper.get_action_mask(*board, player);
   std::vector< double > Ps_filtered(action_mask.size());

   // mask invalid actions
   float Ps_sum = 0;
   auto log_Ps_acc = log_Ps.template accessor< float, 1 >();
   for(size_t i = 0; i < action_mask.size(); ++i) {
      float masked_action = log_Ps_acc[i] * action_mask[i];
      Ps_sum += masked_action;
      Ps_filtered[i] = masked_action;
   }

   // normalize the likelihoods
   for(auto &p_val : Ps_filtered) {
      p_val /= Ps_sum;
   }
   return std::make_tuple(Ps_filtered, action_mask, v);
}
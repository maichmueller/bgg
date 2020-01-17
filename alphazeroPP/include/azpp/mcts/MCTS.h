#pragma once

#include <azpp/logic/Logic.h>

#include <memory>
#include <string>
#include <unordered_map>

#include "azpp/game/State.h"
#include "azpp/nn/model/NeuralNet.h"
#include "azpp/nn/representation/Representer.h"
#include "azpp/utils/utils.h"

class MCTS {
   // Container for visit count, terminal value, policy
   // vector, valid actions per state s
   struct StateInfo {
      size_t count = 0;
      int terminal_value;
      std::vector< double > policy;
      std::vector< unsigned int > validity_mask;

      StateInfo(
         size_t count,
         int terminal_value,
         const std::vector< double > &policy,
         const std::vector< unsigned int > &validity_mask)
          : count(count),
            terminal_value(terminal_value),
            policy(policy),
            validity_mask(validity_mask)
      {
      }

      bool operator==(const StateInfo &other)
      {
         return count == other.count && terminal_value == other.terminal_value
                && policy == other.policy
                && validity_mask == other.validity_mask;
      }

      void operator+=(size_t c) { count += c; }
   };

   // Container for Q-values and visit count per state s and
   // action a
   struct StateActionInfo {
      size_t count = 0;
      double qvalue;

      StateActionInfo(size_t count, double qvalue)
          : count(count), qvalue(qvalue)
      {
      }

      bool operator==(const StateActionInfo &other)
      {
         return count == other.count && qvalue == other.qvalue;
      }

      void operator+=(size_t c) { count += c; }
   };

   std::shared_ptr< NetworkWrapper > m_nnet_sptr;
   double m_cpuct;
   int m_num_mcts_sims;

   size_t search_depth = 0;
   constexpr static const double m_EPS = 1e-10;

   std::unordered_map< std::string, StateInfo > m_NTPVs;
   std::unordered_map< std::tuple< std::string, int >, StateActionInfo > m_NQsa;

   static std::vector< double > _sample_dirichlet(size_t size);

   template < typename StateType, typename ActionRepresenterType >
   double _search(
      StateType &state,
      int player,
      RepresenterBase< StateType, ActionRepresenterType > &action_repper,
      bool root = false);

   template < typename StateType, typename ActionRepresenterType >
   std::tuple< std::vector< double >, std::vector< unsigned int >, double >
   _evaluate_new_state(
      StateType &state,
      int player,
      RepresenterBase< StateType, ActionRepresenterType > &action_repper);

  public:
   MCTS(
      std::shared_ptr< NetworkWrapper > nnet_sptr,
      int num_mcts_sims,
      double cpuct = 4);

   template < typename StateType, typename ActionRepresenterType >
   std::vector< double > get_action_probabilities(
      StateType &state,
      int player,
      RepresenterBase< StateType, ActionRepresenterType > &action_repper,
      double expl_rate = 1.);
};

template < typename StateType, typename ActionRepresenterType >
std::vector< double > MCTS::get_action_probabilities(
   StateType &state,
   int player,
   RepresenterBase< StateType, ActionRepresenterType > &action_repper,
   double expl_rate)
{
//   tqdm bar;
   for(int i = 0; i < m_num_mcts_sims; ++i) {
//              bar.progress(i, m_num_mcts_sims);
      LOGD2("Number of elements in NTPV", m_NTPVs.size())
      LOGD2("Number of elements in NQsa", m_NQsa.size())
      search_depth = -1;
      _search(state, player, action_repper, /*root=*/true);
   }
//       bar.finish();

   std::string state_rep = state.get_board()->print_board(
      static_cast< bool >(player), true);

   std::vector< int > counts(action_repper.get_actions().size());

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
      return std::vector< double >(0);
   }

   std::vector< double > probabilities(counts.size(), 0.0);
   if(expl_rate == 0) {
      probabilities[best_act] = 1;
      return probabilities;
   }
   sum_counts = 0;
   for(auto &count : counts) {
      auto val = std::pow(count, 1 / expl_rate);
      count = val;
      sum_counts += val;
   }

   for(size_t i = 0; i < counts.size(); ++i) {
      probabilities[i] = counts[i] / sum_counts;
   }
   return probabilities;
}

template < typename StateType, typename ActionRepresenterType >
std::tuple< std::vector< double >, std::vector< unsigned int >, double >
MCTS::_evaluate_new_state(
   StateType &state,
   int player,
   RepresenterBase< StateType, ActionRepresenterType > &action_repper)
{
   auto board = state.get_board();
   // convert State to torch tensor representation
   const torch::Tensor state_tensor = action_repper.state_representation(
      state, player);

   auto [Ps, v] = m_nnet_sptr->predict(state_tensor);

   Ps = Ps.view(-1);  // flatten the tensor as the first dim
                      // is the batch size dim

   // mask for invalid actions
   const auto action_mask = action_repper.get_action_mask(*board, player);

   torch::TensorAccessor Ps_acc = Ps.template accessor< float, 1 >();
   std::vector< double > Ps_filtered(action_mask.size());
   float Ps_sum = 0;

   // mask invalid actions
   for(size_t i = 0; i < action_mask.size(); ++i) {
      auto temp = Ps_acc[i] * action_mask[i];
      Ps_sum += temp;
      Ps_filtered[i] = temp;
   }
   // normalize the likelihoods
   for(auto &p_val : Ps_filtered) {
      p_val /= Ps_sum;
   }
   return std::make_tuple(Ps_filtered, action_mask, v);
}

template < typename StateType, typename ActionRepresenterType >
double MCTS::_search(
   StateType &state,
   int player,
   RepresenterBase< StateType, ActionRepresenterType > &action_repper,
   bool root)
{
   search_depth += 1;
   //    LOGD2("Search depth", search_depth);
   //    LOGD(state.get_board()->print_board(false, false));
   // for the state rep we flip the board if player == 1 and
   // we dont if player
   // == 0! all the enemy hidden pieces wont be printed out
   // -> unknown pieces are also hidden for the neural net
   std::string s = state.string_representation(player, true);
   auto state_data_iter = m_NTPVs.find(s);
   if(state_data_iter == m_NTPVs.end()) {
      auto [Ps_filtered, action_mask, v] = std::move(
         _evaluate_new_state(state, player, action_repper));
      m_NTPVs.emplace(
         s, StateInfo{0, state.is_terminal(), Ps_filtered, action_mask});
      return -v;
   } else if(int terminal_value = state_data_iter->second.terminal_value;
             terminal_value != 404) {
      // note that we cant reuse the originally computed
      // terminality value for this state as the game may
      // have reached this position again by repeating the
      // same steps over and over again, which should
      // culminate in a draw.
      return -terminal_value;
   }

   StateInfo &ntpv = state_data_iter->second;
   // the mask never changes -> const ref
   const std::vector< unsigned int > &validity_mask = ntpv.validity_mask;
   // the policy may be adapted though -> copy
   std::vector< double > Ps = ntpv.policy;

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
   double curr_best = -std::numeric_limits< double >::infinity();
   int best_action = -1;

   std::vector< decltype(m_NQsa)::iterator > sa_iterators;
   size_t nr_actions = Ps.size();
   sa_iterators.reserve(nr_actions);
   const auto sa_end_iter = m_NQsa.end();
   for(size_t a = 0; a < nr_actions; ++a) {
      if(validity_mask[a]) {
         double u;
         const auto sa = std::make_tuple(s, a);
         const auto sa_iter = m_NQsa.find(sa);
         sa_iterators[a] = sa_iter;
         if(sa_iter != sa_end_iter) {
            auto &qn = sa_iter->second;
            u = qn.qvalue
                + m_cpuct * Ps[a] * sqrt(ntpv.count)
                     / static_cast< double >((1 + qn.count));
         } else {
            u = m_cpuct * Ps[a] * sqrt(ntpv.count + m_EPS);
         }
         if(u > curr_best) {
            curr_best = u;
            best_action = a;
         }
      }
   }

   int a = best_action;

   auto move = action_repper.action_to_move(state, a, player);
   state.do_move(move);

   double v = _search(
      state,
      (player + 1) % 2,
      action_repper,
      /*root=*/false);
   state.undo_last_rounds();

   if(auto &container = sa_iterators[a]->second;
      sa_iterators[a] != sa_end_iter) {
      size_t &n_sa = container.count;
      double &q_sa = container.qvalue;

      q_sa = (n_sa * q_sa + v) / static_cast< double >(n_sa + 1);
      n_sa += 1;
   } else {
      m_NQsa.emplace(std::make_tuple(s, a), StateActionInfo{1, v});
   }

   ntpv += 1;
   return -v;
}

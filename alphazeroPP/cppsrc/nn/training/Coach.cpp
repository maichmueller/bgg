//
// Created by michael on 05.06.19.
//

#include "Coach.h"

#include <random>

#include "azpp/agent/AgentReinforce.h"
#include "azpp/game/Arena.h"
#include "azpp/nn/alphazero/MCTS.h"
#include "azpp/utils/torch_utils.h"
#include "filesystem"
#include "torch/torch.h"

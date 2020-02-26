//
// Created by michael on 05.06.19.
//

#include "Coach.h"

#include <random>

#include "aze/agent/AgentReinforce.h"
#include "aze/game/Arena.h"
#include "aze/nn/alphazero/MCTS.h"
#include "aze/utils/torch_utils.h"
#include "filesystem"
#include "torch/torch.h"

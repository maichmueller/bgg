//
// Created by michael on 17.04.19.
//

#include "torch_utils.h"

const torch::DeviceType torch_utils::GLOBAL_DEVICE::global_dev = torch::cuda::is_available() ? torch::kCPU : torch::kCUDA;
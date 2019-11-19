//
// Created by Michael on 04.11.19.
//

#include "azpp/nn/model/modules/FullyConnected.h"

#include "azpp/utils/torch_utils.h"



FullyConnected::FullyConnected(int D_in, int D_out, int nr_lin_layers, int start_expo,
                               const torch::nn::Functional & activation_function)
        : m_D_in(D_in),
          m_D_out(D_out),
          m_start_exponent(start_expo),
          m_nr_lin_layers(nr_lin_layers),
          m_layers()
{

    if(nr_lin_layers < 1) {
        throw std::invalid_argument("Less than 1 linear layer requested. Aborting.");
    }
    else if(nr_lin_layers == 1) {
        m_layers->push_back(torch::nn::Linear(torch::nn::LinearOptions(D_in, D_out)));
    }
    else {

        // note that 2 << N-1 == 2^N (exponentiating, not xor operator)
        int hidden_nodes = 2 << (start_expo - 1);

        m_layers->push_back(torch::nn::Linear(torch::nn::LinearOptions(D_in, hidden_nodes)));
        // build a trickle down layer stack with halved number of nodes in each iteration
        // eg. layer1: Linear(128, 64)
        //     layer2: Linear( 64, 32)
        // ...
        for(int i = 0; i < nr_lin_layers-2; ++i) {
            int denom1;
            if(i==0)
                denom1 = 1;
            else
                denom1 = 2 << (i -1);
            int denom2 = 2 << (i);

            auto options = torch::nn::LinearOptions(hidden_nodes / denom1, hidden_nodes / denom2);
            m_layers->push_back(torch::nn::Linear(options));
            m_layers->push_back(activation_function );
        }
        m_layers->push_back(torch::nn::Linear(
                torch::nn::LinearOptions(hidden_nodes / (2 << (nr_lin_layers - 3)), D_out)) );
    }
}

torch::Tensor FullyConnected::forward(const torch::Tensor &input) {

    input.to(GLOBAL_DEVICE::get_device());

    return m_layers->forward(input);
}

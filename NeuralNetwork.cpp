//
// Created by michael on 13.04.19.
//

#include "NeuralNetwork.h"

#include "filesystem"

Convolutional::Convolutional(int channels_in, std::vector<int>&& filter_sizes, const std::vector<int>&& kernel_sizes_vec,
                             const std::vector<bool>&& maxpool_used_vec, const std::vector<float>&& dropout_probs,
                             const torch::nn::Functional & activation_function)
: m_channels_in(channels_in), m_layers(), m_filter_sizes(filter_sizes),
  m_kernel_sizes(kernel_sizes_vec), m_maxpool_used_vec(maxpool_used_vec),
  m_droput_prop_per_layer(dropout_probs)
{
    /// Constructor for a pre-defined Convultional Neural Network.
    /// Options allow for including maxpooling and dropout layers
    /// in the specified layer index if
    /// "maxpool_used_vec[index] = true" or "dropout_prob[index] > 0"

    int nr_conv_layers = m_kernel_sizes.size();

    std::vector<int> zero_paddings(nr_conv_layers, 0);
    for(int i = 0; i < nr_conv_layers; ++i) {
        zero_paddings[i] = (m_kernel_sizes[i] - 1) / 2;
    }

    filter_sizes.insert(filter_sizes.begin(), channels_in);

    for(int k = 0; k < nr_conv_layers; ++k) {
        // extend the layer sequential by a convolutional
        auto options = torch::nn::Conv2dOptions(filter_sizes[k], filter_sizes[k+1], m_kernel_sizes[k])
                .stride(1).padding(zero_paddings[k]).with_bias(false);
        m_layers->push_back(torch::nn::Conv2d(options));
        // check for maxpool and/or dropout layer additions
        if(maxpool_used_vec[k]) {
            m_layers->push_back( torch::nn::Functional(torch::max_pool2d,
                    /*kernel_size=*/3, /*stride=*/2, /*padding=*/0, /*dilation=*/1, false) );
        }
        auto p_drop = m_droput_prop_per_layer[k];
        if(p_drop > 0) {
            m_layers->push_back( torch::nn::Functional(torch::dropout, /*p=*/p_drop, true) );
        }
        // finish this layer-segment with a layer activation
        m_layers->push_back( activation_function );
    }
}

template <typename prim_type>
Convolutional::Convolutional(int channels_in,
        const std::vector<int> & filter_sizes,
        const std::vector<int> & kernel_sizes_vec,
        const std::vector<bool> & maxpool_used_vec,
        prim_type dropout_prob_for_all,
        const torch::nn::Functional & activation_function)
        : Convolutional(channels_in, filter_sizes,
                        kernel_sizes_vec, maxpool_used_vec,
                        std::vector<float> (4, static_cast<float> (dropout_prob_for_all)),
                        activation_function)
                        {}

torch::Tensor Convolutional::forward(const torch::Tensor &input) {

    input.to(torch_utils::GLOBAL_DEVICE::get_device());
    std::cout << "\nCONVOLUTIONAL INPUT\n";
    std::cout << input;

    return m_layers->forward(input);
}

FullyConnected::FullyConnected(int D_in, int D_out, int nr_lin_layers, int start_expo,
                               const torch::nn::Functional & activation_function)
:   D_in(D_in), D_out(D_out), nr_lin_layers(nr_lin_layers), start_exponent(start_expo), layers()
{

    if(nr_lin_layers < 1) {
        throw std::invalid_argument("Less than 1 linear layer requested. Aborting.");
    }
    else if(nr_lin_layers == 1) {
        layers->push_back( torch::nn::Linear(torch::nn::LinearOptions(D_in, D_out)));
    }
    else {

    // note that 2 << N-1 == 2^N (exponentiating, not xor operator)
    int hidden_nodes = 2 << (start_expo - 1);

    layers->push_back( torch::nn::Linear(torch::nn::LinearOptions(D_in, hidden_nodes)));
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
        layers->push_back( torch::nn::Linear(options));
        layers->push_back( activation_function );
    }
    layers->push_back( torch::nn::Linear(
            torch::nn::LinearOptions(hidden_nodes / (2 << (nr_lin_layers - 3)), D_out)) );
    }
}

torch::Tensor FullyConnected::forward(const torch::Tensor &input) {

    input.to(torch_utils::GLOBAL_DEVICE::get_device());

    return layers->forward(input);
}


StrategoAlphaZero::StrategoAlphaZero(int D_in, int D_out,
                                     int nr_lin_layers, int start_exponent,
                                     int channels,
                                     std::vector<int> filter_sizes,
                                     std::vector<int> kernel_sizes_vec,
                                     std::vector<bool> maxpool_used_vec,
                                     std::vector<float> dropout_probs,
                                     const torch::nn::Functional & activation_function)
                                     : D_in(D_in), convo_layers(channels, std::move(filter_sizes),
                                       std::move(kernel_sizes_vec), std::move(maxpool_used_vec),
                                       std::move(dropout_probs), activation_function),
                                       pi_act_layer(nullptr), v_act_layer(nullptr)
{
    if(nr_lin_layers < 2) {
        throw std::invalid_argument("Less than 2 linear m_layers requested. Aborting.");
    }

    int hidden_nodes = 2 << (start_exponent - 1);
    int substitute_d_out = hidden_nodes / (2 << (nr_lin_layers - 4));
    linear_layers = FullyConnected(
            D_in,
            /*D_out=*/substitute_d_out,
            nr_lin_layers - 1, start_exponent, activation_function);

    pi_act_layer = torch::nn::Linear(
            torch::nn::LinearOptions(substitute_d_out, D_out));
    v_act_layer = torch::nn::Linear(
            torch::nn::LinearOptions(substitute_d_out, 1));

}

std::tuple<torch::Tensor, torch::Tensor> StrategoAlphaZero::forward(const torch::Tensor &input) {
    input.to(torch_utils::GLOBAL_DEVICE::get_device());
    std::cout << "\nALPHAZERO INPUT\n";
    std::cout << input;
    torch::Tensor output = convo_layers.forward(input).view({-1, D_in});
    output = linear_layers.forward(output);

    torch::Tensor pi = torch::log_softmax(pi_act_layer->forward(output), /*dim=*/1);
    torch::Tensor v = torch::tanh(v_act_layer->forward(output));

    return std::make_tuple(pi, v);
}



std::vector<int> NetworkWrapper::_sample_without_replacement(int pool_len, int sample_len) {
    /// We have to first build a vector of all the possible numbers,
    /// since we want to sample WITHOUT replacement. This can only
    /// be achieved by knowing all the numbers at draw time, i.e.
    /// all the numbers allocated.

    // fill the pool with ints from 0 to pool_len
    std::vector<int> pool(pool_len);
    for(int i = 0; i < pool_len; ++i) {
        pool[i] = i;
    }

    std::random_device rd;
    std::mt19937 g(rd());
    // shuffle all the values in the pool uniformly
    std::shuffle(pool.begin(), pool.end(), g);

    // draw sample_len many samples
    std::vector<int> sample(sample_len);
    for(int i = 0; i < sample_len; ++i) {
        sample[i] = pool[i];
    }
    return sample;
}

void NetworkWrapper::to_device(torch::Device dev) {
    nnet->to(dev);
}


std::tuple<torch::Tensor, double> NetworkWrapper::predict(const torch::Tensor & board_tensor) {
    nnet->eval();

    // We dont want gradient updates here, so we need the NoGradGuard
    std::cout << board_tensor;
    torch::NoGradGuard no_grad;
    std::cout << board_tensor;
    auto [pi_tensor, v_tensor] = nnet->forward(board_tensor);

//    // copy the pi tensor data into a std::vector
//    std::vector<double> pi_vec(pi_tensor.size(1));
//    torch::TensorAccessor pi_acc = pi_tensor.accessor<float, 1>();
//    for(int i = 0; i < pi_tensor.size(1); ++i) {
//        pi_vec[i] = pi_acc[i];
//    }

    return std::make_tuple(pi_tensor, v_tensor.template item<float>());
}

void NetworkWrapper::save_checkpoint(std::string const & folder, std::string const & filename) {
    namespace fs = std::filesystem;
    fs::path dir(folder);
    fs::path file (filename);
    fs::path full_path = dir / file;
    // if directory doesnt exist, create it
    if(!fs::exists(dir)) {
        std::cout << "Checkpoint directory doesn't exist yet. Creating it." << std::endl;
        fs::create_directory(dir);
    }

    torch::save(nnet, full_path);
}


void NetworkWrapper::load_checkpoint(std::string const &folder, std::string const &filename) {
    namespace fs = std::filesystem;
    fs::path dir(folder);
    fs::path file (filename);
    fs::path full_path = dir / file;
    // if file doesnt exist, raise an error
    if(!fs::exists(full_path)) {
        std::cout << "Checkpoint directory doesn't exists yet. Creating it." << std::endl;
        throw std::invalid_argument("No file found for filename " + filename + ".");
    }

    torch::load(nnet, full_path);
}
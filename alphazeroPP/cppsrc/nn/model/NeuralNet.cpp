#include "azpp/nn/model/NeuralNet.h"


std::tuple<torch::Tensor, double>
NetworkWrapper::predict(const torch::Tensor &board_tensor) {
    m_network->eval();

    // We dont want gradient updates here, so we need the NoGradGuard
    torch::NoGradGuard no_grad;
    auto[pi_tensor, v_tensor] = m_network->forward(board_tensor);

//    // copy the pi tensor data into a std::vector
//    std::vector<double> pi_vec(pi_tensor.size(1));
//    torch::TensorAccessor pi_acc = pi_tensor.accessor<float, 1>();
//    for(int i = 0; i < pi_tensor.size(1); ++i) {
//        pi_vec[i] = pi_acc[i];
//    }
    return std::make_tuple(pi_tensor, v_tensor.template item<double>());
}


void NetworkWrapper::save_checkpoint(std::string const &folder, std::string const &filename) {
    namespace fs = std::filesystem;
    fs::path dir(folder);
    fs::path file(filename);
    fs::path full_path = dir / file;
    // if directory doesn't exist, create it
    if (!fs::exists(dir)) {
        std::cout << "Checkpoint directory doesn't exist yet. Creating it." << std::endl;
        fs::create_directory(dir);
    }
    torch::save(m_network, full_path.string());
}


void NetworkWrapper::load_checkpoint(std::string const &folder, std::string const &filename) {
    namespace fs = std::filesystem;
    fs::path dir(folder);
    fs::path file(filename);
    fs::path full_path = dir / file;
    // if file doesnt exist, raise an error
    if (!fs::exists(full_path)) {
        std::cout << "Checkpoint directory doesn't exists yet. Creating it." << std::endl;
        throw std::invalid_argument("No file found for filename " + filename + ".");
    }
    torch::load(m_network, full_path.string());
}


std::vector<long long> NetworkWrapper::prepend_to_shape(
        const torch::Tensor &tensor,
        size_t value) const {
    // get current shape and initialize +1
    auto sizes = tensor.sizes();
    std::vector<long long> sizes_out(static_cast<long long>(sizes.size()) + 1);
    // size 0 is batch size, rest of the shape needs to be kept from input
    sizes_out[0] = value;
    std::copy(sizes.begin(), sizes.end(), sizes_out.begin() + 1);

    return sizes_out;
}


#include "azpp/nn/model/NeuralNet.h"
#include "azpp/utils/logging_macros.h"


std::tuple< torch::Tensor, double > NetworkWrapper::evaluate(
   const torch::Tensor &board_tensor)
{
   // We dont want gradient updates here, so we need the NoGradGuard
   torch::NoGradGuard no_grad;
   m_network->eval();

   auto [pi_tensor, v_tensor] = m_network->forward(board_tensor.to(GLOBAL_DEVICE::get_device()));

   return std::make_tuple(pi_tensor.detach(), v_tensor.template item< double >());
}

void NetworkWrapper::save_checkpoint(
   std::string const &folder, std::string const &filename)
{
   namespace fs = std::filesystem;
   fs::path dir(folder);
   fs::path file(filename);
   fs::path full_path = dir / file;
   // if directory doesn't exist, create it
   if(! fs::exists(dir)) {
      std::cout << "Checkpoint directory doesn't exist yet. Creating it."
                << std::endl;
      fs::create_directory(dir);
   }
   torch::save(m_network, full_path.string());
}

void NetworkWrapper::load_checkpoint(
   std::string const &folder, std::string const &filename)
{
   namespace fs = std::filesystem;
   fs::path dir(folder);
   fs::path file(filename);
   fs::path full_path = dir / file;
   // if file doesnt exist, raise an error
   if(! fs::exists(full_path)) {
      std::cout << "Checkpoint directory doesn't exists yet. Creating it."
                << std::endl;
      throw std::invalid_argument(
         "No file found for filename " + filename + ".");
   }
   torch::load(m_network, full_path.string());
}

std::vector< TORCH_ARRAYREF_TYPE > NetworkWrapper::_prepend_to_shape(
   const torch::Tensor &tensor, size_t value) const
{
   // get current shape and initialize +1
   auto sizes = tensor.sizes();
   std::vector< TORCH_ARRAYREF_TYPE > sizes_out(
      static_cast< TORCH_ARRAYREF_TYPE >(sizes.size()) + 1);
   // size 0 is batch size, rest of the shape needs to be kept from input
   sizes_out[0] = value;
   std::copy(sizes.begin(), sizes.end(), sizes_out.begin() + 1);

   return sizes_out;
}

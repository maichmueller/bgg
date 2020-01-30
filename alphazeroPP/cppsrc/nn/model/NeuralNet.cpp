
#include "azpp/nn/model/NeuralNet.h"

#include "azpp/utils/logging_macros.h"

std::tuple< torch::Tensor, double > NetworkWrapper::evaluate(
   const torch::Tensor &board_tensor)
{
   // We dont want gradient updates here, so we need the NoGradGuard
   torch::NoGradGuard no_grad;
   m_network->eval();

   auto [pi_tensor, v_tensor] = m_network->forward(
      board_tensor.to(GLOBAL_DEVICE::get_device()));

   return std::make_tuple(
      pi_tensor.detach(), v_tensor.template item< double >());
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
   // get current shape and initialize another dimension +1
   std::vector< TORCH_ARRAYREF_TYPE > sizes_out;
   auto sizes_tensor = tensor.sizes();
   auto begin_it = sizes_tensor.begin();
   if(sizes_tensor[0] == 1) {
      // size 0 is batch size if it is == 0, rest of the shape needs to be kept
      // from input
      sizes_out.reserve(sizes_tensor.size());
      begin_it += 1;
   } else {
      sizes_out.reserve(sizes_tensor.size() + 1);
   }
   sizes_out.push_back(value);
   std::copy(begin_it, sizes_tensor.end(), std::back_inserter(sizes_out));

   return sizes_out;
}

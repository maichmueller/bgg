//
// Created by michael on 13.04.19.
//

#include "NeuralNetwork.h"


std::vector<int> NeuralNetwork::_sample_without_replacement(int pool_len, int sample_len) {
    // fill the pool with ints from 0 to pool_len
    std::vector<int> pool(pool_len);
    for(int i = 0; i < pool_len; ++i) {
        pool[i] = i;
    }

    std::random_device rd;
    std::mt19937 g(rd());
    // shuffle all the values in the pool uniformly
    std::shuffle(pool.begin(), pool.end(), g);

    std::vector<int> sample(sample_len);
    for(int i = 0; i < sample_len; ++i) {
        sample[i] = pool[i];
    }
    return sample;
}


template<typename Board>
void NeuralNetwork::train(std::vector<std::tuple<torch::Tensor, std::vector<double>, int, int>> train_examples, int epochs,
                          int batch_size) {
    to_device();

    auto optimizer = torch::optim::Adam(nnet->parameters(), /*lr=*/0.01);

    tqdm bar;
    for(int epoch = 0; epoch < epochs; ++epoch) {
        bar.progress(epoch, epochs);
        // set the nnet into train mode (i.e. demands gradient updates for tensors)
        nnet->train();

        for(int b = 0; b < static_cast<int> (train_examples.size() / batch_size); ++b) {

            // get a randomly drawn sample index batch
            auto sample_ids = _sample_without_replacement(train_examples.size(), batch_size);

            std::vector<torch::Tensor> board_batch(batch_size);
            std::vector<std::vector<double>> pi_batch(batch_size);
            std::vector<int> v_batch(batch_size);

            // fill the batch vectors with the respective part of the sample-tuple
            for(const auto& i : sample_ids) {
                auto& sample = train_examples[i];
                board_batch.push_back(std::get<0>(sample));
                pi_batch.push_back(std::get<1>(sample));
                v_batch.push_back(std::get<2>(sample));
            }

//            torch::Tensor pi_tensor = torch::from_blob(pi_batch.data());
        }
    }
    bar.finish();

}
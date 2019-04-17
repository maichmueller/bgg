//
// Created by michael on 17.04.19.
//

#ifndef STRATEGO_CPP_TORCH_UTILS_H
#define STRATEGO_CPP_TORCH_UTILS_H


#include "torch/torch.h"


namespace torch_utils {

    // if CUDA is available, we are using it. Otherwise CPU
    auto GLOBAL_DEVICE = torch::cuda::is_available() ? torch::kCPU : torch::kCUDA;

    template <typename T>
    struct nesting_dimension {
        static const int value = 0;
    };

    template <template<class, class> class TT, class T1, class T2>
    struct nesting_dimension<TT<T1,T2>> {
        static const int value = 1 + nesting_dimension<T1>::value;
    };

    template <typename DType>
    torch::Tensor& tensor_from_vector(const std::vector<DType>& vector, torch::Tensor& tensor_to_fill) {
        int dimensions = nesting_dimension<std::vector<DType>>::value;
        std::vector<int> shape(dimensions);

    }

};


#endif //STRATEGO_CPP_TORCH_UTILS_H

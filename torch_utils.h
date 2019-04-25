//
// Created by michael on 17.04.19.
//

#ifndef STRATEGO_CPP_TORCH_UTILS_H
#define STRATEGO_CPP_TORCH_UTILS_H


#include "torch/torch.h"


namespace torch_utils {

    // if CUDA is available, we are using it. Otherwise CPU
    static auto GLOBAL_DEVICE = torch::cuda::is_available() ? torch::kCPU : torch::kCUDA;

    auto& get_global_device() {
        return GLOBAL_DEVICE;
    }

    template <typename Type>
    struct NestedVectorManip {
        static const int dimension = 0;

        template <typename size_type>
        static void sizes(const Type&, std::vector<size_type>&) {}

        template <typename size_type, size_t dim>
        static void fill_tensor(const std::vector<Type>& vec,
                                torch::TensorAccessor<size_type, dim>& accessor) {
            for( int i = 0; i < vec.size(); ++i) {
                accessor[i] = vec[i];
            }
        }
    };

    template <template <class, class> class Vec, typename Type, typename Alloc>
    struct NestedVectorManip<Vec<Type, Alloc>> {
        static const int dimension = 1 + NestedVectorManip<Type>::dimension;

        template <typename size_type>
        static void sizes(const Vec<Type,Alloc>& v, std::vector<size_type>& size)
        {
            size.push_back(v.size());
            NestedVectorManip<Type>::sizes(v[0],size);
        }

        template <typename size_type, size_t dim>
        static void fill_tensor(const std::vector<Type, Alloc>& vec,
                                torch::TensorAccessor<size_type, dim>& accessor) {
            for( int i = 0; i < vec.size(); ++i) {
                fill_tensor(vec[i], accessor[i]);
            }
        }
    };


    template <typename DType>
    torch::Tensor& fill_tensor_from_vector_(const std::vector<DType>& vec, torch::Tensor& tensor_to_fill) {
        // get the number of nested vector<vector<...<vector<TYPE>...>>
        int dimensions = NestedVectorManip<std::vector<DType>>::value;
        // init a shape vector catching the sizes of each nested vector
        std::vector<int64_t > shape(dimensions);
        NestedVectorManip<std::vector<DType>>::sizes(vec, shape);
        // pass shape vector to ArrayRef (which is holding only the pointer to the vector)
        // for torch API
        torch::ArrayRef<int64_t> tensor_shape(shape.data(), shape.size());

        tensor_to_fill.resize_(tensor_shape);
        auto tensor_access = tensor_to_fill.accessor<int64_t, NestedVectorManip<std::vector<DType>>::value>();

        NestedVectorManip<std::vector<DType>>::fill_tensor(vec, tensor_access);
    }

};


#endif //STRATEGO_CPP_TORCH_UTILS_H

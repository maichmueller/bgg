//
// Created by michael on 17.04.19.
//

#ifndef STRATEGO_CPP_TORCH_UTILS_H
#define STRATEGO_CPP_TORCH_UTILS_H


#include "torch/torch.h"


namespace torch_utils {

    struct GLOBAL_DEVICE {
        // if CUDA is available, we are using it. Otherwise CPU
        static const torch::DeviceType global_dev;

        static auto& get_device() {
            return global_dev;
        }
    };




    template <typename Val_Type>
    struct NestedVectorManip {
        static const int dimension = 0;

        template <typename size_type>
        static void sizes(const Val_Type&, std::vector<size_type>&) {}

        template <typename size_type, size_t dim>
        static void fill_tensor(const std::vector<Val_Type>& vec,
                                torch::TensorAccessor<size_type, dim>& accessor) {
            for( int i = 0; i < vec.size(); ++i) {
                accessor[i] = vec[i];
            }
        }
    };

    template <template <class, class> class Vec, typename Val_Type, typename Alloc>
    struct NestedVectorManip<Vec<Val_Type, Alloc>> {
        static const int dimension = 1 + NestedVectorManip<Val_Type>::dimension;

        template <typename size_type>
        static void sizes(const Vec<Val_Type,Alloc>& v, std::vector<size_type>& size)
        {
            size.push_back(v.size());
            NestedVectorManip<Val_Type>::sizes(v[0],size);
        }

        template <typename size_type, size_t dim>
        static void fill_tensor(const std::vector<Val_Type, Alloc>& vec,
                                torch::TensorAccessor<size_type, dim>& accessor) {

            for( int i = 0; i < vec.size(); ++i) {
                NestedVectorManip<Val_Type> fill_tensor(vec[i], accessor[i]);
            }
        }
    };


    template <typename DType>
    torch::Tensor& fill_tensor_from_vector_(const std::vector<DType>& vec, torch::Tensor& tensor_to_fill) {
        // get the number of nested vector<vector<...<vector<TYPE>...>>
        int dimensions = NestedVectorManip<std::vector<DType>>::dimension;
        // init a shape vector catching the sizes of each nested vector
        std::vector<int64_t > shape(dimensions);
        NestedVectorManip<std::vector<DType>>::sizes(vec, shape);
        // pass shape vector to ArrayRef (which is holding only the pointer to the vector)
        // for torch API
        torch::ArrayRef<int64_t> tensor_shape(shape.data(), shape.size());

        tensor_to_fill.resize_(tensor_shape);
        auto tensor_access = tensor_to_fill.accessor<int64_t, NestedVectorManip<std::vector<DType>>::dimension>();

        NestedVectorManip<std::vector<DType>>::fill_tensor(vec, tensor_access);
    }

};


#endif //STRATEGO_CPP_TORCH_UTILS_H

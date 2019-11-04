//
// Created by michael on 17.04.19.
//

#ifndef STRATEGO_CPP_TORCH_UTILS_H
#define STRATEGO_CPP_TORCH_UTILS_H


#include "torch/torch.h"
#include "type_traits"

#include "utils.h"

struct GLOBAL_DEVICE {
    // if CUDA is available, we are using it. Otherwise CPU
    //static inline torch::Device global_dev = torch::cuda::is_available() ? torch::kCUDA : torch::kCPU;

    static inline torch::Device global_dev = torch::kCPU;

    static auto& get_device() {
        return global_dev;
    }
    static void set_device_cuda() {
        global_dev = torch::Device(torch::kCUDA);
    }
    static void set_device_cpu() {
        global_dev = torch::Device(torch::kCPU);
    }
};

namespace torch_utils {

    template <typename Val_Type>
    struct NestedVectorManip {
        static const int dimension = 0;

        template <typename size_type>
        static void sizes(const Val_Type&, std::vector<size_type>&) {}

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
    };

    // specialized fill tensor function for the one m_dim case
    template <template <class, class> class Container, typename Val_Type, typename Alloc>
    static void fill_tensor(const Container<Val_Type, Alloc>& vec,
                            torch::Tensor& tensor_to_fill) {

        for( int i = 0; i < vec.size(); ++i) {
            tensor_to_fill[i] = vec[i];
        }
    }

    template <template <class, class> class OuterContainer, template <class, class> class InnerContainer, typename OuterAlloc,
                                        typename InnerDType, typename InnerAlloc>
    static void fill_tensor(const OuterContainer<InnerContainer<InnerDType, InnerAlloc>, OuterAlloc>& vec,
                            torch::Tensor& tensor_to_fill) {

        for( int i = 0; i < vec.size(); ++i) {
            InnerContainer<InnerDType, InnerAlloc> next_level_vec = vec[i];

            // assert that vec is indeed a std::vector
            static_assert(std::is_same< decltype(next_level_vec), std::vector<InnerDType, InnerAlloc>>::value,
                          "Inner vector is not a vector");

            auto next_level_tensor = tensor_to_fill[i];
            fill_tensor(next_level_vec, next_level_tensor);

        }
    }

    template <typename DType>
    torch::Tensor tensor_from_vector(const std::vector<DType>& vec) {
        /// Copies the vector (even nested vectors in vectors) data 
        /// into a tensor and returns this tensor by copy.

//        // get the number of nested vector<vector<...<vector<TYPE>...>>
//        int dimensions = NestedVectorManip<std::vector<DType>>::dimension;
        // init a m_shape vector catching the sizes of each nested vector
        std::vector<int64_t > shape;
        NestedVectorManip<std::vector<DType>>::sizes(vec, shape);
        // pass m_shape vector to ArrayRef (which is holding only the pointer to the vector)
        // for torch API
        torch::ArrayRef<int64_t> tensor_shape(shape);
        auto build_details = torch::TensorOptions()
                .dtype(torch::kInt64)
                .device(GLOBAL_DEVICE::get_device());
        torch::Tensor tensor_to_fill = torch::zeros(tensor_shape, build_details);
//        auto tensor_access = tensor_to_fill.accessor<int64_t, NestedVectorManip<std::vector<DType>>::dimension>();

        fill_tensor(vec, tensor_to_fill);

        return tensor_to_fill;
    }

    template <typename DType>
    torch::Tensor tensor_from_vector(const std::vector<DType>& vec, torch::TensorOptions& options) {
        /// Copies the vector (even nested vectors in vectors) data
        /// into a tensor and returns this tensor by copy.

//        // get the number of nested vector<vector<...<vector<TYPE>...>>
//        int dimensions = NestedVectorManip<std::vector<DType>>::dimension;
        // init a m_shape vector catching the sizes of each nested vector
        std::vector<int64_t > shape;
        NestedVectorManip<std::vector<DType>>::sizes(vec, shape);
        // pass m_shape vector to ArrayRef (which is holding only the pointer to the vector)
        // for torch API
        torch::ArrayRef<int64_t> tensor_shape(shape);
        options = options.device(GLOBAL_DEVICE::get_device());
        torch::Tensor tensor_to_fill = torch::zeros(tensor_shape, options);
//        auto tensor_access = tensor_to_fill.accessor<int64_t, NestedVectorManip<std::vector<DType>>::dimension>();

        fill_tensor(vec, tensor_to_fill);

        return tensor_to_fill;
    }

};


#endif //STRATEGO_CPP_TORCH_UTILS_H

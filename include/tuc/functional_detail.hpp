#pragma once

// To be included only via tuc/functional.hpp

namespace tuc
{    
    namespace detail {
        template <typename NotVector> void reserve(NotVector&, size_t) {
            // By default, do nothing (not std::vector)
        }
        template <typename VectorElement> void reserve(std::vector<VectorElement>& vector, size_t new_capacity) {
            vector.reserve(new_capacity);
        }
    }
}
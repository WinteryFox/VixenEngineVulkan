#pragma once

#include <memory>
#include <iostream>

namespace Vixen {
    template<typename T>
    class PushConstant {
        std::unique_ptr<T> data;

    public:
        explicit PushConstant(const T &initialData) : data(std::make_unique<T>(initialData)) {}

        PushConstant(const PushConstant &) = delete;

        PushConstant(PushConstant &&other) noexcept : data(std::exchange(other.data, nullptr)) {}

        void update(const T &newData) {
            *data = newData;
        }

        const T *getData() const {
            return data.get();
        }
    };
}

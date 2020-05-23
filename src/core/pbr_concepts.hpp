#pragma once

#define __cpp_lib_concepts
#include <concepts>

namespace pbr {

template <typename T>
concept arithmetic = std::is_arithmetic_v<T>;

} // namespace pbr

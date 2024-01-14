
#ifndef ECS_HELPER_H
#define ECS_HELPER_H

#include <cstdint>
#include <array>
#include <type_traits>
#include <memory>

namespace ecs::helper
{

template <auto Value>
constexpr void print_error() { static_assert(false); }

template <typename Type>
constexpr void print_error() { static_assert(false); }

}

#endif  // ECS_HELPER_H

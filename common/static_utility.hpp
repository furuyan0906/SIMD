#ifndef  H__STATIC_UTILITY__H
#define  H__STATIC_UTILITY__H


#include  <cstddef>

template <typename T>
constexpr bool is_power_of_2(T value) noexcept
{
    return (value > 0) && (((value - 1U) & value) == 0);
}

template <std::size_t N, typename T>
constexpr bool is_multiple_of_N(T value)
{
    return (value > 0) && (value % N == 0);
}


#endif  // H__STATIC_UTILITY__H


#include  <iostream>
#include  <type_traits>
#include  <vector>
#include  <string>
#include  <utility>
#include  <chrono>
#include  <stdexcept>
#include  <cstdlib>
#include  <x86intrin.h>
#include  "static_utility.hpp"
#include  "time_profile.hpp"


template <std::size_t nMemory>
constexpr void static_validate()
{
    // TODO
}

int main(void)
{
    constexpr std::size_t nMemory = 1280 * 720;  // = 32 * 28800
    constexpr int alignedNBits = 32;  // = 256 / 8;
    constexpr int numSample = 100;

    auto profiles = std::vector<time_profile<uint64_t>>
    {
        // TODO
    };

    for (int i = 0; i < numSample; ++i)
    {
        // TODO
    }

    outputProfiles(profiles);

    return EXIT_SUCCESS;
}


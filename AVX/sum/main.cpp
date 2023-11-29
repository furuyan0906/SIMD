#include  <iostream>
#include  <type_traits>
#include  <memory>
#include  <vector>
#include  <string>
#include  <utility>
#include  <chrono>
#include  <iomanip>
#include  <stdexcept>
#include  <cstdlib>
#include  <x86intrin.h>
#include  "static_utility.hpp"
#include  "time_profile.hpp"


template <std::size_t nMemory>
constexpr void static_validate()
{
    static_assert(is_multiple_of_N<32>(nMemory), "N must be a multiplication of 32");
}

template <std::size_t nMemory>
constexpr void make_answer(std::unique_ptr<uint8_t[]>& ans)
{
    static_validate<nMemory>();

    constexpr std::size_t numData = nMemory / sizeof(uint8_t);
    for (std::size_t i = 0; i < numData; ++i)
    {
        uint8_t x = i % 64;  // 64 = 256 / 4
        ans[i] = static_cast<uint8_t>(x * 3);
    }
}

template <std::size_t nMemory>
void make_initial(std::unique_ptr<uint8_t[]>& a, std::unique_ptr<uint8_t[]>& b)
{
    static_validate<nMemory>();

    constexpr std::size_t numData = nMemory / sizeof(uint8_t);
    for (std::size_t i = 0; i < numData; ++i)
    {
        uint8_t x = i % 64;  // 64 = 256 / 4
        a[i] = x;
        b[i] = x * 2;
    }
}

template <std::size_t nMemory>
std::pair<time_point, time_point> sumby_scalar(
        const std::unique_ptr<uint8_t[]>& a,
        const std::unique_ptr<uint8_t[]>& b,
        std::unique_ptr<uint8_t[]>& c)
{
    static_validate<nMemory>();

    auto start = std::chrono::system_clock::now();

    constexpr std::size_t numData = nMemory / sizeof(uint8_t);
    for (std::size_t i = 0; i < numData; ++i)
    {
        c[i] = a[i] + b[i];
    }

    auto end = std::chrono::system_clock::now();

    return std::pair<time_point, time_point>(start, end);
}

template <std::size_t nMemory>
std::pair<time_point, time_point> sumby_avx2(
        const std::unique_ptr<uint8_t[]>& a,
        const std::unique_ptr<uint8_t[]>& b,
        std::unique_ptr<uint8_t[]>& c)
{
    static_validate<nMemory>();

    auto start = std::chrono::system_clock::now();

    constexpr std::size_t numData = nMemory / sizeof(uint8_t);
    for (std::size_t i = 0; i < numData; i += 32 /* = 256/8 */)
    {
        auto x = _mm256_loadu_si256((__m256i *)(&a[i]));
        auto y = _mm256_loadu_si256((__m256i *)(&b[i]));

        auto z = _mm256_add_epi8(x, y);
        _mm256_storeu_si256((__m256i *)&c[i], z);
    }

    auto end = std::chrono::system_clock::now();

    return std::pair<time_point, time_point>(start, end);
}

template <std::size_t nMemory>
std::pair<time_point, time_point> sumby_avx2_aligned(
        const std::unique_ptr<uint8_t[]>& a,
        const std::unique_ptr<uint8_t[]>& b,
        std::unique_ptr<uint8_t[]>& c)
{
    static_validate<nMemory>();

    auto start = std::chrono::system_clock::now();

    constexpr std::size_t numData = nMemory / sizeof(uint8_t);
    for (std::size_t i = 0; i < numData; i += 32 /* = 256/8 */)
    {
        auto x = _mm256_load_si256((__m256i *)(&a[i]));
        auto y = _mm256_load_si256((__m256i *)(&b[i]));

        auto z = _mm256_add_epi8(x, y);
        _mm256_store_si256((__m256i *)&c[i], z);
    }

    auto end = std::chrono::system_clock::now();

    return std::pair<time_point, time_point>(start, end);
}

template <std::size_t nMemory>
bool checkResult(
        const std::unique_ptr<uint8_t[]>& expected,
        const std::unique_ptr<uint8_t[]>& actual)
{
    static_validate<nMemory>();

    bool ans = true;

    constexpr std::size_t numData = nMemory / sizeof(uint8_t);
    for (std::size_t i = 0; i < numData; ++i)
    {
        ans &= (expected[i] == actual[i]);
    }

    return ans;
}

template <std::size_t nMemory>
void ThrowRuntimeErrorIfCheckResultNG(
        const std::unique_ptr<uint8_t[]>& expected,
        const std::unique_ptr<uint8_t[]>& actual,
        const std::string& target)
{
    static_validate<nMemory>();

    if (!checkResult<nMemory>(expected, actual))
    {
        std::stringstream ss;
        ss << "Check failed (" << target << ")";

        throw std::runtime_error(ss.str());
    }
}

int main(void)
{
    constexpr std::size_t nMemory = 1280 * 720;  // = 32 * 28800
    constexpr int alignedNBits = 32;  // = 256 / 8;
    constexpr int numSample = 100;

    auto profiles = std::vector<time_profile<uint64_t>>
    {
        time_profile<uint64_t>("sumby_scalar"),
        time_profile<uint64_t>("sumby_scalar(aligned)"),
        time_profile<uint64_t>("sumby_avx2"),
        time_profile<uint64_t>("sumby_avx2(aligned)"),
    };

    auto ans = std::unique_ptr<uint8_t[]>(new uint8_t[nMemory]);
    make_answer<nMemory>(ans);

    for (int i = 0; i < numSample; ++i)
    {
        // スカラー
        {
            auto a = std::unique_ptr<uint8_t[]>(new uint8_t[nMemory * sizeof(uint8_t)]);
            auto b = std::unique_ptr<uint8_t[]>(new uint8_t[nMemory * sizeof(uint8_t)]);
            auto c = std::unique_ptr<uint8_t[]>(new uint8_t[nMemory * sizeof(uint8_t)]);
            make_initial<nMemory>(a, b);

            auto tw = sumby_scalar<nMemory>(a, b, c);
            ThrowRuntimeErrorIfCheckResultNG<nMemory>(c, ans, profiles[0].target);

            profiles[0].update(tw);
        }

        // スカラー (メモリアライメント)
        {
            auto a = std::unique_ptr<uint8_t[]>(new (std::align_val_t{alignedNBits}) uint8_t[nMemory * sizeof(uint8_t)]);
            auto b = std::unique_ptr<uint8_t[]>(new (std::align_val_t{alignedNBits}) uint8_t[nMemory * sizeof(uint8_t)]);
            auto c = std::unique_ptr<uint8_t[]>(new (std::align_val_t{alignedNBits}) uint8_t[nMemory * sizeof(uint8_t)]);
            make_initial<nMemory>(a, b);

            auto tw = sumby_scalar<nMemory>(a, b, c);
            ThrowRuntimeErrorIfCheckResultNG<nMemory>(c, ans, profiles[1].target);

            profiles[1].update(tw);
        }

        // AVX2
        {
            auto a = std::unique_ptr<uint8_t[]>(new uint8_t[nMemory * sizeof(uint8_t)]);
            auto b = std::unique_ptr<uint8_t[]>(new uint8_t[nMemory * sizeof(uint8_t)]);
            auto c = std::unique_ptr<uint8_t[]>(new uint8_t[nMemory * sizeof(uint8_t)]);
            make_initial<nMemory>(a, b);

            auto tw = sumby_avx2<nMemory>(a, b, c);
            ThrowRuntimeErrorIfCheckResultNG<nMemory>(c, ans, profiles[2].target);

            profiles[2].update(tw);
        }

        // AVX2 (メモリアライメント)
        {
            auto a = std::unique_ptr<uint8_t[]>(new (std::align_val_t{alignedNBits}) uint8_t[nMemory * sizeof(uint8_t)]);
            auto b = std::unique_ptr<uint8_t[]>(new (std::align_val_t{alignedNBits}) uint8_t[nMemory * sizeof(uint8_t)]);
            auto c = std::unique_ptr<uint8_t[]>(new (std::align_val_t{alignedNBits}) uint8_t[nMemory * sizeof(uint8_t)]);
            make_initial<nMemory>(a, b);

            auto tw = sumby_avx2_aligned<nMemory>(a, b, c);
            ThrowRuntimeErrorIfCheckResultNG<nMemory>(c, ans, profiles[3].target);

            profiles[3].update(tw);
        }
    }

    outputProfiles(profiles);

    return EXIT_SUCCESS;
}


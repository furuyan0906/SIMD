#ifndef  H__TIME_PROFILE__H
#define  H__TIME_PROFILE__H


#include  <iostream>
#include  <vector>
#include  <string>
#include  <sstream>
#include  <tuple>
#include  <chrono>
#include  <chrono>

using time_point = std::chrono::system_clock::time_point;
using time_watch = std::pair<time_point, time_point>;

template <typename T = uint64_t>
class time_profile
{
    private:
        T N;
        T avg;
        T max;
        T min;

        inline T make_record(const time_watch& tw) const noexcept
        {
            auto start = tw.first;
            auto end = tw.second;        
            return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
        }

    public: 
        std::string target;

        time_profile(const std::string&& target) noexcept
            : N(0), avg(0), max(0), min(UINT64_MAX), target(target)
        {
        }

        void update(const time_watch& tw) noexcept
        {
            auto newRecord = this->make_record(tw);

            this->max = std::max(this->max, newRecord);
            this->min = std::min(this->min, newRecord);
            this->avg = (this->avg * this->N + newRecord) / (this->N + 1);
            this->N++;
        }

        std::string get_profile_string() const noexcept
        {
            std::stringstream ss;
            ss << "(N=" << this->N << ") avg=" << this->avg << "ns, max=" << this->max << "ns, min=" << this->min << "ns (" << this->target << ")";
            return ss.str();
        }
};

template <typename T = uint64_t>
void outputProfiles(const std::vector<time_profile<T>>& profiles) noexcept
{
    for (auto profile : profiles)
    {
        auto prfstr = profile.get_profile_string();
        std::cout << prfstr << std::endl;
    }
}


#endif  // H__TIME_PROFILE__H


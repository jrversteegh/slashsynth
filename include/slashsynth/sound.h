#ifndef SLASHSYNTH_SOUND_H_INCLUDED__
#define SLASHSYNTH_SOUND_H_INCLUDED__

#include <array>
#include <numbers>
#include <ranges>
#include <vector>

namespace slashsynth {

namespace ranges = std::ranges;
namespace views = std::views;
namespace numbers = std::numbers;
static constexpr float inv_sqrt2 = static_cast<float>(0.5 * numbers::sqrt2);

static constexpr size_t const channel_count = 6;

struct Sample {
  std::array<float, channel_count> channel_values;
  double time = 0;

  float operator[](size_t i) const {
    return channel_values[i];
  }

  Sample& operator*=(float value) {
    for (auto& channel_value : channel_values) {
      channel_value *= value;
    }
    return *this;
  }

  Sample& operator+=(Sample const& rhs) {
    if (rhs.time != time) {
      throw std::runtime_error("Sample time mismatch");
    }
    for (auto [i, values] :
         views::zip(channel_values, rhs.channel_values) | views::enumerate) {
      channel_values[i] =
          inv_sqrt2 * (std::get<0>(values) + std::get<1>(values));
    }
    return *this;
  }
};

using Samples = std::vector<Sample>;

Sample operator*(Sample const& lhs, float rhs) {
  Sample result(lhs);
  result *= rhs;
  return result;
}

Sample operator*(float lhs, Sample const& rhs) {
  return rhs * lhs;
}

Sample operator+(Sample const& lhs, Sample const& rhs) {
  Sample result(lhs);
  result += rhs;
  return result;
}

} // namespace slashsynth

#endif

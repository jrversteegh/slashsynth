#ifndef SLASHSYNTH_SOUND_H_INCLUDED__
#define SLASHSYNTH_SOUND_H_INCLUDED__

#include <array>
#include <vector>

namespace slashsynth {

static constexpr size_t const channel_count = 6;

struct Sample {
  std::array<float, channel_count> channel_values;

  Sample& operator*=(float value) {
    for (auto& channel_value : channel_values) {
      channel_value *= value;
    }
    return *this;
  }
};

using Samples = std::vector<Sample>;

Sample operator*(Sample const& sample, float value) {
  Sample result(sample);
  result *= value;
  return result;
}

} // namespace slashsynth

#endif

#ifndef SLASHSYNTH_CLOCK_H_INCLUDED__
#define SLASHSYNTH_CLOCK_H_INCLUDED__

#include <atomic>
#include <chrono>
#include <thread>

namespace slashsynth {

namespace chrono = std::chrono;

struct Clock {
  std::atomic<bool> running{false};
  static constexpr auto period = chrono::microseconds(10);

  void start() {
    auto next = chrono::high_resolution_clock::now();
    while (running) {
      next += period;
      std::this_thread::sleep_until(next);
    }
  }

private:
  double time_{0};
};

} // namespace slashsynth

#endif

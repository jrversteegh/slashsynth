#ifndef OUTPUT_H_INCLUDED__

#include <soundio/soundio.h>

#include <exception>

namespace slashsynth {

struct SoundIO {
  SoundIO(): io_{soundio_create()} {
    if (io_ == nullptr) {
      throw std::runtime_error("Failed to create SoundIO instance");
    }
  }
  SoundIO(SoundIO const&) = delete;
  SoundIO(SoundIO&&) = default;
  ~SoundIO() {
    soundio_destroy(io_);
  }
  SoundIO& operator=(SoundIO const&) = delete;
  SoundIO& operator=(SoundIO&&) = default;
private:
  SoundIo* io_;
};

}  // namespace slashsynth

#endif

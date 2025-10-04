#ifndef OUTPUT_H_INCLUDED__

#include <soundio/soundio.h>

namespace slashsynth {

struct SoundIO {
  SoundIO(): sndio_{soundio_create()} {
  }
  SoundIO(SoundIO const&) = delete;
  SoundIO(SoundIO&&) = default;
  ~SoundIO() {
    soundio_destroy(sndio_.get());
  }
  SoundIO& operator=(SoundIO const&) = delete;
  SoundIO& operator=(SoundIO&&) = default;
private:
  std::unique_ptr<SoundIo> sndio_;
};

}  // namespace ssynth

#endif

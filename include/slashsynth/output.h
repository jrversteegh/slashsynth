#ifndef OUTPUT_H_INCLUDED__

namespace sio {
#include <soundio/soundio.h>
}
#include <fmt/format.h>

#include <exception>
#include <string>

namespace slashsynth {

struct SoundIODevice {
  SoundIODevice() = delete;
  SoundIODevice(sio::SoundIoDevice* device) : device_{device} {
    if (device_ == nullptr) {
      throw std::runtime_error("SoundIo failed to create device");
    }
  }
  SoundIODevice(SoundIODevice const& device) = delete;
  SoundIODevice(SoundIODevice&& device) = default;
  SoundIODevice& operator=(SoundIODevice const& device) = delete;
  SoundIODevice& operator=(SoundIODevice&& device) = default;
  ~SoundIODevice() {
    sio::soundio_device_unref(device_);
  }

  auto get_name() {
    return std::string(device_->name);
  }

private:
  sio::SoundIoDevice* device_;
};

struct SoundIO {
  SoundIO() : io_{sio::soundio_create()} {
    if (io_ == nullptr) {
      throw std::runtime_error("Failed to create SoundIO instance");
    }
    if (auto error = soundio_connect(io_)) {
      sio::soundio_destroy(io_);
      throw std::runtime_error(fmt::format("SoundIO failed to connect: {}",
                                           sio::soundio_strerror(error)));
    }
    sio::soundio_flush_events(io_);
  }
  SoundIO(SoundIO const&) = delete;
  SoundIO(SoundIO&&) = default;
  ~SoundIO() {
    sio::soundio_destroy(io_);
  }
  SoundIO& operator=(SoundIO const&) = delete;
  SoundIO& operator=(SoundIO&&) = default;

  SoundIODevice get_default_device() {
    auto device_index = sio::soundio_default_output_device_index(io_);
    if (device_index < 0) {
      throw std::runtime_error("SoundIO doesn't have default device");
    }
    return SoundIODevice(sio::soundio_get_output_device(io_, device_index));
  }

private:
  sio::SoundIo* io_;
  friend SoundIODevice;
};

} // namespace slashsynth

#endif

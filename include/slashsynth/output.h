#ifndef OUTPUT_H_INCLUDED__

namespace sio {
#include <soundio/soundio.h>
}
#include <fmt/format.h>

#include <exception>
#include <string>
#include <type_traits>

namespace slashsynth {

struct SoundIO_input;
struct SoundIO_output;

void write_callback(sio::SoundIoOutStream* stream, int frame_count_min, int frame_count_max);

void register_input(sio::SoundIoInStream* stream, SoundIO_input* input);
void register_output(sio::SoundIoOutStream* stream, SoundIO_output* output);
void unregister_input(sio::SoundIoInStream* stream);
void unregister_output(sio::SoundIoOutStream* stream);

struct SoundIO_input {
  SoundIO_input() = delete;
  SoundIO_input(sio::SoundIoInStream* stream) : stream_{stream} {
    if (stream_ == nullptr) {
      throw std::runtime_error("SoundIo failed to create stream");
    }
    stream_->format = sio::SoundIoFormatFloat32NE;
    if (auto error = soundio_instream_open(stream_)) {
      sio::soundio_instream_destroy(stream_);
      throw std::runtime_error(fmt::format("SoundIO failed to open stream: {}",
                                           sio::soundio_strerror(error)));
    }
  }
  SoundIO_input(SoundIO_input const&) = delete;
  SoundIO_input(SoundIO_input&&) = default;
  SoundIO_input& operator=(SoundIO_input const&) = delete;
  SoundIO_input& operator=(SoundIO_input&&) = default;
  ~SoundIO_input() {
    sio::soundio_instream_destroy(stream_);
  }

private:
  sio::SoundIoInStream* stream_;
};

struct SoundIO_output {
  SoundIO_output() = delete;
  SoundIO_output(sio::SoundIoOutStream* stream) : stream_{stream} {
    if (stream_ == nullptr) {
      throw std::runtime_error("SoundIo failed to create stream");
    }
    stream_->format = sio::SoundIoFormatFloat32NE;
    stream_->write_callback = write_callback;
    if (auto error = soundio_outstream_open(stream_)) {
      sio::soundio_outstream_destroy(stream_);
      throw std::runtime_error(fmt::format("SoundIO failed to open stream: {}",
                                           sio::soundio_strerror(error)));
    }
    if (stream_->layout_error) {
      auto error_msg =
          fmt::format("Stream layout error: {}",
                      sio::soundio_strerror(stream_->layout_error));
      sio::soundio_outstream_destroy(stream_);
      throw std::runtime_error(error_msg);
    }
  }
  SoundIO_output(SoundIO_output const&) = delete;
  SoundIO_output(SoundIO_output&&) = default;
  SoundIO_output& operator=(SoundIO_output const&) = delete;
  SoundIO_output& operator=(SoundIO_output&&) = default;
  ~SoundIO_output() {
    sio::soundio_outstream_destroy(stream_);
  }

private:
  sio::SoundIoOutStream* stream_;
};

struct SoundIO_device {
  SoundIO_device() = delete;
  SoundIO_device(sio::SoundIoDevice* device) : device_{device} {
    if (device_ == nullptr) {
      throw std::runtime_error("SoundIo failed to create device");
    }
  }
  SoundIO_device(SoundIO_device const& device) = delete;
  SoundIO_device(SoundIO_device&& device) = default;
  SoundIO_device& operator=(SoundIO_device const& device) = delete;
  SoundIO_device& operator=(SoundIO_device&& device) = default;
  ~SoundIO_device() {
    sio::soundio_device_unref(device_);
  }

  auto get_name() {
    return std::string(device_->name);
  }

  auto get_output() {
    if (device_->aim != sio::SoundIoDeviceAimOutput) {
      throw std::runtime_error("Can't get output on non-output device");
    }
    return SoundIO_output(sio::soundio_outstream_create(device_));
  }

  auto get_input() {
    if (device_->aim != sio::SoundIoDeviceAimInput) {
      throw std::runtime_error("Can't get input on non-input device");
    }
    return SoundIO_input(sio::soundio_instream_create(device_));
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

  auto get_default_output_device() {
    auto device_index = sio::soundio_default_output_device_index(io_);
    if (device_index < 0) {
      throw std::runtime_error("SoundIO doesn't have default device");
    }
    return SoundIO_device(sio::soundio_get_output_device(io_, device_index));
  }

private:
  sio::SoundIo* io_;
};

} // namespace slashsynth

#endif

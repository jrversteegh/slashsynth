#ifndef SLASHSYNTH_OUTPUT_H_INCLUDED__
#define SLASHSYNTH_OUTPUT_H_INCLUDED__

namespace sio {
#include <soundio/soundio.h>
}
#include <fmt/format.h>

#include <cassert>
#include <exception>
#include <string>
#include <type_traits>

#include "sound.h"

namespace slashsynth {

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
  SoundIO_output(sio::SoundIoOutStream* stream)
      : stream_{stream}, buffer_{sio::soundio_ring_buffer_create(
                             stream->device->soundio, 0x400000)} {
    if (stream_ == nullptr) {
      throw_construction_error("SoundIo failed to create stream");
    }
    if (buffer_ == nullptr) {
      throw_construction_error("Failed to allocate ring buffer");
    }
    stream_->format = sio::SoundIoFormatFloat32NE;
    stream_->software_latency = 0.01;
    stream_->userdata = buffer_;
    if (auto error = soundio_outstream_open(stream_)) {
      throw_construction_error("SoundIO failed to open stream: {}", error);
    }
    if (stream_->layout_error) {
      throw_construction_error("SoundIO stream layout error: {}",
                               stream_->layout_error);
    }
    stream_->write_callback = [](sio::SoundIoOutStream* stream,
                                 [[maybe_unused]] int frame_count_min,
                                 int frame_count_max) {
      int frames_left = frame_count_max;
      int bytes_per_sample = stream->bytes_per_sample;
      int bytes_per_frame = bytes_per_sample * channel_count;
      while (frames_left > 0) {
        int frame_count = frames_left;
        sio::SoundIoChannelArea* areas;
        if (soundio_outstream_begin_write(stream, &areas, &frame_count) != 0) {
          // Couldn't begin writing, log some error?
          return;
        }
        sio::SoundIoRingBuffer* buffer =
            (sio::SoundIoRingBuffer*)stream->userdata;
        float* read_ptr = (float*)soundio_ring_buffer_read_ptr(buffer);
        int fill_bytes = soundio_ring_buffer_fill_count(buffer);
        int fill_count = fill_bytes / bytes_per_frame;
        int write_count = 0;
        size_t channels = std::min(
            static_cast<size_t>(stream->layout.channel_count), channel_count);
        for (int frame = 0; frame < frame_count; ++frame) {
          size_t channel = 0;
          if (write_count < fill_count) {
            while (channel < channels) {
              memcpy(areas[channel].ptr, read_ptr + channel, bytes_per_sample);
              areas[channel].ptr += areas[channel].step;
              ++channel;
            }
            read_ptr += channel_count;
            ++write_count;
          }
          while (channel < static_cast<size_t>(stream->layout.channel_count)) {
            memset(areas[channel].ptr, 0, bytes_per_sample);
            areas[channel].ptr += areas[channel].step;
            ++channel;
          }
        }
        if (soundio_outstream_end_write(stream) != 0) {
          // Couldn't end writing, log some error?
          return;
        }
        sio::soundio_ring_buffer_advance_read_ptr(buffer, write_count *
                                                              bytes_per_frame);
        frames_left -= frame_count;
      }
    };
    if (auto error = soundio_outstream_start(stream_)) {
      throw_construction_error("SoundIO failed to start stream: {}", error);
    }
  }
  SoundIO_output(SoundIO_output const&) = delete;
  SoundIO_output(SoundIO_output&&) = default;
  SoundIO_output& operator=(SoundIO_output const&) = delete;
  SoundIO_output& operator=(SoundIO_output&&) = default;
  ~SoundIO_output() {
    destroy();
  }

  auto get_sample_rate() const {
    return stream_->sample_rate;
  }

  void play(Samples const& samples) {
    int bytes_per_sample = stream_->bytes_per_sample;
    assert(bytes_per_sample == sizeof(float));
    int bytes_per_frame = bytes_per_sample * channel_count;
    float* write_ptr = (float*)soundio_ring_buffer_write_ptr(buffer_);
    size_t free_bytes =
        static_cast<size_t>(soundio_ring_buffer_free_count(buffer_));
    size_t free_count = free_bytes / bytes_per_frame;
    if (free_count < samples.size()) {
      throw std::runtime_error(fmt::format(
          "Too many samples ({}) for buffer ({})", samples.size(), free_count));
    }
    for (auto& sample : samples) {
      memcpy(write_ptr, sample.channel_values.data(), bytes_per_frame);
      write_ptr += channel_count;
    }
    sio::soundio_ring_buffer_advance_write_ptr(buffer_, samples.size() *
                                                            bytes_per_frame);
  }

private:
  sio::SoundIoOutStream* stream_;
  sio::SoundIoRingBuffer* buffer_;
  void destroy() {
    if (stream_ != nullptr) {
      sio::soundio_outstream_destroy(stream_);
    }
    if (buffer_ != nullptr) {
      sio::soundio_ring_buffer_destroy(buffer_);
    }
  }
  void throw_construction_error(char const* msg, int code = 0) {
    destroy();
    if (code != 0) {
      throw std::runtime_error(
          fmt::format(fmt::runtime(msg), sio::soundio_strerror(code)));
    } else {
      throw std::runtime_error(msg);
    }
  }
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
    io_->app_name = "slashsynth";
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

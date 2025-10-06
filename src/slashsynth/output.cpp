#include <slashsynth/output.h>

#include <map>

namespace slashsynth {

static std::map<sio::SoundIoInStream*, SoundIO_input*> input_map{};
static std::map<sio::SoundIoOutStream*, SoundIO_output*> output_map{};

void write_callback(sio::SoundIoOutStream* stream, int frame_count_min, int frame_count_max) {
}

void register_input(sio::SoundIoInStream* stream, SoundIO_input* input) {
  input_map[stream] = input;
}

void register_output(sio::SoundIoOutStream* stream, SoundIO_output* output) {
  output_map[stream] = output;
}

void unregister_input(sio::SoundIoInStream* stream) {
  input_map.erase(stream);
}

void unregister_output(sio::SoundIoOutStream* stream) {
  output_map.erase(stream);
}

}  // namespace slashsynth

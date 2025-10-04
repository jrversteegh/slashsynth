#include <soundio/soundio.h>

#include <iostream>

int main()
{
  SoundIo *soundio = soundio_create();
  if (soundio == nullptr) {
    std::cerr << "Failed to create SoundIO instance" << std::endl;
    return 1;
  }
  soundio_destroy(soundio);
  return 0;
}

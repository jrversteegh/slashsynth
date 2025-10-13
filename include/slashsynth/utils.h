#ifndef SLASHSYNTH_UTILS_H_INCLUDED__
#define SLASHSYNTH_UTILS_H_INCLUDED__

namespace slashsynth {

void remove_one(auto& container, auto const& value) {
  auto it = std::find(container.begin(), container.end(), value);
  if (it != container.end()) {
    container.erase(it);
  }
}

} // namespace slashsynth

#endif

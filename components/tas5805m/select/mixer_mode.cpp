#include "mixer_mode.h"

namespace esphome {
namespace tas5805m {

void MixerModeSelect::control(const std::string &value) {
  this->publish_state(value);
  auto index = this->index_of(value);
  if (index.has_value()) {
    bool ok = this->parent_->set_mixer_mode((index.value());
  }
}

}  // namespace tas5805m
}  // namespace esphome
#include "mixer_preset.h"

namespace esphome {
namespace tas5805m {

void MixerModePreset::setup() {

}

void MixerModePreset::dump_config() {

}

void MixerModePreset::control(const std::string &value) {
  this->publish_state(value);
  auto index = this->index_of(value);
  if (index.has_value()) {
    this->parent_->select_mixer_mode(index.value());
  }
}

}  // namespace tas5805m
}  // namespace esphome
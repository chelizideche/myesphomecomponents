#include "enable_dac_switch.h"

namespace esphome {
namespace tas5805m {

void EnableDacSwitch::write_state(bool state) {
  this->publish_state(state);
  this->parent_->enable_dac(state);
}

}  // namespace tas5805m
}  // namespace esphome

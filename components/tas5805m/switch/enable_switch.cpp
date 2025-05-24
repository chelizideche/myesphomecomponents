#include "enable_switch.h"

namespace esphome {
namespace tas5805m {

void EnableSwitch::write_state(bool state) {
  this->publish_state(state);
  this->parent_->set_enable(state);
}

}  // namespace tas5805m
}  // namespace esphome

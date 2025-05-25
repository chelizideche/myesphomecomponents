#include "enable_eq_switch.h"

namespace esphome {
namespace tas5805m {

void EnableEqSwitch::write_state(bool state) {
  this->publish_state(state);
  this->parent_->enable_eq(state);
}

}  // namespace tas5805m
}  // namespace esphome

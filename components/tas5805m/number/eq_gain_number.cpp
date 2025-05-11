#pragma once

#include "eq_gain_number.h"

namespace esphome {
namespace tas5805m {

void EqGainNumber::control(float value) {
  this->publish_state(value);
  ESP_LOGD(TAG, " Number Change to: %f", value);
  this->parent_->set_eq_gain(0, value);
}

}  // namespace tas5805m
}  // namespace esphome
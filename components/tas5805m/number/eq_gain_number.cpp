#pragma once

#include "eq_gain_number.h"

namespace esphome {
namespace tas5805m {

void EqGainNumber::control(float value) {
  this->publish_state(value);
  ESP_LOGD(TAG, " Number Change to: %f", value);
}

}  // namespace tas5805m
}  // namespace esphome
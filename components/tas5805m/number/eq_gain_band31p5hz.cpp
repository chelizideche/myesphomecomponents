#include "eq_gain20hz_band.h"

namespace esphome {
namespace tas5805m {

void EqGainBand31p5hz::control(float value) {
  this->publish_state(value);
  this->parent_->set_eq_gain(0, static_cast<int>(value));
}

}  // namespace tas5805m
}  // namespace esphome
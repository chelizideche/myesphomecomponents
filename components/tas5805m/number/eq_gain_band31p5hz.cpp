#include "eq_gain_band31p5hz.h"

namespace esphome {
namespace tas5805m {

void EqGainBand31p5hz::setup() {
  float value;
  this->pref_ = global_preferences->make_preference<float>(this->get_object_id_hash());
  if (!this->pref_.load(&value)) value= 0.0;
  this->publish_state(value);
}

void EqGainBand31p5hz::dump_config() {
  LOG_NUMBER("", "Tas5805m EQ Gain Band 31.5Hz", this);
}

void EqGainBand31p5hz::control(float value) {
  this->publish_state(value);
  this->parent_->set_eq_gain(1, static_cast<int>(value));
  this->pref_.save(&value);
}

}  // namespace tas5805m
}  // namespace esphome
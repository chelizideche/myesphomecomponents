#include "eq_gain_band50hz.h"

namespace esphome {
namespace tas5805m {

void EqGainBand50hz::setup() {
  float value;
  this->pref_ = global_preferences->make_preference<float>(this->get_object_id_hash());
  if (!this->pref_.load(&value)) value= 0.0;
  this->publish_state(value);
}

void EqGainBand50hz::dump_config() {
  LOG_NUMBER("", "Tas5805m EQ Gain Band 50Hz", this);
}
void EqGainBand50hz::control(float value) {
  this->publish_state(value);
  this->parent_->set_eq_gain(2, static_cast<int>(value));
  this->pref_.save(&value);
}

}  // namespace tas5805m
}  // namespace esphome
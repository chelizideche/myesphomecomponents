#include "eq_gain_band80hz.h"

namespace esphome {
namespace tas5805m {

static const char *const TAG = "Tas5805m.number";

void EqGainBand80hz::setup() {
  float value;
  this->pref_ = global_preferences->make_preference<float>(this->get_object_id_hash());
  if (!this->pref_.load(&value)) value= 0.0;
  this->publish_state(value);
}

void EqGainBand80hz::dump_config() {
  LOG_NUMBER("", "TEQ Gain Band 80Hz", this);
}

void EqGainBand80hz::control(float value) {
  this->publish_state(value);
  this->parent_->set_eq_gain(3, static_cast<int>(value));
  this->pref_.save(&value);
}

}  // namespace tas5805m
}  // namespace esphome
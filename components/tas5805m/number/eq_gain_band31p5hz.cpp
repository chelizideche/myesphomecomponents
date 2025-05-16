#include "esphome/core/log.h"
#include "eq_gain_band31p5hz.h"

namespace esphome {
namespace tas5805m {

static const char *const TAG = "Tas5805m.number";

void EqGainBand31p5hz::setup() {
  float value;
  this->pref_ = global_preferences->make_preference<float>(this->get_object_id_hash());
  if (!this->pref_.load(&value)) value= 0.0;
  this->publish_state(value);
}

void EqGainBand31p5hz::dump_config() {
  ESP_LOGCONFIG(TAG, "%s '%s'", "Gain Number for EQ Band", this->get_name().c_str());
}

void EqGainBand31p5hz::control(float value) {
  this->publish_state(value);
  this->parent_->set_eq_gain(BAND_31_5HZ, static_cast<int>(value));
  this->pref_.save(&value);
}

}  // namespace tas5805m
}  // namespace esphome
#include "esphome/core/log.h"
#include "eq_gain_band125hz.h"

namespace esphome {
namespace tas5805m {

static const char *const TAG = "Tas5805m.number";

void EqGainBand125hz::setup() {
  float value;
  this->pref_ = global_preferences->make_preference<float>(this->get_object_id_hash());
  if (!this->pref_.load(&value)) value = 0.0; // no saved gain so set to 0dB
  this->publish_state(value);
}

void EqGainBand125hz::dump_config() {
  ESP_LOGCONFIG(TAG, "%s '%s'", "Number: Gain 125Hz EQ Band", this->get_name().c_str());
}

void EqGainBand125hz::control(float value) {
  this->publish_state(value);
  this->parent_->set_eq_gain(BAND_125HZ, static_cast<int>(value));
  this->pref_.save(&value);
}

}  // namespace tas5805m
}  // namespace esphome
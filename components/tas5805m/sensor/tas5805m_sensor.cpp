#include "esphome/core/log.h"
#include "tas5805m_sensor.h"

namespace esphome {
namespace tas5805m {

static const char *const TAG = "Tas5805m.sensor";

void Tas5805mSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "Tas5805m sensor:");
  LOG_SENSOR("  ", "Recovery Count Sensor", this->recovery_count_sensor_);
}

void Tas5805mSensor::update()  {
  this->parent_->refresh_faults();
  if (this->recovery_count_sensor_ != nullptr) {
    this->recovery_count_sensor_->publish_state(this->parent_->get_auto_clear_faults_count());
    return;
  }
  ESP_LOGE(TAG, "  Recovery Count Sensor is not setup");
  return;
}

}  // namespace tas5805m
}  // namespace esphome
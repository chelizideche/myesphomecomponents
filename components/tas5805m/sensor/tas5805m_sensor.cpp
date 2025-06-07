#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "tas5805m_sensor.h"

namespace esphome {
namespace tas5805m {

static const char *const TAG = "Tas5805m.sensor";

void  Tas5805mSensor::dump_config() {
  LOG_SENSOR("  ", "Auto Fault Recovery Count:", this->recovery_count_sensor_);
  // LOG_SENSOR("  ", "Last Channel Fault Code:", this->last_channel_fault_);
  // LOG_SENSOR("  ", "Last Global Fault 1 Code:", this->last_global_fault1_sensor_);
  // LOG_SENSOR("  ", "Last Global Fault 2 Code:", this->last_global_fault2_sensor_);
}

void  Tas5805mSensor::update() {
  this->parent_->refresh_faults();
  if (this->recovery_count_sensor_ != nullptr) {
    float recovery_count = this->parent_->get_auto_clear_faults_count();
    this->recovery_count_sensor_->publish_state(recovery_count);
  }
  // if (this->last_channel_fault_ != nullptr) {
  //   float channel_fault = this->parent_->get_last_channel_fault();
  //   this->last_channel_fault_->publish_state(channel_fault);
  // }
  // if (this->last_global_fault1_sensor_ != nullptr) {
  //   float global_fault1 = this->parent_->get_last_global_fault1();
  //   this->last_global_fault1_sensor->publish_state(global_fault1);
  // }
  // if (this->last_global_fault2_sensor_ != nullptr) {
  //   float global_fault2 = this->parent_->get_last_global_fault2();
  //   this->last_global_fault2_sensor->publish_state(global_fault2);
  // }
}

}  // namespace tas5805m
}  // namespace esphome
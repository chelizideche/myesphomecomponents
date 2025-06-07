#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "tas5805m_sensor.h"

namespace esphome {
namespace tas5805m {

static const char *const TAG = "Tas5805m.sensor";

void  Tas5805mSensor::dump_config() {
  LOG_SENSOR("  ", "Clear Faults:", this->clear_faults_sensor_);
  LOG_SENSOR("  ", "Last Channel Fault Code:", this->last_channel_fault_sensor_);
  LOG_SENSOR("  ", "Last Global Fault Code:", this->last_global_fault_sensor_);
}

void  Tas5805mSensor::update() {
  if (this->first_update_) {
    this->parent_->reset_faults();
    this->first_update_ = false;
  }

  this->parent_->refresh_faults();
  if (this->clear_faults_sensor_ != nullptr) {
    this->clear_faults_sensor_->publish_state(this->parent_->number_of_clear_faults());
  }
  if (this->last_channel_fault_sensor_ != nullptr) {
    this->last_channel_fault_sensor_->publish_state(this->parent_->last_channel_fault());
  }
  if (this->last_global_fault_sensor_ != nullptr) {
    this->last_global_fault_sensor_->publish_state(this->parent_->last_global_fault());
  }
}

}  // namespace tas5805m
}  // namespace esphome
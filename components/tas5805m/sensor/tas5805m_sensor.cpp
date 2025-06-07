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
void Tas5805mSensor::setup() {
   this->set_timeout(100, [this]() { this->parent_->reset_faults(); });
}

void  Tas5805mSensor::update() {
  // if (this->first_update_) {
  //   this->parent_->reset_faults();
  //   this->first_update_ = false;
  // }

  this->parent_->refresh_faults();
  if (this->clear_faults_sensor_ != nullptr) {
    if ( (uint32_t)(this->clear_faults_sensor_->get_state()) != this->parent_->number_of_clear_faults() ) {
      this->clear_faults_sensor_->publish_state(this->parent_->number_of_clear_faults());
    }
  }
  if (this->last_channel_fault_sensor_ != nullptr) {
    uint8_t new_fault_code = this->parent_->last_channel_fault();
    if ( (uint8_t)(this->last_channel_fault_sensor_->get_state()) != new_fault_code ) {
      this->last_channel_fault_sensor_->publish_state(new_fault_code);
      if (new_fault_code) {
        if (new_fault_code & (1 << 0)) ESP_LOGW(TAG, "Tas5805m Right channel over current fault");
        if (new_fault_code & (1 << 1)) ESP_LOGW(TAG, "Tas5805m Left channel over current fault");
        if (new_fault_code & (1 << 2)) ESP_LOGW(TAG, "Tas5805m Right channel DC fault");
        if (new_fault_code & (1 << 3)) ESP_LOGW(TAG, "Tas5805m Left channel DC fault");
      }
    }
  }
  if (this->last_global_fault_sensor_ != nullptr) {
    uint8_t new_fault_code = this->parent_->last_global_fault();

    if ( (uint8_t)(this->last_global_fault_sensor_->get_state()) != new_fault_code ) {
      this->last_global_fault_sensor_->publish_state(new_fault_code);
      if (new_fault_code) {
        if (new_fault_code & (1 << 0)) ESP_LOGW(TAG, "Tas5805m PVDD under-voltage fault");
        if (new_fault_code & (1 << 1)) ESP_LOGW(TAG, "Tas5805m PVDD over-voltage fault");
        if (new_fault_code & (1 << 2)) ESP_LOGW(TAG, "Tas5805m Clock fault");
        if (new_fault_code & (1 << 6)) ESP_LOGW(TAG, "Recent BQ write failed");
        if (new_fault_code & (1 << 7)) ESP_LOGW(TAG, "OTP CRC check error");
      }
    }
  }
}

}  // namespace tas5805m
}  // namespace esphome
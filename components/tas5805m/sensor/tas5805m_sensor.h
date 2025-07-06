#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"
#include "../tas5805m.h"

namespace esphome {
namespace tas5805m {

class Tas5805mSensor : public PollingComponent, public Parented<Tas5805mComponent> {
 public:
  void dump_config() override;
  void update() override;
  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

  void set_times_faults_cleared_sensor(sensor::Sensor* sensor) { times_faults_cleared_sensor_ = sensor; }
  //void set_last_channel_fault_sensor(sensor::Sensor *sensor) { this->last_channel_fault_sensor_ = sensor; }
  //void set_last_global_fault_sensor(sensor::Sensor *sensor) { this->last_global_fault_sensor_ = sensor; }

 protected:
  sensor::Sensor* times_faults_cleared_sensor_{nullptr};

  // initialise as large number so first value of first update interval is saved
  uint32_t last_faults_cleared_{1000}; // large number so first value of first update interval is saved

  //sensor::Sensor *last_channel_fault_sensor_{nullptr};
  //sensor::Sensor *last_global_fault_sensor_{nullptr};
};

}  // namespace tas5805m
}  // namespace esphome
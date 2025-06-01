#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"
#include "../tas5805m.h"

namespace esphome {
namespace tas5805m {

class TAS5805MSensor : public sensor.Sensor, public PollingComponent, public Parented<Tas5805mComponent> {
 public:
  //TAS5805MSensor(Tas5805mComponent *parent) : parent_(parent) {}

  void dump_config() override;
  void update() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_recovery_count_sensor(sensor::Sensor *sensor) { this->recovery_count_sensor_ = sensor; }
  // void set_last_channel_fault_sensor(sensor::Sensor *sensor) { this->last_channel_fault_ = sensor; }
  // void set_last_global_fault1_sensor(sensor::Sensor *sensor) { this->last_global_fault1_sensor_ = sensor; }
  // void set_last_global_fault2_sensor(sensor::Sensor *sensor) { this->last_global_fault2_sensor_ = sensor; }

 protected:
  //Tas5805mComponent *parent_;
  sensor::Sensor *recovery_count_sensor_{nullptr};
  // sensor::Sensor *last_channel_fault_sensor_{nullptr};
  // sensor::Sensor *last_global_fault1_sensor_{nullptr};
  // sensor::Sensor *last_global_fault2_sensor_{nullptr};
};

}  // namespace tas5805m
}  // namespace esphome
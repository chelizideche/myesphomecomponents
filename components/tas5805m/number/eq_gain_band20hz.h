#pragma once

#include "esphome/components/number/number.h"
#include "esphome/core/component.h"
#include "esphome/core/preferences.h"
#include "../tas5805m.h"

namespace esphome {
namespace tas5805m {

class EqGainBand20hz : public number::Number, public Component {
 public:
  //EqGainBand20hz() = default;

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::HARDWARE; }
  void set_20hz_parent(Tas5805mComponent *parent) { this->parent_ = parent; }

 protected:
  void control(float value) override;

  Tas5805mComponent *parent_;

  ESPPreferenceObject pref_;
};

}  // namespace tas5805m
}  // namespace esphome
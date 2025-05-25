#pragma once

#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"
#include "../tas5805m.h"

namespace esphome {
namespace tas5805m {

class EnableEqSwitch : public switch_::Switch, public Component, public Parented<Tas5805mComponent> {
public:se
  float get_setup_priority() const override { return setup_priority::BEFORE_CONNECTION;; }

 protected:
  void write_state(bool state) override;
};

}  // namespace tas5805m
}  // namespace esphome

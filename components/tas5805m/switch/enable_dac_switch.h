#pragma once

#include "esphome/components/switch/switch.h"
#include "../tas5805m.h"

namespace esphome {
namespace tas5805m {

class EnableDacSwitch : public switch_::Switch, public Parented<Tas5805mComponent> {
 protected:
  void write_state(bool state) override;
};

}  // namespace tas5805m
}  // namespace esphome

#pragma once

#include "esphome/components/select/select.h"
//#include "esphome/core/component.h"
#include "../tas5805m.h"

namespace esphome {
namespace tas5805m {

class MixerModeSelect : public select::Select, public Parented<Tas5805mComponent> {
 public:
  MixerModeSelect() = default;
  // void setup() override;
  // void dump_config() override;

 protected:
  void control(const std::string &value) override;
};

}  // namespace tas5805m
}  // namespace esphome
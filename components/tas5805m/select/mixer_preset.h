#pragma once

#include "esphome/components/select/select.h"
#include "../tas5805m.h"

namespace esphome {
namespace tas5805m {

class MixerModePreset : public select::Select, public Parented<Tas5805mComponent> {
 public:
  MixerModePreset() = default;

 protected:
  void control(const std::string &value) override;
};

}  // namespace tas5805m
}  // namespace esphome
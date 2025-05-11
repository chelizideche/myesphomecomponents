#pragma once

#include "esphome/components/number/number.h"
#include "../tas5805m.h"

namespace esphome {
namespace tas5805m {

class EqGainNumber : public number::Number, public Parented<Tas5805mComponent> {
 public:
  EqGainNumber() = default;

 protected:
  void control(float value) override;
};

}  // namespace tas5805m
}  // namespace esphome
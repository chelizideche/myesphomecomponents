#pragma once

#include "esphome/components/number/number.h"
#include "../tas5805m.h"

namespace esphome {
namespace tas5805m {

class EqGainBand31p5hz : public number::Number, public Parented<Tas5805mComponent> {
 public:
  EqGainBand31p5hz() = default;

 protected:
  void control(float value) override;
};

}  // namespace tas5805m
}  // namespace esphome
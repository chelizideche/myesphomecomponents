#pragma once

#include "esphome/components/audio_dac/audio_dac.h"
#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/hal.h"


namespace esphome {
namespace tas5805m {

struct tas5805m_cfg_reg_t {
  uint8_t offset;
  uint8_t value;
};

class Tas5805mComponent : public audio_dac::AudioDac, public Component, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::IO; }

  void set_enable_pin(GPIOPin *enable) { this->enable_pin_ = enable; }

  float volume() override { return this->volume_; }
  bool set_volume(float value) override;

  bool set_mute_off() override;
  bool set_mute_on() override;
  bool is_muted() override { return this->is_muted_; }

  bool set_deep_sleep_off();
  bool set_deep_sleep_on();

 protected:
   GPIOPin *enable_pin_{nullptr};

   bool configure_registers();

   bool get_state(ControlState* state);
   bool set_state(ControlState state);

   bool get_digital_volume(uint8_t*  raw_volume);
   bool set_digital_volume(uint8_t new_volume);

   bool get_gain(uint8_t* raw_gain);
   bool set_gain(uint8_t new_gain);

   bool tas5805m_read_byte(uint8_t a_register, uint8_t* data);
   bool tas5805m_write_byte(uint8_t a_register, uint8_t data);
   bool tas5805m_write_bytes(uint8_t a_register, uint8_t *data, uint8_t len);

   enum ErrorCode {
     NONE = 0,
     CONFIGURATION_FAILED,
     WRITE_REGISTER_FAILED
   } error_code_{NONE};

   enum ControlState {
    CTRL_DEEP_SLEEP = 0x00, // Deep Sleep
    CTRL_SLEEP      = 0x01, // Sleep
    CTRL_HI_Z       = 0x02, // Hi-Z
    CTRL_PLAY       = 0x03, // Play
    CTRL_MUTE       = 0x08  // LR Channel Mute
   };

  enum MixerMode {
    MIXER_UNKNOWN = 0,
    MIXER_STEREO,
    MIXER_STEREO_INVERSE,
    MIXER_MONO,
    MIXER_RIGHT,
    MIXER_LEFT,
  };

  struct {
    //bool           is_muted; // not used as esphome AudioDac component has its own is_muted variables
    bool           is_powered;
    //int8_t       eq_gain[TAS5805M_EQ_BANDS]; // not implemented yet
    ControlState   state;
    MixerMode      mixer_mode;
  } tas5805_state_;

   float volume_{0};

   uint8_t i2c_error_{0};
   uint8_t analog_gain_{0};
   uint8_t digital_volume_{0};

   uint16_t number_registers_configured_{0};
};

}  // namespace tas5805m
}  // namespace esphome

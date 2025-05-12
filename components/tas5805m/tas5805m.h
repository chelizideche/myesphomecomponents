#pragma once

#include "esphome/components/audio_dac/audio_dac.h"
#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/hal.h"

#include "tas5805m_cfg.h"
#include "tas5805m_eq.h"

namespace esphome {
namespace tas5805m {

class Tas5805mComponent : public audio_dac::AudioDac, public Component, public i2c::I2CDevice {
 public:
  void config_analog_gain(int8_t analog_gain) { analog_gain_ = analog_gain; }

  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::IO; }

  void set_enable_pin(GPIOPin *enable) { this->enable_pin_ = enable; }
  void config_analog_gain(float analog_gain) { this->analog_gain_ = analog_gain; }

  float volume() override { return this->volume_; }
  bool set_volume(float value) override;

  bool set_mute_off() override;
  bool set_mute_on() override;
  bool is_muted() override { return this->is_muted_; }

  bool set_deep_sleep_off();
  bool set_deep_sleep_on();

  bool set_eq_on();
  bool set_eq_off();
  bool set_eq_gain(uint8_t band, int8_t gain);
  int8_t eq_gain(uint8_t band);
  void refresh_eq_gains();

 protected:
   GPIOPin *enable_pin_{nullptr};

   bool configure_registers();

   bool get_state(Tas5805mControlState* state);
   bool set_state(Tas5805mControlState state);

   bool get_digital_volume(uint8_t*  raw_volume);
   bool set_digital_volume(uint8_t new_volume);

   // set analog gain in dB from -15.5 to 0 in 0.5dB increments
   bool set_analog_gain(float gain_db);

   // reads analog gain register and returns number 0-31
   bool get_analog_gain(uint8_t* raw_gain);

   bool get_dac_mode(Tas5805mDacMode* mode);
   bool get_eq(bool* enabled);
   bool get_modulation_mode(Tas5805mModMode* mode, Tas5805mSwFreq* freq, Tas5805mBdFreq* bd_freq);
   bool get_fs_freq(Tas5805mFsFreq* freq);
   bool get_bck_ratio(uint8_t* ratio);
   bool get_power_state(Tas5805mControlState* state);
   bool set_eq(bool enable);

   bool set_book_and_page(uint8_t book, uint8_t page);

   bool tas5805m_read_byte(uint8_t a_register, uint8_t* data);
   bool tas5805m_write_byte(uint8_t a_register, uint8_t data);
   bool tas5805m_write_bytes(uint8_t a_register, uint8_t *data, uint8_t len);

   enum ErrorCode {
     NONE = 0,
     CONFIGURATION_FAILED,
     WRITE_REGISTER_FAILED
   } error_code_{NONE};

   struct Tas5805mState {
    //bool                 is_muted;                   // not used as esphome AudioDac component has its own is_muted variable
    bool                   is_powered;
    bool                   eq_enabled;
    int8_t                 eq_gain[TAS5805M_EQ_BANDS]{0};
    bool                   eq_gain_set[TAS5805M_EQ_BANDS]{false};
    Tas5805mControlState   state;
    Tas5805mMixerMode      mixer_mode;
   } tas5805m_state_;

   bool run_refresh_eq_gains_{false};

   float volume_{0};

   float analog_gain_{0};

   uint8_t i2c_error_{0};

   uint8_t digital_volume_{0};

   uint8_t refresh_band_{0};

   uint16_t number_registers_configured_{0};
};

}  // namespace tas5805m
}  // namespace esphome

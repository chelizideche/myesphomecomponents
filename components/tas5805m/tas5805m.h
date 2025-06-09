#pragma once

#include "esphome/components/audio_dac/audio_dac.h"
#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/hal.h"
#include "tas5805m_cfg.h"

#ifdef USE_TAS5805M_EQ
#include "tas5805m_eq.h"
#endif

#ifdef USE_SELECT
#include "esphome/components/select/select.h"
#endif

namespace esphome {
namespace tas5805m {

class Tas5805mComponent : public audio_dac::AudioDac, public Component, public i2c::I2CDevice {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::IO; }

  void set_enable_pin(GPIOPin *enable) { this->enable_pin_ = enable; }
  void config_analog_gain(float analog_gain) { this->tas5805m_state_.analog_gain = analog_gain; }
  void config_dac_mode(DacMode dac_mode) {this->tas5805m_state_.dac_mode = dac_mode; }
  void config_mixer_mode(MixerMode mixer_mode) {this->tas5805m_state_.mixer_mode = mixer_mode; }

  #ifdef USE_SELECT
  void set_mixer_mode_select(select::Select *select) { this->mixer_mode_select_ = select; }
  #endif

  float volume() override;
  bool set_volume(float value) override;

  bool set_mute_off() override;
  bool set_mute_on() override;
  bool is_muted() override { return this->is_muted_; }

  bool set_deep_sleep_off();
  bool set_deep_sleep_on();

  bool set_mixer_mode(MixerMode mode);

  bool refresh_faults();
  bool reset_faults();

  uint32_t number_of_clear_faults();
  uint8_t last_channel_fault();
  uint8_t last_global_fault();
  bool last_over_temperature_fault_state();
  bool last_over_temperature_warning_state();

  bool set_eq_on();
  bool set_eq_off();
  void refresh_eq_gains();

  #ifdef USE_TAS5805M_EQ
  bool set_eq_gain(uint8_t band, int8_t gain);
  #endif

 protected:
   GPIOPin *enable_pin_{nullptr};

   bool configure_registers();

   // set analog gain in dB from -15.5 to 0 in 0.5dB increments
   bool set_analog_gain(float gain_db);

   // reads analog gain register and returns number 0-31
   bool get_analog_gain(uint8_t* raw_gain);

   bool get_dac_mode(DacMode* mode);
   bool set_dac_mode(DacMode mode);

   bool get_mixer_mode(MixerMode *mode);

   bool get_digital_volume(uint8_t*  raw_volume);
   bool set_digital_volume(uint8_t new_volume);

   bool get_state(ControlState* state);
   bool set_state(ControlState state);

   bool clear_faults();

   #ifdef USE_TAS5805M_EQ
   bool get_eq(bool* enabled);
   bool set_eq(bool enable);
   bool set_book_and_page(uint8_t book, uint8_t page);

   #endif

  //int8_t eq_gain(uint8_t band);


  //  bool get_modulation_mode(ModMode* mode, SwFreq* freq, BdFreq* bd_freq);
  //  bool get_fs_freq(FsFreq* freq);
  //  bool get_bck_ratio(uint8_t* ratio);
  //  bool get_power_state(ControlState* state);

   bool tas5805m_read_byte(uint8_t a_register, uint8_t* data);
   bool tas5805m_write_byte(uint8_t a_register, uint8_t data);
   bool tas5805m_write_bytes(uint8_t a_register, uint8_t *data, uint8_t len);

   enum ErrorCode {
     NONE = 0,
     CONFIGURATION_FAILED,
     WRITE_REGISTER_FAILED
   } error_code_{NONE};

   struct Tas5805mState {
    //bool               is_muted;                   // not used as esphome AudioDac component has its own is_muted variable
    //bool               is_powered;                 // currently not used
    float             analog_gain;
    DacMode           dac_mode;
    MixerMode         mixer_mode;

    ControlState      control_state;

    uint32_t          number_of_clear_faults{0};
    uint8_t           last_channel_fault{0};
    uint8_t           last_global_fault{0};
    uint8_t           last_over_temperature_fault{0};
    uint8_t           last_over_temperature_warning{0};

    #ifdef USE_TAS5805M_EQ
    bool              eq_enabled{false};
    int8_t            eq_gain[TAS5805M_EQ_BANDS]{0};
    #endif

   } tas5805m_state_;

   #ifdef USE_SELECT
   select::Select* mixer_mode_select_{nullptr};
   #endif

   bool running_refresh_eq_gains_{false};
   bool eq_gains_refresh_initiated_{false};

   uint8_t refresh_band_{0};
   uint8_t i2c_error_{0};
   uint8_t loop_counter_{0};

   uint16_t number_registers_configured_{0};
};

}  // namespace tas5805m
}  // namespace esphome

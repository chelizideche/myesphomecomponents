#pragma once

#include "esphome/components/audio_dac/audio_dac.h"
#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/hal.h"

#include "tas5805m_cfg.h"
#include "tas5805m_eq.h"

namespace esphome {
namespace tas5805m {

static const uint8_t  TAS5805M_EQ_BANDS = 15;

enum Tas5805mControlState : uint8_t {
  CTRL_DEEP_SLEEP = 0x00, // Deep Sleep
  CTRL_SLEEP      = 0x01, // Sleep
  CTRL_HI_Z       = 0x02, // Hi-Z
  CTRL_PLAY       = 0x03, // Play
 };

enum Tas5805mDacMode: uint8_t  {
  DAC_MODE_BTL  = 0x00, // Bridge tied load
  DAC_MODE_PBTL = 0x01, // Parallel load
};

enum Tas5805mSwFreq : uint8_t {
  SW_FREQ_768K = (0x00 << 4),
  SW_FREQ_384K = (0x01 << 4),
  SW_FREQ_480K = (0x03 << 4),
  SW_FREQ_576K = (0x04 << 4),
};

enum Tas5805mBdFreq : uint8_t {
  bd_FREQ_80K  = (0x00 << 5),
  bd_FREQ_100K = (0x01 << 5),
  bd_FREQ_120K = (0x02 << 5),
  bd_FREQ_175K = (0x03 << 5),
};

enum Tas5805mModMode : uint8_t {
  MOD_MODE_BD     = 0x00,
  MOD_MODE_1SPW   = 0x01,
  MOD_MODE_HYBRID = 0x02,
};

enum Tas5805mMixerMode : uint8_t {
  MIXER_UNKNOWN = 0,
  MIXER_STEREO,
  MIXER_STEREO_INVERSE,
  MIXER_MONO,
  MIXER_RIGHT,
  MIXER_LEFT,
};

enum Tas5805mFsFreq : uint8_t {
  FS_FREQ_ERROR     = 0b0000,
  FS_FREQ_8K        = 0b0010,
  FS_FREQ_16K       = 0b0100,
  FS_FREQ_32K       = 0b0110,
  FS_FREQ_48K       = 0b1001,
  FS_FREQ_96K       = 0b1011,
  FS_FREQ_RESERVED  = 0b1000,
};

struct Tas5805mConfiguration {
  uint8_t offset;
  uint8_t value;
};

struct Tas5805mFault {
  uint8_t err0;
  uint8_t err1;
  uint8_t err2;
  uint8_t ot_warn;
};

class Tas5805mComponent : public audio_dac::AudioDac, public Component, public i2c::I2CDevice {
 public:
  void config_analog_gain(int8_t analog_gain) { analog_gain_ = analog_gain; }

  void setup() override;
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
    Tas5805mControlState   state;
    Tas5805mMixerMode      mixer_mode;
   } tas5805m_state_;

   float volume_{0};

   float analog_gain_{0};
   uint8_t raw_analog_gain_val_{0};
   uint8_t raw_analog_gain_reg_{0};

   uint8_t i2c_error_{0};

   uint8_t digital_volume_{0};

   uint16_t number_registers_configured_{0};
};

}  // namespace tas5805m
}  // namespace esphome

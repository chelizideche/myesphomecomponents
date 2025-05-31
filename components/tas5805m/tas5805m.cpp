#include "tas5805m.h"
#include "tas5805m_minimal.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace tas5805m {

static const char *const TAG = "tas5805m";

static const uint8_t TAS5805M_MUTE_CONTROL   = 0x08;  // LR Channel Mute

static const uint8_t ESPHOME_MAXIMUM_DELAY   = 5;     // milliseconds
static const uint8_t DELAY_LOOPS             = 30;    // number of loop iterations about equal 500ms

void Tas5805mComponent::setup() {
  if (this->enable_pin_ != nullptr) {
    // Set enable pin as OUTPUT and disable the enable pin
    this->enable_pin_->setup();
    this->enable_pin_->digital_write(false);
    delay(10);
    this->enable_pin_->digital_write(true);
  }

  if (!configure_registers()) {
    this->error_code_ = CONFIGURATION_FAILED;
    this->mark_failed();
  }
}

void Tas5805mComponent::loop() {
  // when tas5805m has detected i2s clock, eq gains can be written
  // do a re-write of gains for all eq bands when triggered by boolean 'run_refresh_eq_gains_'
  // write gains one band per loop so component does not take too long in loop

  if (!this->running_refresh_eq_gains_) return;

  // once refresh eq gains is activated wait 'DELAY_LOOPS' before execution
  // to ensure on boot sound has played and tas5805m has detected i2s clock
  if (this->loop_counter_ < DELAY_LOOPS) {
    this->loop_counter_ = this->loop_counter_ + 1;
    return;
  }

  // refresh_band_ is initially set to 0 in tas5805m.h
  // when finished reset variables ready for next time
  if (this->refresh_band_ == TAS5805M_EQ_BANDS) {
    this->running_refresh_eq_gains_ = false;
    this->refresh_band_ = 0;
    this->loop_counter_ = 0;
    return;
  }

  // re-write gains of current band and increment to next band ready for when loop next runs
  this->set_eq_gain(this->refresh_band_, this->tas5805m_state_.eq_gain[this->refresh_band_]);
  this->refresh_band_ = this->refresh_band_ + 1;
  return;
}

bool Tas5805mComponent::configure_registers() {
  uint16_t i = 0;
  uint16_t counter = 0;
  uint16_t number_configurations = sizeof(tas5805m_registers) / sizeof(tas5805m_registers[0]);

  while (i < number_configurations) {
    switch (tas5805m_registers[i].offset) {
      case TAS5805M_CFG_META_DELAY:
        if (tas5805m_registers[i].value > ESPHOME_MAXIMUM_DELAY) return false;
        delay(tas5805m_registers[i].value);
        break;
      default:
        if (!this->tas5805m_write_byte(tas5805m_registers[i].offset, tas5805m_registers[i].value)) return false;
        counter++;
        break;
    }
    i++;
  }
  this->number_registers_configured_ = counter;

  if (!this->set_dac_mode(this->tas5805m_state_.dac_mode)) return false;
  if (!this->set_mixer_mode(this->tas5805m_state_.mixer_mode)) return false;
  if (!this->set_analog_gain(this->tas5805m_state_.analog_gain)) return false;
  if (!this->set_state(CTRL_PLAY)) return false;
  if (!this->set_eq(false)) return false;
  return true;
}

void Tas5805mComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Tas5805m:");

  switch (this->error_code_) {
    case CONFIGURATION_FAILED:
      ESP_LOGE(TAG, "  Write configuration failure with error code = %i",this->i2c_error_);
      break;
    case WRITE_REGISTER_FAILED:
      ESP_LOGE(TAG, "  Write register failure with error code = %i",this->i2c_error_);
      break;
    case NONE:
      ESP_LOGD(TAG, "  Registers configured: %i", this->number_registers_configured_);
      ESP_LOGD(TAG, "  DAC mode: %s", this->tas5805m_state_.dac_mode ? "PBTL" : "BTL");
      ESP_LOGD(TAG, "  Mixer mode: %s", MIXER_MODE_TEXT[this->tas5805m_state_.mixer_mode]);
      ESP_LOGD(TAG, "  Analog Gain: %3.1fdB", this->tas5805m_state_.analog_gain);
      ESP_LOGD(TAG, "  Setup successful");
      LOG_I2C_DEVICE(this);
      break;
  }
}

bool Tas5805mComponent::set_volume(float volume) {
  float new_volume = clamp(volume, 0.0f, 1.0f);
  uint8_t raw_volume = remap<uint8_t, float>(new_volume, 0.0f, 1.0f, 254, 0);
  if (!this->set_digital_volume(raw_volume)) return false;
  ESP_LOGD(TAG, "  Volume changed to: %2.0f%%", new_volume*100);
  return true;
}

float Tas5805mComponent::volume() {
  uint8_t raw_volume;
  get_digital_volume(&raw_volume);
  return remap<float, uint8_t>(raw_volume, 254, 0, 0.0f, 1.0f);
}

bool Tas5805mComponent::set_mute_off() {
  if (!this->is_muted_) return true;
  if (!this->tas5805m_write_byte(TAS5805M_DEVICE_CTRL_2, this->tas5805m_state_.control_state)) return false;
  this->is_muted_ = false;
  ESP_LOGD(TAG, "  Tas5805m Mute Off");
  return true;
}

// set bit 3 MUTE in TAS5805M_DEVICE_CTRL_2 and retain current Control State
// ensures get_state = get_power_state
bool Tas5805mComponent::set_mute_on() {
  if (this->is_muted_) return true;
  if (!this->tas5805m_write_byte(TAS5805M_DEVICE_CTRL_2, this->tas5805m_state_.control_state + TAS5805M_MUTE_CONTROL)) return false;
  this->is_muted_ = true;
  ESP_LOGD(TAG, "  Tas5805m Mute On");
  return true;
}

bool Tas5805mComponent::set_deep_sleep_on() {
  if (this->tas5805m_state_.control_state == CTRL_DEEP_SLEEP) return true; // already in deep sleep

  // preserve mute state
  uint8_t new_value = (this->is_muted_) ? (CTRL_DEEP_SLEEP + TAS5805M_MUTE_CONTROL) : CTRL_DEEP_SLEEP;
  if (!this->tas5805m_write_byte(TAS5805M_DEVICE_CTRL_2, new_value)) return false;

  this->tas5805m_state_.control_state = CTRL_DEEP_SLEEP;                   // set Control State to deep sleep
  ESP_LOGD(TAG, "  Tas5805m Deep Sleep On");
  if (this->is_muted_) ESP_LOGD(TAG, "  Tas5805m Mute On preserved");
  return true;
}

bool Tas5805mComponent::set_deep_sleep_off() {
  if (this->tas5805m_state_.control_state != CTRL_DEEP_SLEEP) return true; // already not in deep sleep
  // preserve mute state
  uint8_t new_value = (this->is_muted_) ? (CTRL_PLAY + TAS5805M_MUTE_CONTROL) : CTRL_PLAY;
  if (!this->tas5805m_write_byte(TAS5805M_DEVICE_CTRL_2, new_value)) return false;

  this->tas5805m_state_.control_state = CTRL_PLAY;                        // set Control State to play
  ESP_LOGD(TAG, "  Tas5805m Deep Sleep Off");
  if (this->is_muted_) ESP_LOGD(TAG, "  Tas5805m Mute On preserved");
  return true;
}

bool Tas5805mComponent::get_state(ControlState* state) {
  *state = this->tas5805m_state_.control_state;
  return true;
}

bool Tas5805mComponent::set_state(ControlState state) {
  if (this->tas5805m_state_.control_state == state) return true;
  if (!this->tas5805m_write_byte(TAS5805M_DEVICE_CTRL_2, state)) return false;
  this->tas5805m_state_.control_state = state;
  ESP_LOGD(TAG, "  Tas5805m state set to: 0x%02X", state);
  return true;
}

bool Tas5805mComponent::get_digital_volume(uint8_t* raw_volume) {
  uint8_t current = 254; // lowest raw volume
  if(!this->tas5805m_read_byte(TAS5805M_DIG_VOL_CTRL, &current)) return false;
  *raw_volume = current;
  return true;
}

// controls both left and right channel digital volume
// digital volume is 24 dB to -103 dB in -0.5 dB step
// 00000000: +24.0 dB
// 00000001: +23.5 dB
// 00101111: +0.5 dB
// 00110000: 0.0 dB
// 00110001: -0.5 dB
// 11111110: -103 dB
// 11111111: Mute
bool Tas5805mComponent::set_digital_volume(uint8_t raw_volume) {
  if (!this->tas5805m_write_byte(TAS5805M_DIG_VOL_CTRL, raw_volume)) return false;
  return true;
}

// Analog Gain Control , with 0.5dB one step
// lower 5 bits controls the analog gain.
// 00000: 0 dB (29.5V peak voltage)
// 00001: -0.5db
// 11111: -15.5 dB
// set analog gain in dB
bool Tas5805mComponent::set_analog_gain(float gain_db) {
  if ((gain_db < TAS5805M_MIN_ANALOG_GAIN) || (gain_db > TAS5805M_MAX_ANALOG_GAIN)) return false;

  uint8_t new_again = static_cast<uint8_t>(-gain_db * 2.0);

  uint8_t current_again;
  if (!this->tas5805m_read_byte(TAS5805M_AGAIN, &current_again)) return false;

  // keep top 3 reserved bits combine with bottom 5 analog gain bits
  new_again = (current_again & 0xE0) | new_again;
  if (!this->tas5805m_write_byte(TAS5805M_AGAIN, new_again)) return false;

  ESP_LOGD(TAG, "  Tas5805m Analog Gain: %fdB (0x%02X)", gain_db, new_again);
  return true;
}

bool Tas5805mComponent::get_analog_gain(uint8_t* raw_gain) {
  uint8_t current;
  if (!this->tas5805m_read_byte(TAS5805M_AGAIN, &current)) return false;
  // remove top 3 reserved bits
  *raw_gain = current & 0x1F;
  return true;
}

bool Tas5805mComponent::get_dac_mode(DacMode* mode) {
    uint8_t current_value;
    if (!this->tas5805m_read_byte(TAS5805M_DEVICE_CTRL_1, &current_value)) return false;
    if (current_value & (1 << 2)) {
        *mode = PBTL;
    } else {
        *mode = BTL;
    }
    this->tas5805m_state_.dac_mode = *mode;
    return true;
}

bool Tas5805mComponent::set_dac_mode(DacMode mode) {
    uint8_t current_value;
    if (!this->tas5805m_read_byte(TAS5805M_DEVICE_CTRL_1, &current_value)) return false;

    // Update bit 2 based on the mode
    if (mode == PBTL) {
        current_value |= (1 << 2);  // Set bit 2 to 1 (PBTL mode)
    } else {
        current_value &= ~(1 << 2); // Clear bit 2 to 0 (BTL mode)
    }
    if (!this->tas5805m_write_byte(TAS5805M_DEVICE_CTRL_1, current_value)) return false;

    this->tas5805m_state_.dac_mode = mode;
    ESP_LOGD(TAG, "Tas5805m DAC mode set to: %s", this->tas5805m_state_.dac_mode ? "PBTL" : "BTL");
    return true;
}

bool Tas5805mComponent::get_mixer_mode(MixerMode *mode) {
  *mode = this->tas5805m_state_.mixer_mode;
  return true;
}

bool Tas5805mComponent::set_mixer_mode(MixerMode mode) {
  uint32_t mixer_l_to_l, mixer_r_to_r, mixer_l_to_r, mixer_r_to_l;

  switch (mode) {
    case STEREO:
      mixer_l_to_l = TAS5805M_MIXER_VALUE_0DB;
      mixer_r_to_r = TAS5805M_MIXER_VALUE_MUTE;
      mixer_l_to_r = TAS5805M_MIXER_VALUE_MUTE;
      mixer_r_to_l = TAS5805M_MIXER_VALUE_0DB;
      break;

    case STEREO_INVERSE:
      mixer_l_to_l = TAS5805M_MIXER_VALUE_MUTE;
      mixer_r_to_r = TAS5805M_MIXER_VALUE_0DB;
      mixer_l_to_r = TAS5805M_MIXER_VALUE_0DB;
      mixer_r_to_l = TAS5805M_MIXER_VALUE_MUTE;
      break;

    case MONO:
      mixer_l_to_l = TAS5805M_MIXER_VALUE_MINUS6DB;
      mixer_r_to_r = TAS5805M_MIXER_VALUE_MINUS6DB;
      mixer_l_to_r = TAS5805M_MIXER_VALUE_MINUS6DB;
      mixer_r_to_l = TAS5805M_MIXER_VALUE_MINUS6DB;
      break;

    case LEFT:
      mixer_l_to_l = TAS5805M_MIXER_VALUE_0DB;
      mixer_r_to_r = TAS5805M_MIXER_VALUE_MUTE;
      mixer_l_to_r = TAS5805M_MIXER_VALUE_0DB;
      mixer_r_to_l = TAS5805M_MIXER_VALUE_MUTE;
      break;

    case RIGHT:
      mixer_l_to_l = TAS5805M_MIXER_VALUE_MUTE;
      mixer_r_to_r = TAS5805M_MIXER_VALUE_0DB;
      mixer_l_to_r = TAS5805M_MIXER_VALUE_MUTE;
      mixer_r_to_l = TAS5805M_MIXER_VALUE_0DB;
      break;

    default:
      ESP_LOGD(TAG, "Attempt to set invalid Mixer Mode");
      return false;
  }

  if(!this->set_book_and_page(TAS5805M_REG_BOOK_5, TAS5805M_REG_BOOK_5_MIXER_PAGE)) {
    ESP_LOGE(TAG, "Error writing book and page to start setting Mixer Mode");
    return false;
  }
  ESP_LOGE(TAG, "writing Left to Left Mixer Gain");
  if (!this->tas5805m_write_bytes(TAS5805M_REG_LEFT_TO_LEFT_GAIN, reinterpret_cast<uint8_t *>(&mixer_l_to_l), 4)) {
    ESP_LOGE(TAG, "Error writing Left to Left Mixer Gain");
    return false;
  }
  ESP_LOGE(TAG, "writing Right to Right Mixer Gain");
  if (!this->tas5805m_write_bytes(TAS5805M_REG_RIGHT_TO_RIGHT_GAIN, reinterpret_cast<uint8_t *>(&mixer_r_to_r), 4)) {
    ESP_LOGE(TAG, "Error writing Right to Right Mixer Gain");
    return false;
  }
  ESP_LOGE(TAG, "writing Left to Right Mixer Gain");
  if (!this->tas5805m_write_bytes(TAS5805M_REG_LEFT_TO_RIGHT_GAIN, reinterpret_cast<uint8_t *>(&mixer_l_to_r), 4)) {
    ESP_LOGE(TAG, "Error writing Left to Right Mixer Gain");
    return false;
  }
  ESP_LOGE(TAG, "writing Right to Left Mixer Gain");
  if (!this->tas5805m_write_bytes(TAS5805M_REG_RIGHT_TO_LEFT_GAIN, reinterpret_cast<uint8_t *>(&mixer_r_to_l), 4)) {
    ESP_LOGE(TAG, "Error writing Right to Left Mixer Gain");
    return false;
  }

  if (!this->set_book_and_page(TAS5805M_REG_BOOK_CONTROL_PORT, TAS5805M_REG_PAGE_ZERO)) {
    ESP_LOGE(TAG, "Error writing book and page to complete setting Mixer Mode");
    return false;
  }

  this->tas5805m_state_.mixer_mode = mode;
  ESP_LOGD(TAG, "Tas5805m Mixer mode set to: %s", MIXER_MODE_TEXT[this->tas5805m_state_.mixer_mode]);
  return true;
}

#ifdef USE_NUMBER
bool Tas5805mComponent::get_eq(bool* enabled) {
  uint8_t current_value;
  if (!this->tas5805m_read_byte(TAS5805M_DSP_MISC, &current_value)) return false;
  *enabled = !(current_value & 0x01);
  this->tas5805m_state_.eq_enabled = *enabled;
  return true;
}

bool Tas5805mComponent::set_eq_on() {
  if (this->tas5805m_state_.eq_enabled) return true;
  if (!this->tas5805m_write_byte(TAS5805M_DSP_MISC, TAS5805M_CTRL_EQ_ON)) return false;
  this->tas5805m_state_.eq_enabled = true;
  ESP_LOGD(TAG, "  Tas5805m EQ control On");
  return true;
}

bool Tas5805mComponent::set_eq_off() {
  if (!this->tas5805m_state_.eq_enabled) return true;
  if (!this->tas5805m_write_byte(TAS5805M_DSP_MISC, TAS5805M_CTRL_EQ_OFF)) return false;
  this->tas5805m_state_.eq_enabled = false;
  ESP_LOGD(TAG, "  Tas5805m EQ control Off");
  return true;
}

bool Tas5805mComponent::set_eq(bool enable) {
  if (!this->tas5805m_write_byte(TAS5805M_DSP_MISC, enable ? TAS5805M_CTRL_EQ_ON : TAS5805M_CTRL_EQ_OFF)) return false;
  this->tas5805m_state_.eq_enabled = enable;
  ESP_LOGD(TAG, "  Tas5805m Set EQ control %s", enable ? "On" : "Off");
  return true;
}

bool Tas5805mComponent::set_eq_gain(uint8_t band, int8_t gain) {
  if (band < 0 || band >= TAS5805M_EQ_BANDS) {
    ESP_LOGE(TAG, "Invalid EQ Band: %d", band);
    return false;
  }
  if (gain < TAS5805M_EQ_MIN_DB || gain > TAS5805M_EQ_MAX_DB) {
    ESP_LOGE(TAG, "Invalid EQ Gain: %d - no change to EQ Band: %d", gain, band);
    return false;
  }

  if (!this->eq_gains_refresh_initiated_) {
    this->tas5805m_state_.eq_gain[band] = gain;
    ESP_LOGD(TAG, "EQ Band: %d updated Gain: %ddB for later setup", band, gain);
    return true;
  }

  uint8_t current_page = 0;
  bool ok = true;
  ESP_LOGD(TAG, "EQ Band %d (%dHz) set to Gain %ddB", band, tas5805m_eq_bands[band], gain);

  int x = gain + TAS5805M_EQ_MAX_DB;
  uint16_t y = band * TAS5805M_EQ_KOEF_PER_BAND * TAS5805M_EQ_REG_PER_KOEF;

  for (int16_t i = 0; i < TAS5805M_EQ_KOEF_PER_BAND * TAS5805M_EQ_REG_PER_KOEF; i++) {
      const RegisterSequenceEq* reg_value = &tas5805m_eq_registers[x][y + i];
      if (reg_value == NULL) {
          ESP_LOGW(TAG, "NULL pointer encountered at row[%d]", y + i);
          continue;
      }

      if (reg_value->page != current_page) {
          current_page = reg_value->page;
          if(!this->set_book_and_page(TAS5805M_REG_BOOK_EQ, reg_value->page)) {
            ESP_LOGE(TAG, "  Error writing EQ Gain for EQ Band %d (%d Hz)", band, tas5805m_eq_bands[band]);
            return false;
          }
      }

      ESP_LOGV(TAG, "Writing EQ Band gain: %d at 0x%02X with 0x%02X", i, reg_value->offset, reg_value->value);
      ok = tas5805m_write_byte(reg_value->offset, reg_value->value);
      if (!ok) {
        ESP_LOGE(TAG, "Error writing EQ Gain to register 0x%02X", reg_value->offset);
      }
  }

  this->tas5805m_state_.eq_gain[band] = gain;
  return this->set_book_and_page(TAS5805M_REG_BOOK_CONTROL_PORT, TAS5805M_REG_PAGE_ZERO);
}

void Tas5805mComponent::refresh_eq_gains() {
  // trigger refresh of EQ gains in 'loop'
  this->running_refresh_eq_gains_ = true;

  // refresh has been initiated so changes to eq gains can written now
  this->eq_gains_refresh_initiated_ = true;
  ESP_LOGE(TAG, "Refresh gains activated with EQ %s", this->tas5805m_state_.eq_enabled ? "Enabled" : "Disabled");
}

int8_t Tas5805mComponent::eq_gain(uint8_t band) {
  if (band < 0 || band >= TAS5805M_EQ_BANDS) {
    ESP_LOGE(TAG, "Invalid EQ Band: %d", band);
    return TAS5805M_EQ_MIN_DB;
  }
  return this->tas5805m_state_.eq_gain[band];
}
#endif

// bool Tas5805mComponent::get_modulation_mode(Tas5805mModMode *mode, Tas5805mSwFreq *freq, Tas5805mBdFreq *bd_freq) {
//   uint8_t device_ctrl1_value;
//   if (!this->tas5805m_read_byte(TAS5805M_DEVICE_CTRL_1, &device_ctrl1_value)) return false;
//   // Read the BD frequency
//   uint8_t ana_ctrl_value;
//   if (!this->tas5805m_read_byte(TAS5805M_ANA_CTRL, &ana_ctrl_value)) return false;

//   // Extract DAMP_MOD bits 0-1
//   *mode = static_cast<Tas5805mModMode>(device_ctrl1_value & 0x02);
//   // Extract FSW_SEL bits 4-6
//   *freq = static_cast<Tas5805mSwFreq>(device_ctrl1_value & 0x70);

//    // Extract ANA_CTRL bits 5-6 (class D bandwidth control)
//   *bd_freq = static_cast<Tas5805mBdFreq>(ana_ctrl_value & 0x60);
//   return true;
// }

// bool Tas5805mComponent::get_fs_freq(Tas5805mFsFreq* freq) {
//   uint8_t current_value;
//   if (!this->tas5805m_read_byte(TAS5805M_FS_MON, &current_value)) return false;
//   *freq = static_cast<Tas5805mFsFreq>(current_value);
//   return true;
// }

// bool Tas5805mComponent::get_bck_ratio(uint8_t *ratio) {
//   uint8_t current_value;
//   if (!this->tas5805m_read_byte(TAS5805M_BCK_MON, &current_value)) return false;
//   *ratio = current_value;
//   return true;
// }

// bool Tas5805mComponent::get_power_state(ControlState* state) {
//   uint8_t current_value;
//   if (!this->tas5805m_read_byte(TAS5805M_POWER_STATE, &current_value)) return false;
//   *state = static_cast<ControlState>(current_value);
//   return true;
// }

#ifdef USE_NUMBER
bool Tas5805mComponent::set_book_and_page(uint8_t book, uint8_t page) {
  if (!this->tas5805m_write_byte(TAS5805M_REG_PAGE_SET, TAS5805M_REG_PAGE_ZERO)) {
    ESP_LOGE(TAG, "  Error writing page_zero");
    return false;
  }
  if (!this->tas5805m_write_byte(TAS5805M_REG_BOOK_SET, book)) {
    ESP_LOGE(TAG, "  Error writing book: 0x%02X", book);
    return false;
  }
  if (!this->tas5805m_write_byte(TAS5805M_REG_PAGE_SET, page)) {
    ESP_LOGE(TAG, "  Error writing page: 0x%02X", page);
    return false;
  }
  return true;
}
#endif

bool Tas5805mComponent::tas5805m_read_byte(uint8_t a_register, uint8_t* data) {
  i2c::ErrorCode error_code;
  error_code = this->write(&a_register, 1);
  if (error_code != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "  Read register - first write error %i", error_code);
    this->i2c_error_ = (uint8_t)error_code;
    return false;
  }
  error_code = this->read_register(a_register, data, 1, true);
  if (error_code != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "  Read register error %i", error_code);
    this->i2c_error_ = (uint8_t)error_code;
    return false;
  }
  return true;
}

bool Tas5805mComponent::tas5805m_write_byte(uint8_t a_register, uint8_t data) {
    i2c::ErrorCode error_code = this->write_register(a_register, &data, 1, true);
    if (error_code != i2c::ERROR_OK) {
      ESP_LOGE(TAG, "  Write register error %i", error_code);
      this->i2c_error_ = (uint8_t)error_code;
      return false;
    }
    return true;
}

bool Tas5805mComponent::tas5805m_write_bytes(uint8_t a_register, uint8_t *data, uint8_t len) {
  // for (uint8_t i=0; i < len; i++) {
  //   this->tas5805m_write_byte(a_register, *(data+i));
  //   ESP_LOGE(TAG, "  Write value: 0x%x", *(data+i));
  // }

  i2c::ErrorCode error_code = this->write_register(a_register, data, len, true);
  if (error_code != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "  Write register error %i", error_code);
    this->i2c_error_ = (uint8_t)error_code;
    return false;
  }
  return true;
}

}  // namespace tas5805m
}  // namespace esphome

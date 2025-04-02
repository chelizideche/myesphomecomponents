#include "tas5805m.h"
#include "tas5805m_minimal.h"
#include "tas5805m_eq.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace tas5805m {

static const char *const TAG = "tas5805m";

static const uint8_t MUTE_CONTROL           = 0x08;  // LR Channel Mute

static const uint8_t ESPHOME_MAXIMUM_DELAY  = 5;     // Milliseconds

void Tas5805mComponent::setup() {
  if (this->enable_pin_ != nullptr) {
    // Set enable pin as OUTPUT and disable the enable pin
    this->enable_pin_->setup();
    this->enable_pin_->digital_write(false);
    delay(10);
    this->enable_pin_->digital_write(true);
  }

  this->set_timeout(100, [this]() {
      if (!configure_registers()) {
        this->error_code_ = CONFIGURATION_FAILED;
        this->mark_failed();
      }
  });
}

bool Tas5805mComponent::configure_registers() {
  uint16_t i = 0;
  uint16_t counter = 0;
  uint16_t number_configurations = sizeof(tas5805m_registers) / sizeof(tas5805m_registers[0]);

  while (i < number_configurations) {
    switch (tas5805m_registers[i].offset) {
      case CFG_META_DELAY:
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

  if (!this->set_state(CTRL_PLAY)) return false;
  this->set_volume(0.05);

  if (!this->get_gain(&this->analog_gain_)) return false;
  return true;
}

void Tas5805mComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Tas5805m:");
  uint8_t volume;

  switch (this->error_code_) {
    case CONFIGURATION_FAILED:
      ESP_LOGE(TAG, "  Write configuration failure with error code = %i",this->i2c_error_);
      break;
    case WRITE_REGISTER_FAILED:
      ESP_LOGE(TAG, "  Write register failure with error code = %i",this->i2c_error_);
      break;
      case NONE:
      ESP_LOGD(TAG, "  Registers configured: %i", this->number_registers_configured_);
      ESP_LOGD(TAG, "  Digital Volume: %i", this->digital_volume_);
      ESP_LOGD(TAG, "  Analog Gain: %i", this->analog_gain_);
      ESP_LOGD(TAG, "  Setup successful");
      LOG_I2C_DEVICE(this);
      break;
  }
}

bool Tas5805mComponent::set_volume(float volume) {
  float new_volume = clamp<float>(volume, 0.0, 1.0);
  uint8_t raw_volume = remap<uint8_t, float>(new_volume, 0.0f, 1.0f, 254, 0);
  if (!this->set_digital_volume(raw_volume)) return false;
  this->volume_ = new_volume;
  ESP_LOGD(TAG, "  Volume changed to: %2.0f%%", new_volume*100);
  return true;
}

bool Tas5805mComponent::set_mute_off() {
  if (!this->is_muted_) return true;
  if (!this->tas5805m_write_byte(DEVICE_CTRL_2_REGISTER, this->tas5805m_state_.state)) return false;
  this->is_muted_ = false;
  ESP_LOGD(TAG, "  Tas5805m Mute Off");
  return true;
}

// set bit 3 MUTE in DEVICE_CTRL_2_REGISTER and retain current Control State
// ensures get_state = get_power_state
bool Tas5805mComponent::set_mute_on() {
  if (this->is_muted_) return true;
  if (!this->tas5805m_write_byte(DEVICE_CTRL_2_REGISTER, this->tas5805m_state_.state + MUTE_CONTROL)) return false;
  this->is_muted_ = true;
  ESP_LOGD(TAG, "  Tas5805m Mute On");
  return true;
}

bool Tas5805mComponent::set_deep_sleep_on() {
  if (this->tas5805m_state_.state == CTRL_DEEP_SLEEP) return true; // already in deep sleep

  // preserve mute state
  uint8_t new_value = (this->is_muted_) ? (CTRL_DEEP_SLEEP + MUTE_CONTROL) : CTRL_DEEP_SLEEP;
  if (!this->tas5805m_write_byte(DEVICE_CTRL_2_REGISTER, new_value)) return false;

  this->tas5805m_state_.state = CTRL_DEEP_SLEEP;                   // set Control State to deep sleep
  ESP_LOGD(TAG, "  Tas5805m Deep Sleep On");
  if (this->is_muted_) ESP_LOGD(TAG, "  Tas5805m Mute On preserved");
  return true;
}

bool Tas5805mComponent::set_deep_sleep_off() {
  if (this->tas5805m_state_.state != CTRL_DEEP_SLEEP) return true; // already not in deep sleep
  // preserve mute state
  uint8_t new_value = (this->is_muted_) ? (CTRL_PLAY + MUTE_CONTROL) : CTRL_PLAY;
  if (!this->tas5805m_write_byte(DEVICE_CTRL_2_REGISTER, new_value)) return false;

  this->tas5805m_state_.state = CTRL_PLAY;                        // set Control State to play
  ESP_LOGD(TAG, "  Tas5805m Deep Sleep Off");
  if (this->is_muted_) ESP_LOGD(TAG, "  Tas5805m Mute On preserved");
  return true;
}

bool Tas5805mComponent::get_state(Tas5805mControlState* state) {
  *state = this->tas5805m_state_.state;
  return true;
}

bool Tas5805mComponent::set_state(Tas5805mControlState state) {
  if (this->tas5805m_state_.state == state) return true;
  if (!this->tas5805m_write_byte(DEVICE_CTRL_2_REGISTER, state)) return false;
  this->tas5805m_state_.state = state;
  ESP_LOGD(TAG, "  Tas5805m state set to: 0x%02X", state);
  return true;
}

bool Tas5805mComponent::get_digital_volume(uint8_t* raw_volume) {
  uint8_t current;
  if(!this->tas5805m_read_byte(DIG_VOL_CTRL_REGISTER, &current)) return false;
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
bool Tas5805mComponent::set_digital_volume(uint8_t new_volume) {
  if (!this->tas5805m_write_byte(DIG_VOL_CTRL_REGISTER, new_volume)) return false;
  this->digital_volume_ = new_volume;
  ESP_LOGD(TAG, "  Tas5805m Digital Volume: %i", new_volume);
  return true;
}

bool Tas5805mComponent::get_gain(uint8_t* raw_gain) {
  uint8_t current;
  if (!this->tas5805m_read_byte(AGAIN_REGISTER, &current)) return false;
  // remove top 3 reserved bits
  *raw_gain = current & 0x1F;
  return true;
}

// Analog Gain Control , with 0.5dB one step
// lower 5 bits controls the analog gain.
// 00000: 0 dB (29.5V peak voltage)
// 00001: -0.5db
// 11111: -15.5 dB
bool Tas5805mComponent::set_gain(uint8_t new_gain) {
  if (new_gain > 0x1F) return false;
  uint8_t raw_gain;
  if (!this->get_gain(&raw_gain)) return false;
  // keep top 3 reserved bits combine with bottom 5 analog gain bits
  raw_gain = (raw_gain & 0xE0) | new_gain;
  if (!this->tas5805m_write_byte(AGAIN_REGISTER, raw_gain)) return false;
  this->analog_gain_ = new_gain;
  ESP_LOGD(TAG, "  Tas5805m Analog Gain: %i", new_gain);
  return true;
}

bool Tas5805mComponent::get_dac_mode(Tas5805mDacMode* mode) {
    uint8_t current_value;
    if (!this->tas5805m_read_byte(DEVICE_CTRL_1_REGISTER, &current_value)) return false;
    if (current_value & (1 << 2)) {
        *mode = DAC_MODE_PBTL;
    } else {
        *mode = DAC_MODE_BTL;
    }
    return true;
}

bool Tas5805mComponent::get_eq(bool* enabled) {
  uint8_t current_value;
  if (!this->tas5805m_read_byte(DSP_MISC_REGISTER, &current_value)) return false;
  *enabled = !(current_value & 0x01);
  return true;
}

bool Tas5805mComponent::tas5805m_set_eq(bool enable) {
  ESP_LOGD(TAG, "Setting EQ to %d", enable);
  return this->tas5805m_write_byte(DSP_MISC_REGISTER, enable ? TAS5805M_CTRL_EQ_ON : TAS5805M_CTRL_EQ_OFF);
}

bool Tas5805mComponent:tas5805m_set_eq_gain(uint8_t band, int8_t gain) {
  if (band < 0 || band >= TAS5805M_EQ_BANDS) {
    ESP_LOGE(TAG, "No change to EQ Band: invalid band %d", band);
    return false;
  }

  if (gain < TAS5805M_EQ_MIN_DB || gain > TAS5805M_EQ_MAX_DB) {
    ESP_LOGE(TAG, "No change to EQ Band %d Gain: invalid gain %d", band, gain);
    return false;
  }

  uint8_t current_page = 0;
  bool ok = true;
  ESP_LOGD(TAG, "%s: Setting EQ band %d (%d Hz) to gain %d",band, tas5805m_eq_bands[band], gain);

  int x = gain + TAS5805M_EQ_MAX_DB;
  uint16_t y = band * TAS5805M_EQ_KOEF_PER_BAND * TAS5805M_EQ_REG_PER_KOEF;

  for (int16_t i = 0; i < TAS5805M_EQ_KOEF_PER_BAND * TAS5805M_EQ_REG_PER_KOEF; i++) {
      const reg_sequence_eq *reg_value = &tas5805m_eq_registers[x][y + i];
      if (reg_value == NULL) {
          ESP_LOGW(TAG, "NULL pointer encountered at row[%d]", y + i);
          continue;
      }

      if (reg_value->page != current_page) {
          current_page = reg_value->page;
          this->tas5805m_write_byte(TAS5805M_REG_PAGE_SET, TAS5805M_REG_PAGE_ZERO);
          this->tas5805m_write_byte(TAS5805M_REG_BOOK_SET, TAS5805M_REG_BOOK_EQ);
          this->tas5805m_write_byte(TAS5805M_REG_PAGE_SET, reg_value->page);
      }

      ESP_LOGV(TAG, "write: %d: w 0x%X 0x%X", i, reg_value->offset, reg_value->value);
      ok = tas5805m_write_byte(reg_value->offset, reg_value->value);
      if (!ok) {
          ESP_LOGE(TAG, "Error writing to register 0x%x", reg_value->offset);
      }
  }

  this->tas5805m_state_.eq_gain[band] = gain;
  this->tas5805m_write_byte(TAS5805M_REG_PAGE_SET, TAS5805M_REG_PAGE_ZERO);
  this->tas5805m_write_byte(TAS5805M_REG_BOOK_SET, TAS5805M_REG_BOOK_CONTROL_PORT);
  this->tas5805m_write_byte(TAS5805M_REG_PAGE_SET, TAS5805M_REG_PAGE_ZERO);

  return true;
}

bool Tas5805mComponent::get_modulation_mode(Tas5805mModMode *mode, Tas5805mSwFreq *freq, Tas5805mBdFreq *bd_freq) {
  uint8_t device_ctrl1_value;
  if (!this->tas5805m_read_byte(DEVICE_CTRL_1_REGISTER, &device_ctrl1_value)) return false;
  // Read the BD frequency
  uint8_t ana_ctrl_value;
  if (!this->tas5805m_read_byte(ANA_CTRL_REGISTER, &ana_ctrl_value)) return false;

  // Extract DAMP_MOD bits 0-1
  *mode = static_cast<Tas5805mModMode>(device_ctrl1_value & 0x02);
  // Extract FSW_SEL bits 4-6
  *freq = static_cast<Tas5805mSwFreq>(device_ctrl1_value & 0x70);

   // Extract ANA_CTRL bits 5-6 (class D bandwidth control)
  *bd_freq = static_cast<Tas5805mBdFreq>(ana_ctrl_value & 0x60);
  return true;
}

bool Tas5805mComponent::get_fs_freq(Tas5805mFsFreq* freq) {
  uint8_t current_value;
  if (!this->tas5805m_read_byte(FS_MON_REGISTER, &current_value)) return false;
  *freq = static_cast<Tas5805mFsFreq>(current_value);
  return true;
}

bool Tas5805mComponent::get_bck_ratio(uint8_t *ratio) {
  uint8_t current_value;
  if (!this->tas5805m_read_byte(BCK_MON_REGISTER, &current_value)) return false;
  *ratio = current_value;
  return true;
}

bool Tas5805mComponent::get_power_state(Tas5805mControlState* state) {
  uint8_t current_value;
  if (!this->tas5805m_read_byte(POWER_STATE_REGISTER, &current_value)) return false;
  *state = static_cast<Tas5805mControlState>(current_value);
  return true;
}

bool Tas5805mComponent::tas5805m_read_byte(uint8_t a_register, uint8_t* data) {
  i2c::ErrorCode error_code;
  error_code = this->write(&a_register, 1);
  if (error_code != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "  read register - first write error %i", error_code);
    this->i2c_error_ = (uint8_t)error_code;
    return false;
  }
  error_code = this->read_register(a_register, data, 1, true);
  if (error_code != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "  read register error %i", error_code);
    this->i2c_error_ = (uint8_t)error_code;
    return false;
  }
  return true;
}

bool Tas5805mComponent::tas5805m_write_byte(uint8_t a_register, uint8_t data) {
    i2c::ErrorCode error_code = this->write_register(a_register, &data, 1, true);
    if (error_code != i2c::ERROR_OK) {
      ESP_LOGE(TAG, "  write register error %i", error_code);
      this->i2c_error_ = (uint8_t)error_code;
      return false;
    }
    return true;
}

bool Tas5805mComponent::tas5805m_write_bytes(uint8_t a_register, uint8_t *data, uint8_t len) {
  i2c::ErrorCode error_code = this->write_register(a_register, data, len, true);
  if (error_code != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "  write register error %i", error_code);
    this->i2c_error_ = (uint8_t)error_code;
    return false;
  }
  return true;
}

}  // namespace tas5805m
}  // namespace esphome

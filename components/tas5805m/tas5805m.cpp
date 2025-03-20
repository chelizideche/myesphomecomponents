#include "tas5805m.h"
#include "tas5805m_minimal.h"
#include "tas5805m_eq.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace tas5805m {

static const char *const TAG = "tas5805m";

static const uint8_t CTRL_STATE_DEEP_SLEEP   = 0x00;
static const uint8_t CTRL_STATE_SLEEP        = 0x01;
static const uint8_t CTRL_STATE_HI_Z         = 0x02;
static const uint8_t CTRL_STATE_PLAY         = 0x03;
static const uint8_t LR_CHANNEL_MUTE         = 0x08; // Left-Right Channel Mute

// tas5805m registers
static const uint8_t DEVICE_CTRL_2_REGISTER = 0x03; // Device state control register
static const uint8_t DIG_VOL_CTRL_REGISTER  = 0x4C;
static const uint8_t AGAIN_REGISTER         = 0x54;

static const uint8_t REG_BOOK_SET           = 0x7F;
static const uint8_t REG_PAGE_SET           = 0x00;
static const uint8_t REG_PAGE_ZERO          = 0x00;

static const uint8_t REG_BOOK_CONTROL_PORT  = 0x00;
static const uint8_t REG_BOOK_5             = 0x8C;
static const uint8_t REG_BOOK_5_VOLUME_PAGE = 0x2A;
static const uint8_t REG_LEFT_VOLUME        = 0x24;
static const uint8_t REG_RIGHT_VOLUME       = 0x28;

static const uint8_t ESPHOME_MAXIMUM_DELAY  = 5;    // Milliseconds

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
  if (!this->get_gain(&this->analog_gain_)) return false;
  this->set_volume(0.01);
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
      ESP_LOGD(TAG, "  Analog Gain: %i", this->analog_gain_);
      ESP_LOGD(TAG, "  Volume: %3.2f", this->volume_);
      ESP_LOGD(TAG, "  LR Raw Volume: %i", this->digital_volume_);
      ESP_LOGD(TAG, "  Setup successful");
      LOG_I2C_DEVICE(this);
      break;
  }
}

bool Tas5805mComponent::set_volume(float volume) {
  float new_volume = clamp<float>(volume, 0.0, 1.0);
  uint8_t raw_volume = remap<uint8_t, float>(new_volume, 0.0f, 1.0f, 0, 158);
  if (!this->set_digital_volume(raw_volume)) return false;
  this->volume_ = new_volume;
  ESP_LOGD(TAG, "  Volume changed to: %2.0f%%", new_volume*100);
  return true;
}

bool Tas5805mComponent::set_mute_off() {
  if (!this->is_muted_) return true;
  if (!this->tas5805m_write_byte(DEVICE_CTRL_2_REGISTER, CTRL_STATE_PLAY)) return false;
  this->is_muted_ = false;
  ESP_LOGD(TAG, "  Tas5805m Mute Off");
  return true;
}

bool Tas5805mComponent::set_mute_on() {
  if (this->is_muted_) return true;
  if (!this->tas5805m_write_byte(DEVICE_CTRL_2_REGISTER, LR_CHANNEL_MUTE)) return false;
  this->is_muted_ = true;
  ESP_LOGD(TAG, "  Tas5805m Mute On");
  return true;
}

bool Tas5805mComponent::set_deep_sleep_on() {
  if (this->deep_sleep_mode_) return true; // already in deep sleep
  this->deep_sleep_mode_ = this->tas5805m_write_byte(DEVICE_CTRL_2_REGISTER, CTRL_STATE_DEEP_SLEEP);
  ESP_LOGD(TAG, "  Tas5805m Deep Sleep On");
  return this->deep_sleep_mode_;
}

bool Tas5805mComponent::set_deep_sleep_off() {
  if (!this->deep_sleep_mode_) return true; // already not in deep sleep
  this->deep_sleep_mode_ = (!this->tas5805m_write_byte(DEVICE_CTRL_2_REGISTER, CTRL_STATE_PLAY));
  ESP_LOGD(TAG, "  Tas5805m Deep Sleep Off");
  return this->deep_sleep_mode_;
}

bool Tas5805mComponent::get_digital_volume(uint8_t* raw_volume) {
  uint8_t current;
  if(!this->tas5805m_read_byte(DIG_VOL_CTRL_REGISTER, &current)) return false;
  *raw_volume = current;
  return true;
}

void Tas5805mComponent::decode_32bit(uint32_t v, uint8_t* a) {
  for (uint8_t i = 4; i > 0; i--) {
	  *(a + i - 1) = v & 0xFF;
	  v >>= 8;
  }
}

// controls both left and right channel digital volume
bool Tas5805mComponent::set_digital_volume(uint8_t new_volume) {
  if (new_volume > 158) {
    ESP_LOGE(TAG, "  set volume too high = %i", new_volume);
    return false;
  }

  //uint8_t* vp;

	//uint32_t x = tas5805m_volume[new_volume];
  //v[0] = uint8_t(x)
  //v[1] = uint8_t(x>>8);
  //v[2] = uint8_t(x>>16);

	// int i;
  // for (i = 0; i < 4; i++) {
	// 	v[3 - i] = x;
	// 	x >>= 8;
	// }

  uint8_t volume_settings[4];
  this->decode_32bit(tas5805m_volume[new_volume], volume_settings);
  if (!this->tas5805m_set_book_and_page(REG_BOOK_5, REG_BOOK_5_VOLUME_PAGE)) return false;
  //vp = (uint8_t*)&tas5805m_volume[new_volume];
  ESP_LOGE(TAG, "Volume byte 0x%02X%02X%02X%02X",volume_settings[0], volume_settings[1], volume_settings[2], volume_settings[3]);
  //if (!tas5805m_write_bytes(REG_LEFT_VOLUME , (uint8_t*)&tas5805m_volume[new_volume], 4)) {
  if (!this->tas5805m_write_bytes(REG_LEFT_VOLUME , volume_settings, 4)) {
     ESP_LOGE(TAG, "  set left volume error ");
     return false;
   }
  //if (!tas5805m_write_bytes(REG_RIGHT_VOLUME , (uint8_t*)&tas5805m_volume[new_volume], 4)) {
  if (!this->tas5805m_write_bytes(REG_RIGHT_VOLUME, volume_settings, 4)) {
     ESP_LOGE(TAG, "  set right volume error ");
     return false;
  }
  if (!this->tas5805m_set_book_and_page(REG_BOOK_CONTROL_PORT, REG_PAGE_ZERO)) return false;
  this->digital_volume_ = new_volume;
  ESP_LOGD(TAG, "  Tas5805m LR Raw Volume: %i", new_volume);
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

bool Tas5805mComponent::tas5805m_set_book_and_page(uint8_t book, uint8_t page) {
    if (!this->tas5805m_write_byte(REG_PAGE_SET, REG_PAGE_ZERO)) {
      ESP_LOGE(TAG, "  write set book-page error 1");
      return false;
    }
    if (!this->tas5805m_write_byte(REG_BOOK_SET, book)) {
      ESP_LOGE(TAG, "  write set book-page error 2");
      return false;
    }
    if (!this->tas5805m_write_byte(REG_PAGE_SET, page)) {
      ESP_LOGE(TAG, "  write set book-page error 3");
      return false;
    }
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

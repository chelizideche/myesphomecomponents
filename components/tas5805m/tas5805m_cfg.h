#pragma once

namespace esphome {
namespace tas5805m {

  enum ControlState : uint8_t {
    CTRL_DEEP_SLEEP = 0x00, // Deep Sleep
    CTRL_SLEEP      = 0x01, // Sleep
    CTRL_HI_Z       = 0x02, // Hi-Z
    CTRL_PLAY       = 0x03, // Play
   };

  enum DacMode : uint8_t {
    BTL  = 0, // Bridge tied load
    PBTL = 1, // Parallel load
  };

  // enum SwFreq : uint8_t {
  //   SW_FREQ_768K = (0x00 << 4),
  //   SW_FREQ_384K = (0x01 << 4),
  //   SW_FREQ_480K = (0x03 << 4),
  //   SW_FREQ_576K = (0x04 << 4),
  // };

  // enum BdFreq : uint8_t {
  //   bd_FREQ_80K  = (0x00 << 5),
  //   bd_FREQ_100K = (0x01 << 5),
  //   bd_FREQ_120K = (0x02 << 5),
  //   bd_FREQ_175K = (0x03 << 5),
  // };

  // enum ModMode : uint8_t {
  //   MOD_MODE_BD     = 0x00,
  //   MOD_MODE_1SPW   = 0x01,
  //   MOD_MODE_HYBRID = 0x02,
  // };

  // enum MixerMode : uint8_t {
  //   MIXER_UNKNOWN = 0,
  //   MIXER_STEREO,
  //   MIXER_STEREO_INVERSE,
  //   MIXER_MONO,
  //   MIXER_RIGHT,
  //   MIXER_LEFT,
  // };

  // enum FsFreq : uint8_t {
  //   FS_FREQ_ERROR     = 0b0000,
  //   FS_FREQ_8K        = 0b0010,
  //   FS_FREQ_16K       = 0b0100,
  //   FS_FREQ_32K       = 0b0110,
  //   FS_FREQ_48K       = 0b1001,
  //   FS_FREQ_96K       = 0b1011,
  //   FS_FREQ_RESERVED  = 0b1000,
  // };

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

// Startup sequence codes
static const uint8_t TAS5805M_CFG_META_DELAY         = 254;

static const float TAS5805M_MIN_ANALOG_GAIN         = -15.5;
static const float TAS5805M_MAX_ANALOG_GAIN         = 0.0;

// set book and page registers
static const uint8_t TAS5805M_REG_PAGE_SET           = 0x00;
static const uint8_t TAS5805M_REG_BOOK_SET           = 0x7F;
static const uint8_t TAS5805M_REG_BOOK_CONTROL_PORT  = 0x00;
static const uint8_t TAS5805M_REG_PAGE_ZERO          = 0x00;

// tas5805m registers
static const uint8_t TAS5805M_DEVICE_CTRL_1 = 0x02;
static const uint8_t TAS5805M_DEVICE_CTRL_2 = 0x03;
static const uint8_t TAS5805M_FS_MON        = 0x37;
static const uint8_t TAS5805M_BCK_MON       = 0x38;
static const uint8_t TAS5805M_DIG_VOL_CTRL  = 0x4C;
static const uint8_t TAS5805M_ANA_CTRL      = 0x53;
static const uint8_t TAS5805M_AGAIN         = 0x54;
static const uint8_t TAS5805M_DSP_MISC      = 0x66;
static const uint8_t TAS5805M_POWER_STATE   = 0x68;

// TAS5805M_REG_FAULT register values

// EQ registers
static const uint8_t  TAS5805M_CTRL_EQ_ON        = 0x00;
static const uint8_t  TAS5805M_CTRL_EQ_OFF       = 0x01;

// Level meter register

// Mixer registers

} // namespace tas5805m
} // namespace esphome
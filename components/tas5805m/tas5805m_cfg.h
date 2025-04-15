#pragma once

#include "tas5805m.h"

namespace esphome {
namespace tas5805m {


// Startup sequence codes
static const uint8_t TAS5805M_CFG_META_DELAY         = 254;

static const float TAS5805M_MIN_ANALOG_GAIN         = -15.0;
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
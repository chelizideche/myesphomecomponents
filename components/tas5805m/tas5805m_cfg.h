#pragma once

#include "tas5805m.h"

namespace esphome {
namespace tas5805m {


// Startup sequence codes
static const uint8_t CFG_META_DELAY = 254;

// set book and page registers
static const uint8_t TAS5805M_REG_PAGE_SET           = 0x00;
static const uint8_t TAS5805M_REG_BOOK_SET           = 0x7F;
static const uint8_t TAS5805M_REG_BOOK_CONTROL_PORT  = 0x00;
static const uint8_t TAS5805M_REG_PAGE_ZERO          = 0x00;

// tas5805m registers
static const uint8_t DEVICE_CTRL_1_REGISTER = 0x02;
static const uint8_t DEVICE_CTRL_2_REGISTER = 0x03;
static const uint8_t FS_MON_REGISTER        = 0x37;
static const uint8_t BCK_MON_REGISTER       = 0x38;
static const uint8_t DIG_VOL_CTRL_REGISTER  = 0x4C;
static const uint8_t ANA_CTRL_REGISTER      = 0x53;
static const uint8_t AGAIN_REGISTER         = 0x54;
static const uint8_t DSP_MISC_REGISTER      = 0x66;
static const uint8_t POWER_STATE_REGISTER   = 0x68;

// TAS5805M_REG_FAULT register values

// EQ registers
static const uint8_t  TAS5805M_CTRL_EQ_ON        = 0x00;
static const uint8_t  TAS5805M_CTRL_EQ_OFF       = 0x01;

// Level meter register

// Mixer registers

} // namespace tas5805m
} // namespace esphome
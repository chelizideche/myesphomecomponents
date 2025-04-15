import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.components.audio_dac import AudioDac

from esphome.const import (
    CONF_ID,
    CONF_ENABLE_PIN,
)

from esphome import pins

CODEOWNERS = ["@mrtoy-me"]
DEPENDENCIES = ["i2c"]

CONF_GAIN = "analog_gain"

tas5805m_ns = cg.esphome_ns.namespace("tas5805m")
Tas5805mComponent = tas5805m_ns.class_("Tas5805mComponent", AudioDac, cg.Component, i2c.I2CDevice)

ANALOG_GAINS = [-15.5, -15, -14.5, -14, -13.5, -13, -12.5, -12, -11.5, -11, -10.5, -10, -9.5, -9, -8.5, -8,
                 -7.5,  -7,  -6.5,  -6,  -5.5,  -5,  -4.5,  -4,  -3.5,  -3,  -2.5,  -2, -1.5, -1, -0.5,  0]

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(Tas5805mComponent),
            cv.Required(CONF_ENABLE_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_GAIN, default="-14.5dB"): cv.All(
                        cv.decibel, cv.one_of(*ANALOG_GAINS)
                    ),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(i2c.i2c_device_schema(0x2D))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
    enable = await cg.gpio_pin_expression(config[CONF_ENABLE_PIN])
    cg.add(var.set_enable_pin(enable))
    cg.add(var.config_analog_gain(config[CONF_GAIN]))

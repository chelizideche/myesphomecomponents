import esphome.codegen as cg
from esphome.components import number
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_SOUND_PRESSURE,
    ENTITY_CATEGORY_CONFIG,
    UNIT_DECIBEL,
)

CONF_GAIN_20HZ = "gain_20Hz"
CONF_GAIN_31_5HZ = "gain_31.5Hz"

ICON_VOLUME_SOURCE = "mdi:volume-source"

from .. import CONF_TAS5805M_ID, Tas5805mComponent, tas5805m_ns

EqGainNumber = tas5805m_ns.class_("EqGainNumber", number.Number)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_TAS5805M_ID): cv.use_id(Tas5805mComponent),
        cv.Required(CONF_GAIN_20HZ): number.number_schema(
            EqGainNumber,
            device_class=DEVICE_CLASS_SOUND_PRESSURE,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_VOLUME_SOURCE,
            unit_of_measurement=UNIT_DECIBEL,
        ),
        cv.Required(CONF_GAIN_31_5HZ): number.number_schema(
            EqGainNumber,
            device_class=DEVICE_CLASS_SOUND_PRESSURE,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_VOLUME_SOURCE,
            unit_of_measurement=UNIT_DECIBEL,
        ),
    }
)

async def to_code(config):
    Tas5805mComponent = await cg.get_variable(config[CONF_TAS5805M_ID])
    if band_config := config.get(CONF_GAIN_20HZ):
        n = await number.new_number(
            band_config, min_value=-15, max_value=15, step=1, restore_value=True, initial_value=0, optimistic=True
        )
        await cg.register_parented(n, config[CONF_TAS5805M_ID])
        cg.add(Tas5805mComponent.set_gain_20_hz(n))
    if band_config := config.get(CONF_GAIN_31_5HZ):
        n = await number.new_number(
            band_config, min_value=-15, max_value=15, step=1, restore_value=True, initial_value=0, optimistic=True
        )
        await cg.register_parented(n, config[CONF_TAS5805M_ID])
        cg.add(Tas5805mComponent.set_gain_31_5hz(n))

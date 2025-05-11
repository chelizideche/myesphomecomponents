import esphome.codegen as cg
from esphome.components import number
import esphome.config_validation as cv
from esphome.const import (
    DEVICE_CLASS_SOUND_PRESSURE,
    ENTITY_CATEGORY_CONFIG,
    UNIT_DECIBEL,
)

CONF_GAIN_20HZ = "gain_20Hz"

ICON_VOLUME_SOURCE = "mdi:volume-source"

from ..audio_dac import CONF_TAS5805M_ID, Tas5805mComponent, tas5805m_ns

EqGainNumber = tas5805m_ns.class_("EqGainNumber", number.Number)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_TAS5805M_ID): cv.use_id(Tas5805mComponent),
        cv.Optional(f"eq_gain"): (
            {
                cv.Required(CONF_GAIN_20HZ): number.number_schema(
                    EqGainNumber,
                    device_class=DEVICE_CLASS_SOUND_PRESSURE,
                    entity_category=ENTITY_CATEGORY_CONFIG,
                    icon=ICON_VOLUME_SOURCE,
                    unit_of_measurement=UNIT_DECIBEL,
                ),
            }
        ),
    }
)

async def to_code(config):
    tas5805m_component = await cg.get_variable(config[CONF_TAS5805M_ID])
    if gain_20Hz_config := config.get(CONF_GAIN_20HZ):
        n = await number.new_number(
            gain_20Hz_config, min_value=-15, max_value=15, step=1
        )
        await cg.register_parented(n, tas5805m_component)
        cg.add(tas5805m_component.set_gain_20_hz_number(n))

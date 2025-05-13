import esphome.codegen as cg
from esphome.components import number
import esphome.config_validation as cv
from esphome.const import (
    DEVICE_CLASS_SOUND_PRESSURE,
    ENTITY_CATEGORY_CONFIG,
    UNIT_DECIBEL,
)

CONF_GAIN_20HZ   = "eq_gain_band20Hz"
CONF_GAIN_31P5HZ = "eq_gain_band31.5Hz"
CONF_GAIN_50HZ   = "eq_gain_band50Hz"
CONF_GAIN_80HZ   = "eq_gain_band80Hz"

ICON_VOLUME_SOURCE = "mdi:volume-source"

from ..audio_dac import CONF_TAS5805M_ID, Tas5805mComponent, tas5805m_ns

EqGainBand20hz = tas5805m_ns.class_("EqGainBand20hz", number.Number, cg.Component)
EqGainBand31p5hz = tas5805m_ns.class_("EqGainBand31p5hz", number.Number, cg.Component)
EqGainBand50hz = tas5805m_ns.class_("EqGainBand50hz", number.Number, cg.Component)
EqGainBand80hz = tas5805m_ns.class_("EqGainBand80hz", number.Number, cg.Component)
EqGainBand125hz = tas5805m_ns.class_("EqGainBand1256hz", number.Number, cg.Component)
EqGainBand200hz = tas5805m_ns.class_("EqGainBand200hz", number.Number, cg.Component)
EqGainBand315hz = tas5805m_ns.class_("EqGainBand315hz", number.Number, cg.Component)
EqGainBand500hz = tas5805m_ns.class_("EqGainBand500hz", number.Number, cg.Component)
EqGainBand800hz = tas5805m_ns.class_("EqGainBand800hz", number.Number, cg.Component)
EqGainBand1250hz = tas5805m_ns.class_("EqGainBand1250hz", number.Number, cg.Component)
EqGainBand2000hz = tas5805m_ns.class_("EqGainBand2000hz", number.Number, cg.Component)
EqGainBand3150hz = tas5805m_ns.class_("EqGainBand3150hz", number.Number, cg.Component)
EqGainBand5000hz = tas5805m_ns.class_("EqGainBand5000hz", number.Number, cg.Component)
EqGainBand8000hz = tas5805m_ns.class_("EqGainBand8000hz", number.Number, cg.Component)
EqGainBand16000hz = tas5805m_ns.class_("EqGainBand16000hz", number.Number, cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_TAS5805M_ID): cv.use_id(Tas5805mComponent),

        cv.Required(CONF_GAIN_20HZ): number.number_schema(
            EqGainBand20hz,
            device_class=DEVICE_CLASS_SOUND_PRESSURE,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_VOLUME_SOURCE,
            unit_of_measurement=UNIT_DECIBEL,
        )
        .extend(cv.COMPONENT_SCHEMA),

        cv.Required(CONF_GAIN_31P5HZ): number.number_schema(
            EqGainBand31p5hz,
            device_class=DEVICE_CLASS_SOUND_PRESSURE,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_VOLUME_SOURCE,
            unit_of_measurement=UNIT_DECIBEL,
        )
        .extend(cv.COMPONENT_SCHEMA),

        cv.Required(CONF_GAIN_50HZ): number.number_schema(
            EqGainBand50hz,
            device_class=DEVICE_CLASS_SOUND_PRESSURE,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_VOLUME_SOURCE,
            unit_of_measurement=UNIT_DECIBEL,
        )
        .extend(cv.COMPONENT_SCHEMA),

        cv.Required(CONF_GAIN_80HZ): number.number_schema(
            EqGainBand80hz,
            device_class=DEVICE_CLASS_SOUND_PRESSURE,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_VOLUME_SOURCE,
            unit_of_measurement=UNIT_DECIBEL,
        )
        .extend(cv.COMPONENT_SCHEMA),
    }
)

async def to_code(config):
    tas5805m_component = await cg.get_variable(config[CONF_TAS5805M_ID])
    if gain_20hz_config := config.get(CONF_GAIN_20HZ):
        n = await number.new_number(
            gain_20hz_config, min_value=-15, max_value=15, step=1
        )
        await cg.register_component(n, gain_20hz_config)
        await cg.register_parented(n, tas5805m_component)

    if gain_31p5hz_config := config.get(CONF_GAIN_31P5HZ):
        n = await number.new_number(
            gain_31p5hz_config, min_value=-15, max_value=15, step=1
        )
        await cg.register_component(n, gain_31p5hz_config)
        await cg.register_parented(n, tas5805m_component)

    if gain_50hz_config := config.get(CONF_GAIN_50HZ):
        n = await number.new_number(
            gain_50hz_config, min_value=-15, max_value=15, step=1
        )
        await cg.register_component(n, gain_50hz_config)
        await cg.register_parented(n, tas5805m_component)

    if gain_80hz_config := config.get(CONF_GAIN_80HZ):
        n = await number.new_number(
            gain_80hz_config, min_value=-15, max_value=15, step=1
        )
        await cg.register_component(n, gain_80hz_config)
        await cg.register_parented(n, tas5805m_component)
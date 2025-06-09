import esphome.codegen as cg
from esphome.components import select
import esphome.config_validation as cv
from esphome.const import ENTITY_CATEGORY_CONFIG

from ..audio_dac import CONF_TAS5805M_ID, Tas5805mComponent, tas5805m_ns

MixerModeSelect = tas5805m_ns.class_("MixerModeSelect", select.Select)

CONF_MIXER_MODE = "mixer_mode"

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_TAS5805M_ID): cv.use_id(Tas5805mComponent),
    cv.Required(CONF_MIXER_MODE): select.select_schema(
        MixerModeSelect,
        entity_category=ENTITY_CATEGORY_CONFIG,
    ),
}

async def to_code(config):
    tas5805m_component = await cg.get_variable(config[CONF_TAS5805M_ID])
    mixer_mode_config = config.get(CONF_MIXER_MODE)
    s = await select.new_select(
            mixer_mode_config,
            options=["STEREO", "STEREO_INVERSE", "MONO", "RIGHT", "LEFT"],
    )
    await cg.register_parented(s, config[CONF_TAS5805M_ID])
    cg.add(tas5805m_component.set_mixer_mode_select(s))

import esphome.codegen as cg
from esphome.components import select
import esphome.config_validation as cv
from esphome.const import CONF_ID, ENTITY_CATEGORY_CONFIG

from ..audio_dac import CONF_TAS5805M_ID, Tas5805mComponent, tas5805m_ns

MixerPresetSelect = tas5805m_ns.class_("MixerPresetSelect", select.Select)

CONF_MIXER_MODE = "mixer_preset"

CONFIG_SCHEMA = (
    cv.Schema(
      {
            cv.GenerateID(CONF_TAS5805M_ID): cv.use_id(Tas5805mComponent),
            cv.Required(CONF_MIXER_MODE): select.select_schema(
                MixerPresetSelect,
                entity_category=ENTITY_CATEGORY_CONFIG,
            ),
      }
    )
)

async def to_code(config):
    tas5805m_component = await cg.get_variable(config[CONF_TAS5805M_ID])
    mixer_preset_config = config.get(CONF_MIXER_MODE)
    s = await select.new_select(
            mixer_preset_config,
            options=["STEREO", "STEREO_INVERSE", "MONO", "RIGHT", "LEFT"],
    )
    await cg.register_component(s, mixer_preset_config)
    await cg.register_parented(s, tas5805m_component)

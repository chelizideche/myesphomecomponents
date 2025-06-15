import esphome.codegen as cg
from esphome.components import select
import esphome.config_validation as cv
from esphome.const import CONF_ID, ENTITY_CATEGORY_CONFIG

from ..audio_dac import CONF_TAS5805M_ID, Tas5805mComponent, tas5805m_ns

MixerPresetSelect = tas5805m_ns.class_("MixerPresetSelect", select.Select, cg.Component)

CONF_MIXER_PRESET = "mixer_preset"

CONFIG_SCHEMA = (
    cv.Schema(
      {
            cv.GenerateID(): cv.declare_id(MixerPresetSelect),
            cv.GenerateID(CONF_TAS5805M_ID): cv.use_id(Tas5805mComponent),
            cv.Required(CONF_MIXER_PRESET): select.select_schema(
                MixerPresetSelect,
                entity_category=ENTITY_CATEGORY_CONFIG,
            )
            .extend(cv.COMPONENT_SCHEMA),
      }
    )
)
# CONFIG_SCHEMA = cv.All(
#     select.select_schema( MixerPresetSelect)
#     .extend(
#         {
#             cv.GenerateID(CONF_TAS5805M_ID): cv.use_id(Tas5805mComponent),
#         }
#     )
#     .extend(cv.COMPONENT_SCHEMA),
# )

async def to_code(config):
    tas5805m_component = await cg.get_variable(config[CONF_TAS5805M_ID])
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await cg.register_parented(var, tas5805m_component)
    await select.new_select(
            config,
            options=["STEREO", "STEREO_INVERSE", "MONO", "RIGHT", "LEFT"],
    )


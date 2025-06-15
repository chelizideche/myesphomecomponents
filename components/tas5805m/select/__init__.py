import esphome.codegen as cg
from esphome.components import select
import esphome.config_validation as cv
from esphome.const import ENTITY_CATEGORY_CONFIG
from ..audio_dac import CONF_TAS5805M_ID, Tas5805mComponent, tas5805m_ns
from esphome.core.entity_helpers import inherit_property_from

MixerModeSelect = tas5805m_ns.class_("MixerModeSelect", select.Select)
CONF_AUDIO_DAC = "audio_dac"
CONF_MIXER_MODE = "mixer_mode"
CONF_DAC_MODE = "dac_mode"
CONF_PBTL = "PBTL"

CONFIG_SCHEMA = (
    select.select_schema(MixerModeSelect,
             entity_category=ENTITY_CATEGORY_CONFIG)
    .extend(
        {
            cv.GenerateID(CONF_TAS5805M_ID): cv.use_id(Tas5805mComponent),
        }
    )
)
# CONFIG_SCHEMA = {
#     cv.GenerateID(CONF_TAS5805M_ID): cv.use_id(Tas5805mComponent),
#     cv.Required(CONF_MIXER_MODE): select.select_schema(
#         MixerModeSelect,
#         entity_category=ENTITY_CATEGORY_CONFIG,
#     ),
# }

FINAL_VALIDATE_SCHEMA = cv.All(
     inherit_property_from(CONF_DAC_MODE, CONF_TAS5805M_ID),
)

async def to_code(config):
    tas5805m_component = await cg.get_variable(config[CONF_TAS5805M_ID])

    # if config.get(CONF_DAC_MODE) == CONF_PBTL:
    #   raise cv.Invalid(
    #         f"DAC MODE set to PBTL ok - value is {config.get(CONF_DAC_MODE)}"
    #     )
    # if config.get(CONF_DAC_MODE) != CONF_PBTL:
    #   raise cv.Invalid(
    #         f"DAC MODE set to BTL ok - value is {config.get(CONF_DAC_MODE)}"
    #   )
    # raise cv.Invalid(
    #         f"Not caught - value is {config.get(CONF_DAC_MODE)} ok"
    #    )
    #mixer_mode_config = config.get(CONF_MIXER_MODE)
    # if config[CONF_DAC_MODE] == CONF_PBTL:
    #   raise cv.Invalid(
    #         f"DAC MODE set to PBTL ok - value is {config[CONF_DAC_MODE]}"
    #     )
    # else:
    #   raise cv.Invalid(
    #         f"DAC MODE set to BTL ok - value is {config[CONF_DAC_MODE]}"
    #   )
    # raise cv.Invalid(
    #         f"Not caught - value is {config.get(CONF_DAC_MODE)} ok"
    #    )
    # if config[CONF_DAC_MODE] == CONF_PBTL:
    #     s = await select.new_select(
    #             #mixer_mode_config,
    #             config,
    #             options=["MONO", "RIGHT", "LEFT"],
    #     )
    # else:

    s = await select.new_select(
            config,
            options=["STEREO", "STEREO_INVERSE", "MONO", "RIGHT", "LEFT"],
    )
    await cg.register_parented(s, config[CONF_TAS5805M_ID])
    cg.add(tas5805m_component.set_mixer_mode_select(s))

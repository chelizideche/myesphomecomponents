import esphome.codegen as cg
from esphome.components import select
import esphome.config_validation as cv
import esphome.final_validate as fv
from esphome.const import ENTITY_CATEGORY_CONFIG
from esphome.core import CORE
from ..audio_dac import CONF_TAS5805M_ID, Tas5805mComponent, tas5805m_ns

from esphome.const import CONF_ID

MixerModeSelect = tas5805m_ns.class_("MixerModeSelect", select.Select)

CONF_MIXER_MODE = "mixer_mode"
CONF_DAC_MODE = "dac_mode"
CONF_PBTL = "PBTL"

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_TAS5805M_ID): cv.use_id(Tas5805mComponent),
    cv.Required(CONF_MIXER_MODE): select.select_schema(
        MixerModeSelect,
        entity_category=ENTITY_CATEGORY_CONFIG,
    ),
}

def find_component_config(component_id, domain=None):
    """Helper function to find a component's configuration by ID"""
    full_config = CORE.config

    if domain:
        # Search in specific domain
        domain_config = full_config.get(domain, [])
        if not isinstance(domain_config, list):
            domain_config = [domain_config]

        for item in domain_config:
            if isinstance(item, dict) and item.get(CONF_ID) == component_id:
                return item
    else:
        # Search all domains
        for domain_name, domain_config in full_config.items():
            if isinstance(domain_config, list):
                for item in domain_config:
                    if isinstance(item, dict) and item.get(CONF_ID) == component_id:
                        return item
            elif isinstance(domain_config, dict) and domain_config.get(CONF_ID) == component_id:
                return domain_config

    return None

async def to_code(config):
    tas5805m_component = await cg.get_variable(config[CONF_TAS5805M_ID])
    mixer_mode_config = config.get(CONF_MIXER_MODE)
    # Get ID for the output speaker and add it to the source speakrs config to easily inherit properties
     # You can also access the referenced component's config here
    # This is useful for generating different code based on the other component
    ref_config = find_component_config(tas5805m_component)
    dac_config = ref_config.get(CONF_DAC_MODE)
    if ref_config and dac_config and config[CONF_DAC_MODE] == CONF_PBTL:
        s = await select.new_select(
                mixer_mode_config,
                options=["MONO", "RIGHT", "LEFT"],
        )
    else:
        s = await select.new_select(
                mixer_mode_config,
                options=["STEREO", "STEREO_INVERSE", "MONO", "RIGHT", "LEFT"],
        )
    await cg.register_parented(s, config[CONF_TAS5805M_ID])
    cg.add(tas5805m_component.set_mixer_mode_select(s))

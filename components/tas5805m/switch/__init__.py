import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import DEVICE_CLASS_SWITCH

from ..audio_dac import CONF_TAS5805M_ID, Tas5805mComponent, tas5805m_ns

DEPENDENCIES = ["tas5805m"]

EnableSwitch = tas5805m_ns.class_("EnableSwitch", switch.Switch)

CONFIG_SCHEMA = switch.switch_schema(
    EnableSwitch,
    device_class=DEVICE_CLASS_SWITCH,
).extend(
    cv.Schema(
        {
            cv.GenerateID(CONF_TAS5805M_ID): cv.use_id(Tas5805mComponent),
        }
    )
)

async def to_code(config):
    tas5805m_component = await cg.get_variable(config[CONF_TAS5805M_ID])
    s = await switch.new_switch(config)
    await cg.register_parented(s, config[CONF_TAS5805M_ID])
    cg.add(tas5805m_component.set_enable_switch(s))

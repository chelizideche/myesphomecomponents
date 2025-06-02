import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_DISTANCE,
    STATE_CLASS_MEASUREMENT,
)

CONF_FAULT_RECOVERY_COUNT = "fault_recovery_count"
CONF_LAST_CHANNEL_FAULT = "last_channel_fault"
CONF_LAST_GLOBAL_FAULT1 = "last_global_fault1"
CONF_LAST_GLOBAL_FAULT2 = "last_global_fault2"

from ..audio_dac import CONF_TAS5805M_ID, TAS5805M_COMPONENT_SCHEMA, tas5805m_ns
#from ..audio_dac import CONF_TAS5805M_ID, Tas5805mComponent

TAS5805MSensor = tas5805m_ns.class_("Tas5805mSensor", cg.PollingComponent)

CONFIG_SCHEMA = TAS5805M_COMPONENT_SCHEMA.extend(
#CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(TAS5805MSensor),
        #cv.GenerateID(CONF_TAS5805M_ID): cv.use_id(Tas5805mComponent),

        cv.Required(CONF_FAULT_RECOVERY_COUNT): sensor.sensor_schema(
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_DISTANCE,
                state_class=STATE_CLASS_MEASUREMENT,
        ),

        # cv.Optional(CONF_LAST_CHANNEL_FAULT): sensor.sensor_schema(
        #         TAS5805MSensor,
        #         accuracy_decimals=0,
        #         device_class=DEVICE_CLASS_DISTANCE,
        #         state_class=STATE_CLASS_MEASUREMENT,
        # ),

        # cv.Optional(CONF_LAST_GLOBAL_FAULT1): sensor.sensor_schema(
        #         TAS5805MSensor,
        #         accuracy_decimals=0,
        #         device_class=DEVICE_CLASS_DISTANCE,
        #         state_class=STATE_CLASS_MEASUREMENT,
        # ),

        # cv.Optional(CONF_LAST_GLOBAL_FAULT2): sensor.sensor_schema(
        #         TAS5805MSensor,
        #         accuracy_decimals=0,
        #         device_class=DEVICE_CLASS_DISTANCE,
        #         state_class=STATE_CLASS_MEASUREMENT,
        # ),
    }
).extend(cv.polling_component_schema("60s"))


async def to_code(config):
    tas5805m_component = await cg.get_variable(config[CONF_TAS5805M_ID])
    var = cg.new_Pvariable(config[CONF_ID], tas5805m_component)
    await cg.register_component(var,config)

    recovery_count_config = config.get(CONF_FAULT_RECOVERY_COUNT)
    sens = await sensor.new_sensor(recovery_count_config)
    cg.add(var.set_recovery_count_sensor(sens))

#     if last_channel_fault_config := config.get(CONF_LAST_CHANNEL_FAULT):
#       sens = await sensor.new_sensor(last_channel_fault_config)
#       cg.add(var.set_last_channel_fault_sensor(sens))

#     if last_global_fault1_config := config.get(CONF_LAST_GLOBAL_FAULT1):
#       sens = await sensor.new_sensor(last_global_fault1_config)
#       cg.add(var.set_last_global_fault1_sensor(sens))

#     if last_global_fault2_config := config.get(CONF_LAST_GLOBAL_FAULT2):
#       sens = await sensor.new_sensor(last_global_fault2_config)
#       cg.add(var.set_last_global_fault2_sensor(sens))

#      tas5805m_parent = await cg.get_variable(config[CONF_TAS5805M_ID])

#      recovery_count_config = config.get(CONF_FAULT_RECOVERY_COUNT)
#      sens = await sensor.new_sensor(recovery_count_config, tas5805m_parent)
#      cg.add(sens.set_recovery_count_sensor(sens))

#      if last_channel_fault_config := config.get(CONF_LAST_CHANNEL_FAULT):
#         sens = await sensor.new_sensor(last_channel_fault_config, tas5805m_parent)
#         cg.add(sens.set_last_channel_fault_sensor(sens))

#      if last_global_fault1_config := config.get(CONF_LAST_GLOBAL_FAULT1):
#         sens = await sensor.new_sensor(last_global_fault1_config, tas5805m_parent)
#         cg.add(sens.set_last_global_fault1_sensor(sens))

#      if last_global_fault2_config := config.get(CONF_LAST_GLOBAL_FAULT2):
#         sens = await sensor.new_sensor(last_global_fault2_config, tas5805m_parent)
#         cg.add(sens.set_last_global_fault2_sensor(sens))



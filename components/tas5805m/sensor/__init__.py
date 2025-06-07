import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_DISTANCE,
    STATE_CLASS_MEASUREMENT,
)

CONF_CLEAR_FAULTS = "clear_faults"
CONF_LAST_CHANNEL_FAULT = "last_channel_fault"
CONF_LAST_GLOBAL_FAULT = "last_global_fault"

from ..audio_dac import CONF_TAS5805M_ID, Tas5805mComponent, tas5805m_ns

Tas5805mSensor = tas5805m_ns.class_("Tas5805mSensor", cg.PollingComponent)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(Tas5805mSensor),
            cv.GenerateID(CONF_TAS5805M_ID): cv.use_id(Tas5805mComponent),

            cv.Required(CONF_CLEAR_FAULTS): sensor.sensor_schema(
                    accuracy_decimals=0,
                    device_class=DEVICE_CLASS_DISTANCE,
                    state_class=STATE_CLASS_MEASUREMENT,
            ),

            cv.Optional(CONF_LAST_CHANNEL_FAULT): sensor.sensor_schema(
                    accuracy_decimals=0,
                    device_class=DEVICE_CLASS_DISTANCE,
                    state_class=STATE_CLASS_MEASUREMENT,
            ),

            cv.Optional(CONF_LAST_GLOBAL_FAULT): sensor.sensor_schema(
                    accuracy_decimals=0,
                    device_class=DEVICE_CLASS_DISTANCE,
                    state_class=STATE_CLASS_MEASUREMENT,
            ),
        }
    ).extend(cv.polling_component_schema("60s"))
)

async def to_code(config):
    tas5805m_component = await cg.get_variable(config[CONF_TAS5805M_ID])
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await cg.register_parented(var, tas5805m_component)

    if clear_faults_config := config.get(CONF_CLEAR_FAULTS):
      sens = await sensor.new_sensor(clear_faults_config)
      cg.add(var.set_clear_faults_sensor(sens))

    if last_channel_fault_config := config.get(CONF_LAST_CHANNEL_FAULT):
      sens = await sensor.new_sensor(last_channel_fault_config)
      cg.add(var.set_last_channel_fault_sensor(sens))

    if last_global_fault_config := config.get(CONF_LAST_GLOBAL_FAULT):
      sens = await sensor.new_sensor(last_global_fault_config)
      cg.add(var.set_last_global_fault_sensor(sens))

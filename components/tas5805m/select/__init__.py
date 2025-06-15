import esphome.codegen as cg
from esphome.components import select
import esphome.config_validation as cv
from esphome.const import CONF_ID, ENTITY_CATEGORY_CONFIG
from ..audio_dac import CONF_TAS5805M_ID, Tas5805mComponent, tas5805m_ns, DAC_MODES
import esphome.final_validate as fv
from esphome.core.entity_helpers import inherit_property_from

MixerModeSelect = tas5805m_ns.class_("MixerModeSelect", select.Select)
CONF_AUDIO_DAC = "audio_dac"
CONF_MIXER_MODE = "mixer_mode"
CONF_DAC_MODE = "dac_mode"
CONF_PBTL = "PBTL"

# CONFIG_SCHEMA = (
#     sensor.sensor_schema(IntegrationSensor)
#     .extend(
#         {
#             cv.Required(CONF_SENSOR): cv.use_id(sensor.Sensor),
#             cv.Required(CONF_TIME_UNIT): cv.enum(INTEGRATION_TIMES, lower=True),
#             cv.Optional(CONF_INTEGRATION_METHOD, default="trapezoid"): cv.enum(
#                 INTEGRATION_METHODS, lower=True
#             ),
#             cv.Optional(CONF_RESTORE, default=False): cv.boolean,
#             cv.Optional("min_save_interval"): cv.invalid(
#                 "min_save_interval was removed in 2022.8.0. Please use the `preferences` -> `flash_write_interval` to adjust."
#             ),
#         }
#     )
#     .extend(cv.COMPONENT_SCHEMA)
# )


# FINAL_VALIDATE_SCHEMA = cv.All(
#     cv.Schema(
#         {
#             cv.Required(CONF_ID): cv.use_id(IntegrationSensor),
#             cv.Optional(CONF_ICON): cv.icon,
#             cv.Optional(CONF_UNIT_OF_MEASUREMENT): sensor.validate_unit_of_measurement,
#             cv.Optional(CONF_ACCURACY_DECIMALS): sensor.validate_accuracy_decimals,
#             cv.Required(CONF_SENSOR): cv.use_id(sensor.Sensor),
#         },
#         extra=cv.ALLOW_EXTRA,
#     ),
#     inherit_property_from(CONF_ICON, CONF_SENSOR),
#     inherit_property_from(
#         CONF_UNIT_OF_MEASUREMENT, CONF_SENSOR, transform=inherit_unit_of_measurement
#     ),
#     inherit_property_from(
#         CONF_ACCURACY_DECIMALS, CONF_SENSOR, transform=inherit_accuracy_decimals
#     ),
# )
CONFIG_SCHEMA = cv.All(
   cv.Schema(
     {
        #cv.GenerateID(CONF_TAS5805M_ID): cv.use_id(Tas5805mComponent),
        cv.Required(CONF_MIXER_MODE): select.select_schema(
          MixerModeSelect,
          entity_category=ENTITY_CATEGORY_CONFIG,
        ),
     },
    ),
    #inherit_property_from(CONF_DAC_MODE, CONF_TAS5805M_ID),
)
FINAL_VALIDATE_SCHEMA = cv.All(
     cv.Schema(
        {
           cv.GenerateID(CONF_TAS5805M_ID): cv.use_id(Tas5805mComponent),
        }
     ),
     inherit_property_from(CONF_DAC_MODE, CONF_TAS5805M_ID),
)

async def to_code(config):
    tas5805m_component = await cg.get_variable(config[CONF_TAS5805M_ID])
    #inherit_property_from(CONF_DAC_MODE, CONF_TAS5805M_ID)config

    if config.get(CONF_DAC_MODE) == "PBTL":
      raise cv.Invalid(
            f"DAC MODE set to PBTL ok"
        )
    if config.get(CONF_DAC_MODE) == "BTL":
      raise cv.Invalid(
            f"DAC MODE set to BTL ok"
      )
    raise cv.Invalid(
            f"DAC MODE set to {config.get(CONF_DAC_MODE)} ok"
        )
    mixer_mode_config = config.get(CONF_MIXER_MODE)

    if config.get(CONF_DAC_MODE) == CONF_PBTL:
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

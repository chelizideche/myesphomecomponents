
## ESPHome component for ESP32-Louder (using ESP32-S3 with PSRAM and TAS5805m DAC)

This ESPHome external component is based on the TAS5805M DAC driver for ESP32 platform<BR>
https://github.com/sonocotta/esp32-tas5805m-dac/tree/main by Andriy Malyshenko<BR>
which is licenced under GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007.


# Usage: Tas5805m component
This component requires Esphome version 2025.6.0 or later.

The following yaml can be used so ESPHome accesses the component files:
```
external_components:
  - source: github://mrtoy-me/esphome-components-test@dev
    components: [ tas5805m ]
    refresh: 0s
```

# Overview
The base component configuration uses the esphome Audio DAC Core component,<BR>
so it is configured as a platform under the core audio dac component.<BR><BR>
The component works in conjuction with the ***speaker:*** ***-platform: i2s_audio***,<BR>
***- platform: mixer*** , ***-platform: remixer*** and ***mediaplayer: -platform: speaker***.<BR>
These components require ***psram:*** so the example YAML includes psram definitions.<BR>
The ***mediaplayer: -platform: speaker*** component operates exclusively under IDF,<BR>
so under ***esp32:*** ***-framework:*** must have ***type: esp-idf***.<BR>
The component also requires definition of ***i2c:*** to define the ***sda:*** and ***scl:*** pins.<BR>
used to communicate with the ESP32 Louder's TAS5805M.<BR>

The component monitors for TAS5805M faults at the update interval of the ***audio_dac: -platform: tas5805m***<BR>
If any fault is detected, the faults will be automatically cleared at this next update interval.<BR>

Several additional settings and sensors can be configured:
- two template Switch definitions - Enable Louder and Enable EQ Control
- a template Number for adjusting Announce Volume for example for Text to Speech annnouncement
- optionally define 15 EQ Gain Numbers (all required or not configured) to control the TAS5805M EQ
- 12 optional Binary Sensors corresonding to TAS5805M fault codes
- an optional Sensor indicating the number of times the faults register have been cleared on detecting a fault

# YAML configuration

## Audio Dac
```
audio_dac:
  - platform: tas5805m
    enable_pin: GPIOxx
    analog_gain: -14.5db
    dac_mode: BTL
    mixer_mode: STEREO
    update_interval: 10s
```
***enable_pin:*** is required and is the enable pin of ESP32 Louder<BR>
***analog_gain:*** is optional, if not specified defaults to -15.5db<BR>
The analog gain must be a number between -15.5db and 0db in 0.5db increments.<BR>
If 5v is used to power the ESP32 Louder board then the default setting of -15.5db is recommended.<BR>
***dac_mode:*** is optional with valid values of BTL or PBTL, if not specified defaults to BTL<BR>
***mixer_mode:*** is optional with valid values of STEREO, INVERSE_STEREO, LEFT, RIGHT<BR>
If not specified defaults to STEREO.<BR>
***update_interval:*** defined the interval faults will be checked and then if detected will be<BR>
cleared at next interval. If not specified, defaults to 60 seconds.<BR>

## Template Switches
The example YAML has template switch definitions provide switches in Homeassistant to:
 - Enable Louder Switch, more specifically places TAS5805M out of/into low power sleep mode
 - Enable TAS5805M EQ Control Switch works in conjunction with 15 EQ Gain Numbers

The example YAML includes ***interval:*** configuration to trigger Enable Louder Switch Off
when there is no music player activity (idle or paused) for the defined time (120s) and
then triggers Enable Louder Switch On when music player activity is detected.

The example YAML enable/disable TAS5805M EQ Controls, has option set ***restore_mode: RESTORE_DEFAULT_ON***<BR>
The last setting of the Enable TAS5805M EQ Controls is restoired and if it cannot be restored, enables EQ Controls.

## Announce Volume Template Number
The example YAML defines a template number used in conjuction with<BR>
***speaker:*** and ***media_player:*** YAML configurations<BR>
allowing the announcement_pipeline audio volume to be adjusted separate to the media_pipeline volume.

## EQ Gain Configuration Numbers
Configuration of the 15 EQ Gain Bands is shown in the example YAML.<BR>
Defining a ***number: -platform: tas5805m*** requires all 15 EQ Gain Bands to be configured.

For TAS5805M EQ control to configure correctly requires some addition YAML configuration as follows:
- defining in the media_player configuration a "startup" media file
```
files:
      id: startup
      file: https://github.com/esphome/home-assistant-voice-pe/raw/dev/sounds/wake_word_triggered.flac
```

- defining a ***on_boot:*** action under ***esphome:***
```
on_boot:
    priority: 220.0
    then:
      - lambda: id(louder_mediaplayer)->play_file(id(startup), true, false);
      - lambda: id(tas5805m_dac).refresh_eq_gains();
```
## Binary Sensors
These binary sensors correspond to fault codes from the TAS5805M.<BR>
All 12 binary sensors can be optionally defined as follows:
```
binary_sensor:
  - platform: tas5805m
    have_fault:
      name: Any Faults
    left_channel_dc_fault:
      name: Left Channel DC Fault
    right_channel_dc_fault:
      name: Right Channel DC Fault
    left_channel_over_current:
      name: Left Channel Over Current
    right_channel_over_current:
      name: Right Channel Over Current
    otp_crc_check:
      name: CRC Check Fault
    bq_write_failed:
      name: BQ Write Failure
    clock fault:
      name: I2S Clock Fault
    pcdd_over_voltage:
      name: PCDD Over Voltage
    pcdd_under_voltage:
      name: PCDD Under Voltage
    over_temp_shutdown:
      name: Over Temperature Shutdown Fault
    over_temp_warning:
      name: Over Temperature Warning
```
The ***have_fault*** binary sensor combines all other binary sensors, effectively,<BR>
if any of the fault conditions exist, then the ***have_fault*** binary sensor is true.<BR>
Binary sensors are updated at the ***update_interval:*** of the ***audio_dac: -platform: tas5805m***<BR>

## Sensor
One sensor can be optionally defined as follows:
```
sensor:
  - platform: tas5805m
    faults_cleared:
      name: "Times Faults Cleared"
```
This sensor counts the number of times that faults has been detected and then automatically cleared.
It is updated at the update interval of this sensor which defaults to 60 seconds.

# YAML examples in this Repository
The ESP32-S3 Louder configuration is provided in "esp32S3_louder_idf_media.yaml".<BR>

The ESP32 Louder configuration is provided in "esp32_louder_idf_media.yaml".<BR>
Due to psram memory limitation of the ESP32, there may be some initial distortion for first few seconds of playing a audio stream.
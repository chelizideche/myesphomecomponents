
## ESPHome component for ESP32-Louder (using ESP32-S3 with PSRAM and TAS5805m DAC)

This ESPHome external component is based on the TAS5805M DAC driver for ESP32 platform<BR>
https://github.com/sonocotta/esp32-tas5805m-dac/tree/main by Andriy Malyshenko<BR>
which is licenced under GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007.


## Usage: Tas5805m component
This component requires Esphome version 2025.2.0 or later.

The following yaml can be used so ESPHome accesses the component files:
```
external_components:
  - source: github://mrtoy-me/esphome-components-test@dev
    components: [ tas5805m ]
    refresh: 0s
```

## Overview
The base component configuration uses the esphome Audio DAC Core component,<BR>
so it is configured as a platform under the core audio dac component.
The component works in conjuction with the ***speaker:*** ***-platform: i2s_audio***,
***- platform: mixer*** , ***-platform: remixer*** and ***mediaplayer: -platform: speaker***.
These component require ***psram:*** so the example YAML includes these definitions.
The ***mediaplayer: -platform: speaker*** component operates exclusively under IDF, that is
under ***esp32:***, ***framework:*** must be ***type: esp-idf***.
The component also requires definition of ***i2c:***, to define the ***sda:*** and ***scl:*** pins
used to communicate with the ESP32 Louder's TAS5805M.

The component monitors for TAS5805M faults at the update interval of the audio_dac TAS5805M platform<BR>
If any fault is detected, the faults will be cleared at this next update interval.

Several additional settings and sensors can be configured:
- the suggested yaml includes two template switch definitions - Enable Louder and Enable EQ Contro
- template Number for adjusting Announce Volume for example for Text to Speech annnouncement
- optionally define 15 EQ Gain Numbers (all required or not configured) to control the TAS5805M EQ
- 12 optional Binary Sensors corresonding to TAS5805M fault codes
- an optional Sensor indicating the number of times the faults register have been cleared on detecting a fault

## YAML configuration

# Audio Dac
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
***update_interval:*** faults will be checked at this interval and<BR>
cleared at next interval if any are detected. If not specified, defaults to 60 seconds.<BR>

# Template Switch
```
switch:
  - platform: template
    name: Enable
    id: enable_louder
    optimistic: true
    restore_mode: ALWAYS_ON
    turn_on_action:
      lambda: id(tas5805m_dac).set_deep_sleep_off();
    turn_off_action:
      lambda: id(tas5805m_dac).set_deep_sleep_on();

  - platform: template
    name: Enable EQ Control
    id: enable_eq_control
    optimistic: true
    restore_mode: RESTORE_DEFAULT_ON
    turn_on_action:
      - lambda: id(tas5805m_dac).set_eq_on();
    turn_off_action:
      - lambda: id(tas5805m_dac).set_eq_off();
```
These template switch definitions provide switches in Homeassistant to:
 - enable/disable Louder, more specifically put TAS5805M out of/into low power sleep mode
 - enable/disable TAS5805M EQ Controls works in conjunction with 15 EQ Gain Numbers

The example YAML includes ***interval:*** configuration to trigger Louder Enable Switch Off
when there is no music player activity (idle or paused) for the defined time (120s) and
then trigger Louder Enable Switch On when music player is detected.

# Template Number
```
number:
  - platform: template
    name: Announce Volume
    id: announce_volume
    icon: mdi:volume-source
    unit_of_measurement: "%"
    min_value: 0
    max_value: 100
    step: 1
    restore_value: true
    initial_value: 50
    optimistic: true
```
This template number in conjuction with the example ***speaker:*** and ***media_player:*** YAML
allows the announcement_pipeline audio volume to be adjusted separate to the media_pipeline volume.

# EQ Gain Configuration Numbers

# YAML examples in this Repository
Esp-idf framework can be used for defining the mediaplayer.

The "esp32S3_louder_idf_media.yaml" provides and example configuration.<BR>
This configurationuses more use of psram and has two audio pipelines - media and announce.<BR>
Volume control and mute/unmute are undertaken by the tas5805m dac. This example reduces the volume of the media pipeline<BR>
if the announce pipeline plays and also adjusts the announce pipeline volume, then reverts back when the announce audio pipeline stops.<BR>
The announce pipeline volume used can be altered through a number template which can be adjusted in Homeassistant.<BR>
EQ control is setup through Enable Eq Control switch and several number templates.<BR>

To control via Homeassistant, quite a few cards need to be defined.<BR>
The example dashboard view configuration "HA_dashboard_view_setup.yaml" can be used to setup a dashboard.<BR>
The following setup assumes you have used "esp32S3_louder_idf_media.yaml" configuration<BR>
and Homeassistant version 2025.5.0 is used so entity naming is the same as in "HA_dashboard_view_setup.yaml".<BR>

Undertake the following steps....<BR>
Create a new dashboard by going to "Settings" then "Dashboards", add a new Dashboard and give it a Title.<BR>
A single view called Home will be created on this new dashboard. Go to edit mode for that dashboard by pressing the top right Pencil icon,<BR>
then using the top right 3 dots icon menu, select Raw configuration editor. Copy and paste the contents of "HA_dashboard_view_setup.yaml"<BR>
over the second line "- title: Home" and press save, then cross out and press Done.<BR>

Now you should have a dashboard view that looks like this:
![alt text](dashboard.png)


## ESPHome component for ESP32-Louder (using ESP32-S3 with PSRAM and TAS5805m DAC)

This ESPHome external component is based on the TAS5805M DAC driver for ESP32 platform<BR>
https://github.com/sonocotta/esp32-tas5805m-dac/tree/main by Andriy Malyshenko<BR>
which is licenced under GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007.


## Usage: Tas5805m component
This component requires Esphome version 2025.2.0 or later.

The following yaml can be used so ESPHome accesses the component files:
```
external_components:
  - source: github://mrtoy-me/esphome-components-test@main
    components: [ tas5805m ]
    refresh: 0s
```

# Example configuration entry
The component configuration uses the esphome Audio DAC Core component,
so it is configured as a platform under the core audio dac component.

```
audio_dac:
  - platform: tas5805m
    enable_pin: GPIOxx
```
***enable_pin:*** is required and is the enable pin of ESP32-Louder<BR>

# YAML examples in this Repository
Esp-idf framework can be used for defining the mediaplayer.

The "esp32S3_louder_idf_media.yaml" example configures more use of psram and has two audio pipelines - media and announce.
Volume control and mute/unmute are undertaken by the tas5805m dac. This example reduces the volume of the media pipeline
if the announce pipeline plays and also adjusts the announce pipeline volume, then reverts back when the announce audio pipeline stops.
The announce pipeline volume used can be altered through a number template which can be adjusted in Homeassistant.
EQ control is setup through Enable Eq Control switch and several number templates.

To control via Homeassistant, quite a few cards need to be defined. The example view config "HA_dashboard_view_setup.yaml" can be used as follows:
Create a new dashboard by going to "Setting then "Dashboards" and add a new Dashboard and give it a Title.
A single view called Home will be created on this new dashboard. Go to edit mode for that dashboard by pressing the top right Pencil,
then using the top right 3 dots menu, select Raw configuration editor. Copy and paste the contents of "HA_dashboard_view_setup.yaml"
over the second line "- title: Home" and press save, then cross out and press Done.

Now you should have a dashboard view that looks like this:

![alt text](dashboard.png)

#!/usr/bin/env bash
#
# Temporary helper script for unbinding drivers as usbtest seems to prevent
# dfu-util flashing. But we should rather modify hdmi2usb-mode-switch utility.

for dev in $(hdmi2usb-mode-switch --get-sysfs 2> /dev/null); do
    # unbind only the drivers under e.g. */1-2.4:1.0, not */1-2.4
    if [[ $dev == */*:* ]] && [ -d "$dev/driver" ]; then
        echo "Unbinding $dev..."
        echo $(basename $dev) | sudo tee "$dev/driver/unbind" > /dev/null
    fi
done

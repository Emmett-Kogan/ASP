# Assignment 7
## Contributor(s)
1. Emmett Kogan

## Build Instructions

    1. run `make` and uninstall, then reinstall the module
    2. Unplug and replug the keyboard
    3. remove hardware from virt-manager, and re-add it
    4. If this has worked, `ls -l /sys/bus/usb/devices` should show a few more devices, with "1-4:1.0" being listed
    5. run the script and spam dmesg



## Notes
1. https://github.com/torvalds/linux/blob/master/drivers/hid/usbhid/usbkbd.c#L190
In `usb_kbd_event` and `usb_kbd_led`, adding state I guess? Or a bit of code in event that checks for the key to toggle modes, and toggle modes, as well as checking for the led thing, and then toggling the LED as specified in the doc. Could probably just add a static variable inside one of the functions.

For switching only on numlock press, might need to check if it generates events at all when a key is pressed. If no event is generated, then who cares, but if one is and the LED state is the same then I need to look for a way to figure out why the event was generated to see if there is a way to check if caps lock or if num lock was pressed

For the idea of switching from MODE2 to MODE1 when num lock is pressed, this is impossible because, when in MODE2, NUML led is on. If the LED is on, and you press NUML (or any key related to any LED), then, the first interrupt (for the press) has a context of 0, so there is no way to distinguish someone pressing say caps lock or num lock, if both LEds are on. When the button is then released, THEN capslock would have an interrupt where the key code is 58, and numlock would have an interrupt of key code 69. Since they are the same on press, then we can't exit MODE2 until release, therefore instructions bad.

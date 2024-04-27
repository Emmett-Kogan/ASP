# Assignment 7b
## Contributor(s)
1. Emmett Kogan

## Build Instructions
Method 1 (manual):
1. Run `make` to build the driver
2. Install it with `sudo insmod usbkbd.ko` (you can uninstall old versions with `sudo rmmod usbkbd`
3. Now you need to figure out which usb device it is, see either the DDUSBkeyboard slides or Method 2 step 3
4. Now you need to unbind whatever driver it was associated with and bind it to the driver you just installed (using `echo -n <usb dev no>` to `/sys/bus/usb/drivers/usbhid/unbind` and `/sys/bus/usb/drivers/usbkbd/bind`)
5. The keyboard driver is now active, note that if the condition to switch to mode2 is already met, while it was initialized to mode1 it will switch to mode2 so... be aware and check with `sudo dmesg -<c or T>`

Method 2 (script):
1. Give `test.sh` permission to  execute: `chmod +x test.sh` 
2. Run `sudo ./test.sh` and follow the prompt for when to plug in/forward the keyboard with virt box
3. It's possible that the device id/number is different, in that case, use `ls -l /sys/bus/usb/devices` before and after enabling the device, and it will be "x-y:1.0" of whatever appeared

## Notes
[I modified this file](https://github.com/torvalds/linux/blob/master/drivers/hid/usbhid/usbkbd.c#L190)
[Here is a backup demo video](https://youtu.be/Fo7RicjQOOk)


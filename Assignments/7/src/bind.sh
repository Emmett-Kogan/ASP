sudo echo -n "1-4:1.0" > /sys/bus/usb/drivers/usbhid/unbind
sudo echo -n "1-4:1.0" > /sys/bus/usb/drivers/my_usbkbd/bind


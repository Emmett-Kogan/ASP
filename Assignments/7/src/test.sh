# resinstall driver
make
sudo rmmod usbkbd
sudo insmod usbkbd.ko

# wait for user input to signify when hardware was recycled in virt-manager
echo "Remove and add hardware in virt manager"
read temp
ls -l /sys/bus/usb/devices

# bind device
sudo echo -n "1-4:1.0" > /sys/bus/usb/drivers/usbhid/unbind
sudo echo -n "1-4:1.0" > /sys/bus/usb/drivers/usbkbd/bind

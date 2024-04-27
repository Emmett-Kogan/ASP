# resinstall driver
make
sudo rmmod usbkbd
sudo insmod usbkbd.ko

# wait for user input to signify when hardware was recycled in virt-manager
echo "Remove and add hardware in virt box"
read temp
sleep 3s

ls -l /sys/bus/usb/devices

# bind device
sudo echo -n "1-2:1.0" > /sys/bus/usb/drivers/usbhid/unbind
sudo echo -n "1-2:1.0" > /sys/bus/usb/drivers/usbkbd/bind

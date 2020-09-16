#!/bin/sh

macaddr=$(ifconfig wlan0 | grep ether | awk '{print $2}' | cut -d':' -f5-7 | sed 's/://g')
echo "Mac $macaddr"
sed -i 's/gt400_ap.*/gt400_ap_'$macaddr'/g' /home/pi/gt400/2g_ap.conf
sudo sed -i 's/gt400_ap.*/gt400_ap_'$macaddr'/g' /etc/create_ap.conf

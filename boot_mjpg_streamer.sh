#!/bin/sh
i2cset -y 1 0x70 0x00 0x01
gpio -g mode 17 out
gpio -g mode 4  out
gpio -g write 17 0 #set the gpio17 low
gpio -g write 4 0 #set the gpio4   low
echo "Choose camera A"

#./goertek/server &

cd mjpg-streamer-master/mjpg-streamer-experimental/
mjpg_streamer -i "./input_raspicam.so" -o "./output_http.so -w ./www -p 8001" &



cd ../../
./gt400/set_ssid_name.sh
./gt400/comm_mavlink &
./gt400/comm_video



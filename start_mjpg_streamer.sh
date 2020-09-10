#!/bin/sh


PIDS='ps -ef | grep mjpg_streamer | grep -v grep | awk '{print $2}''
if [ "$PIDS" != "" ]; then
echo "mjpg_streamer is running"
else
i2cset -y 1 0x70 0x00 0x01
gpio -g mode 17 out
gpio -g mode 4  out
gpio -g write 17 0 #set the gpio17 low
gpio -g write 4 0 #set the gpio4   low

cd /home/pi/mjpg-streamer-master/mjpg-streamer-experimental/
mjpg_streamer -i "./input_raspicam.so" -o "./output_http.so -w ./www -p 8001" &
fi

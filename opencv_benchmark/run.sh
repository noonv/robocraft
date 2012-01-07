#!/bin/sh

file="result.txt"

echo "[i] Start..."
echo "[i] file: $file"

printf "\n" > $file
echo "`cat /proc/cpuinfo | grep \"model name\" | head -n 1` / `uname -s` `uname -r` / OpenCV `cat /usr/local/lib/pkgconfig/opencv.pc | grep \"Version\"`" >> $file
printf "\n" >> $file

echo "[i] benchmarking..."
./opencv_benchmark >> $file

echo "[i] Done."

cat $file

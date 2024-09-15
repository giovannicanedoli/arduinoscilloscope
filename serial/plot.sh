#! /bin/bash

sleep 5 #to be sure I've something to analize

if [ -z "$1" ]; then
    echo "Usage: $0 <#files>"
    echo "BEWARE: you have to start ./serial in order to start plot.sh"
    exit 1
fi

max_range=$1
max_range=$((max_range-1))
open_gnuplot() {
  (
    echo "plot [0:100] [0:6] '< tail -n100 datafile$1' title 'Datafile$1' with lines";
    while :; do 
      sleep .4; 
      echo replot; 
    done
  ) | gnuplot -persist
}

for i in $(seq 0 $max_range); do
    open_gnuplot $i  &
done
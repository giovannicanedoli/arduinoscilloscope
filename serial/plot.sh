#! /bin/bash

sleep 5

(
  echo "plot [0:100] [0:6] '< tail -n100 datafile0' with lines";
  while :; do sleep .4; echo replot; done
) | gnuplot -persist &

(
  echo "plot [0:100] [0:6] '< tail -n100 datafile1' with lines";
  while :; do sleep .4; echo replot; done
) | gnuplot -persist &

(
  echo "plot [0:100] [0:6] '< tail -n100 datafile2' with lines";
  while :; do sleep .4; echo replot; done
) | gnuplot -persist &

(
  echo "plot [0:100] [0:6] '< tail -n100 datafile3' with lines";
  while :; do sleep .4; echo replot; done
) | gnuplot -persist &

(
  echo "plot [0:100] [0:6] '< tail -n100 datafile4' with lines";
  while :; do sleep .4; echo replot; done
) | gnuplot -persist &

# (
#   echo "plot [0:100] [0:6] '< tail -n100 datafile5' with lines";
#   while :; do sleep .4; echo replot; done
# ) | gnuplot -persist 

# (
#   echo "plot [0:100] [0:6] '< tail -n100 datafile6' with lines";
#   while :; do sleep .4; echo replot; done
# ) | gnuplot -persist

# (
#   echo "plot [0:100] [0:6] '< tail -n100 datafile7' with lines";
#   while :; do sleep .4; echo replot; done
# ) | gnuplot -persist

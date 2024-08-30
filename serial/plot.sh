(
  echo "plot [0:100] [0:1050] '< tail -n100 datafile' with lines";
  while :; do sleep .4; echo replot; done
) | gnuplot -persist

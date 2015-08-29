reset
set term postscript
set output "delay.ps"
set xlabel "DataRate(Kbps)"
set ylabel "AvgDelay(s)"
set autoscale
plot "plotDelay.txt" with linespoints, "plotDelay.txt" with yerrorbars
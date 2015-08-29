reset
set term postscript
set output "delay.ps"
set title "Delay-DataRate"
set xlabel "DataRate(Kbps),SD"
set ylabel "Jitter(s)"
set autoscale
plot "plotDelay.txt" with linespoints, "plotDelay.txt" with yerrorlines 

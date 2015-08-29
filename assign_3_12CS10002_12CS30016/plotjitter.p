reset
set term postscript
set output "jitter.ps"
set title "Jitter-DataRate"
set xlabel "DataRate(Kbps),SD"
set ylabel "Jitter(s)"
set autoscale
plot "plotJitter.txt" with linespoints, "plotJitter.txt" with yerrorlines 
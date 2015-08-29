reset
set term postscript
set output "tp.ps"
set title "TP-DataRate"
set xlabel "DataRate(Kbps)"
set ylabel "ThroughtPut(Kbps)"
set autoscale
plot "plotThroughput.txt" with lines, "plotThroughput.txt" with yerrorbars 
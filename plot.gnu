set terminal png nocrop enhanced
set output 'plot.png'
set ylabel 'Milliseconds'
set xlabel 'Thread Count'
plot "timings.txt" using 1:2 title 'matrix multiply' with linespoints lw 2

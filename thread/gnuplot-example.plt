set terminal svg size 350,262 enhanced font 'Verdana,10' background rgb 'white'
set output 'introduction.svg'

# Line width of the axes
set border linewidth 1.5
# Line styles
set style line 1 linecolor rgb '#0060ad' linetype 1 linewidth 2
set style line 2 linecolor rgb '#dd181f' linetype 1 linewidth 2

# Legend
set key at 6.1,1.3
# Axes label
set xlabel 'x'
set ylabel 'y'
# Axes ranges
set xrange [-2*pi:2*pi]
set yrange [-1.5:1.5]
# Axes tics
set xtics ('-2π' -2*pi, '-π' -pi, 0, 'π' pi, '2π' 2*pi)
set ytics 1
set tics scale 0.75

a = 0.9
f(x) = a * sin(x)
g(x) = a * cos(x)
# Plot
plot f(x) title 'sin(x)' with lines linestyle 1, \
     g(x) notitle with lines linestyle 2

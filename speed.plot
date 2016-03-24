# IMPORT-DATA stats stats.txt

set terminal pdf size 28cm,18cm linewidth 2.0
set output "speed.pdf"

set pointsize 0.7
set style line 6 lc rgb "#f0b000"
set style line 15 lc rgb "#f0b000"
set style line 24 lc rgb "#f0b000"
set style line 33 lc rgb "#f0b000"
set style line 42 lc rgb "#f0b000"
set style line 51 lc rgb "#f0b000"
set style line 60 lc rgb "#f0b000"
set style increment user

set grid xtics ytics

set key top left

set xlabel 'Item Count [log_2(n)]'
set ylabel 'Run Time / n log_2n [Nanoseconds]'

#SQL DELETE FROM stats WHERE LOG(2, size) < 12

set title 'Super Scalar Sample Sort Test: Random'
## MULTIPLOT(algo) SELECT LOG(2, size) AS x,
## MEDIAN(time / (size * log(2, size)) * 1e6) AS y,
## MULTIPLOT
## FROM stats WHERE name = "random"
## GROUP BY MULTIPLOT, x ORDER BY MULTIPLOT, x
plot \
    'speed-data.txt' index 0 title "algo=ssssort" with linespoints, \
    'speed-data.txt' index 1 title "algo=stdsort" with linespoints


set title 'Super Scalar Sample Sort Test: 80% Sorted'
## MULTIPLOT(algo) SELECT LOG(2, size) AS x,
## MEDIAN(time / (size * log(2, size)) * 1e6) AS y,
## MULTIPLOT
## FROM stats WHERE name = "80pcsorted"
## GROUP BY MULTIPLOT, x ORDER BY MULTIPLOT, x
plot \
    'speed-data.txt' index 2 title "algo=ssssort" with linespoints, \
    'speed-data.txt' index 3 title "algo=stdsort" with linespoints


set title 'Super Scalar Sample Sort Test: 90% Sorted'
## MULTIPLOT(algo) SELECT LOG(2, size) AS x,
## MEDIAN(time / (size * log(2, size)) * 1e6) AS y,
## MULTIPLOT
## FROM stats WHERE name = "90pcsorted"
## GROUP BY MULTIPLOT, x ORDER BY MULTIPLOT, x


set title 'Super Scalar Sample Sort Test: 99% Sorted'
## MULTIPLOT(algo) SELECT LOG(2, size) AS x,
## MEDIAN(time / (size * log(2, size)) * 1e6) AS y,
## MULTIPLOT
## FROM stats WHERE name = "99pcsorted"
## GROUP BY MULTIPLOT, x ORDER BY MULTIPLOT, x


set title 'Super Scalar Sample Sort Test: 99.9% Sorted'
## MULTIPLOT(algo) SELECT LOG(2, size) AS x,
## MEDIAN(time / (size * log(2, size)) * 1e6) AS y,
## MULTIPLOT
## FROM stats WHERE name = "99.9pcsorted"
## GROUP BY MULTIPLOT, x ORDER BY MULTIPLOT, x


set title 'Super Scalar Sample Sort Test: 90% Sorted + 10% Random Tail'
## MULTIPLOT(algo) SELECT LOG(2, size) AS x,
## MEDIAN(time / (size * log(2, size)) * 1e6) AS y,
## MULTIPLOT
## FROM stats WHERE name = "tail90"
## GROUP BY MULTIPLOT, x ORDER BY MULTIPLOT, x


set title 'Super Scalar Sample Sort Test: 99% Sorted + 1% Random Tail'
## MULTIPLOT(algo) SELECT LOG(2, size) AS x,
## MEDIAN(time / (size * log(2, size)) * 1e6) AS y,
## MULTIPLOT
## FROM stats WHERE name = "tail99"
## GROUP BY MULTIPLOT, x ORDER BY MULTIPLOT, x


set title 'Super Scalar Sample Sort Test: Sorted'
## MULTIPLOT(algo) SELECT LOG(2, size) AS x,
## MEDIAN(time / (size * log(2, size)) * 1e6) AS y,
## MULTIPLOT
## FROM stats WHERE name = "sorted"
## GROUP BY MULTIPLOT, x ORDER BY MULTIPLOT, x


set title 'Super Scalar Sample Sort Test: Reverse Sorted'
## MULTIPLOT(algo) SELECT LOG(2, size) AS x,
## MEDIAN(time / (size * log(2, size)) * 1e6) AS y,
## MULTIPLOT
## FROM stats WHERE name = "reverse"
## GROUP BY MULTIPLOT, x ORDER BY MULTIPLOT, x

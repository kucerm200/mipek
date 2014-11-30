set term png
set output "printme.png"
set grid  xtics ytics
set   autoscale                        # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title "závislost času na počtu procesů -- triangle dimenze 6"
set xlabel "počet procesů"
set ylabel "čas (sec)"

plot "data6" using 1:2 title 'čas' with line , "data6" using 1:2:($2-0.08*$2):($2+0.08*$2) title '8% odchylka měření' with errorbars, 287.384048/x with line title 'lineární zrychlení'
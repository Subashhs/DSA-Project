
        set terminal pngcairo size 800,600 enhanced font 'Verdana,10'
        set output 'service_times_graph.png'
        set title "Customer Service Times"
        set xlabel "Customer ID"
        set ylabel "Service Time"
        plot 'service_times.dat' using 1:2 with linespoints title 'Service Time'
    
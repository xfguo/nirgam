# avg latency per pkt along N and S channels
reset

set term postscript
set output "results/graphs/ylatency_pkt"
set xlabel "Routers along X-direction"
set ylabel "Routers along Y-direction"
set title "Average latency per packet (clock cycles) along N and S channels"
set label "R1" at 0.9,0.5
set arrow 1 from 0.2,0.5 to 0,0.7,0.3
set label "N" at 0,0.5,0.6
set arrow 2 from 0.8,0.5 to 0,1.7,0.3
set label "S" at 0,1.5,0.6
set ticslevel 0
set hidden3d
splot "log/gnuplot/ylatency" using 1:2:3 with lines

#added
# avg Power
reset
set term postscript eps enhanced color
set pm3d
set output "results/graphs/power"
set xlabel "Routers along X-direction"
set ylabel "Routers along Y-direction"
#set zlabel "Power(in Watt)" center rotate by 90
set title "Power Consumption at routers (in Watt)"
set label "R0" at 0.7,0.5
#set arrow 1 from 0.2,0.5 to 0,0.7,0.3
#set label "N" at 0,0.5,0.6
#set arrow 2 from 0.8,0.5 to 0,1.7,0.3
#set label "S" at 0,1.5,0.6
set ticslevel 0
set hidden3d
splot "log/gnuplot/power" using 1:2:3 with lines 



# avg latency per pkt along E and W channels
reset

set term postscript
set output "results/graphs/xlatency_pkt"
set xlabel "Routers along X-direction"
set ylabel "Routers along Y-direction"
set title "Average latency per packet (clock cycles) along E and W channels"
set label "R1" at 0.9,0.5
set arrow 1 from 0.2,0.5 to 0,0.7,0.3
set label "E" at 0,0.4,0.7
set ticslevel 0
set hidden3d
splot "log/gnuplot/xlatency" using 1:2:3 with lines

# avg latency per flit along N and S directions
reset
set term postscript
set output "results/graphs/ylatency_flit"
set xlabel "Routers along X-direction"
set ylabel "Routers along Y-direction"
set title "Average latency per flit (clock cycles) along N and S channels"
set label "R1" at 0.9,0.5
set arrow 1 from 0.2,0.5 to 0,0.7,0.2
set label "N" at 0,0.5,0.3
set arrow 2 from 0.8,0.5 to 0,1.7,0.2
set label "S" at 0,1.5,0.3
set ticslevel 0
set hidden3d
splot "log/gnuplot/ylatency_flit" using 1:2:3 with lines

# avg latency per flit along E and W directions
reset

set term postscript
set output "results/graphs/xlatency_flit"
set xlabel "Routers along X-direction"
set ylabel "Routers along Y-direction"
set title "Average latency per flit (clock cycles) along E and W channels"
set label "R1" at 0.9,0.5
set arrow 1 from 0.2,0.5 to 0,0.7,0.3
set label "E" at 0,0.5,0.5
set ticslevel 0
set hidden3d
splot "log/gnuplot/xlatency_flit" using 1:2:3 with lines

# avg. throughput in Gbps along N and S channels
reset

set term postscript
set output "results/graphs/ytput"
set xlabel "Routers along X-direction"
set ylabel "Routers along Y-direction"
set title "Average throughput (Gbps) along N and S channels"
set label "R1" at 0.9,0.5
set arrow 1 from 0.2,0.5 to 0,0.7,1
set label "N" at 0,0.4,2.5
set arrow 2 from 0.8,0.5 to 0,1.7,1
set label "S" at 0,1.4,2.5
set ticslevel 0
set hidden3d
splot "log/gnuplot/ytput" using 1:2:3 with lines

# avg. throughput in Gbps along E and W channels
reset

set term postscript
set output "results/graphs/xtput"
set xlabel "Routers along X-direction"
set ylabel "Routers along Y-direction"
set title "Average throughput (Gbps) along E and W channels"
set label "R1" at 0.9,0.5
set arrow 1 from 0.2,0.5 to 0,0.7,1
set label "E" at 0,0.4,2.5
set ticslevel 0
set hidden3d
splot "log/gnuplot/xtput" using 1:2:3 with lines

# plotting the two graphs for latency per pkt on one
reset

set term postscript eps color
set output "results/graphs/latency_pkt"
set ticslevel 0
set hidden3d

set size 1,1
set origin 0,0

set multiplot

set size 0.5,1
set origin 0,0
splot "log/gnuplot/ylatency" using 1:2:3 with lines
 
set size 0.5,1
set origin 0.5,0
splot "log/gnuplot/xlatency" using 1:2:3 with lines

unset multiplot

# plotting the two graphs for latency per flit on one
reset

set term postscript eps color
set output "results/graphs/latency_flit"
set ticslevel 0
set hidden3d

set size 1,1
set origin 0,0

set multiplot

set size 0.5,1
set origin 0,0
splot "log/gnuplot/ylatency_flit" using 1:2:3 with lines
 
set size 0.5,1
set origin 0.5,0
splot "log/gnuplot/xlatency_flit" using 1:2:3 with lines

unset multiplot

# plotting the two graphs for avg. throughput on one
reset

set term postscript eps color
set output "results/graphs/throughput"
set ticslevel 0
set hidden3d

set size 1,1
set origin 0,0

set multiplot

set size 0.5,1
set origin 0,0
splot "log/gnuplot/ytput" using 1:2:3 with lines
 
set size 0.5,1
set origin 0.5,0
splot "log/gnuplot/xtput" using 1:2:3 with lines

unset multiplot

reset


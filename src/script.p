set title "Running Time Vs Number of Threads"

set ylabel "Runnig Time"
set xlabel "Number of Threads"

plot 'mat.dat'  using 1:2 with boxes title "userThread run time",\
'mat2.dat' using 1:2 with boxes title "pthread run time"


pause -1 "press key"                       
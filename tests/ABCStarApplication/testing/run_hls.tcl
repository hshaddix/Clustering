open_project -reset processHits
set_top processHits
add_files processHits.cpp
add_files -tb testBench.cpp
open_solution solution1
set_part {xcvu9p-flga2104-2-e}
create_clock -period 5 -name default
csim_design
csynth_design
cosim_design -rtl
export_design -format ip_catalog
exit

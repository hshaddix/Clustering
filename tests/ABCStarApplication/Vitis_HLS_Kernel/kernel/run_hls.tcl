# Create a project
open_project -reset processHits

# Add design files
add_files processHits.cpp
# Add test bench & files
add_files -tb testBench.cpp

# Set the top-level function
set_top processHits

# ########################################################
# Create a solution
open_solution -reset solution1
# Define technology and clock rate
set_part  {xcvu9p-flga2104-2-e}
create_clock -period 5

# Set variable to select which steps to execute
set hls_exec 2

# Run C simulation
csim_design

if {$hls_exec == 1} {
    # Run Synthesis and Exit
    csynth_design
    
} elseif {$hls_exec == 2} {
    # Run Synthesis, RTL Simulation and Exit
    csynth_design
    
    cosim_design
} elseif {$hls_exec == 3} { 
    # Run Synthesis, RTL Simulation, RTL implementation and Exit
    csynth_design
    
    cosim_design
    export_design -format ip_catalog -xo processHits.xo
} else {
    # Default is to exit after setup
    csynth_design
}

exit

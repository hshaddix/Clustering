This is my testing attempt to kernel build with my HLS code with some steps on how I am currently trying to run it: 

from hpcfpga02:

1) source /opt/Xilinx/Vitis_HLS/2023.1/settings64.sh 
2) Clone Vitis_HLS_Kernel 
3) cd Vitis_HLS_Kernel/kernel 
4) vitis_hls -f run_hls.tcl 

This should create a .xo file, although I have been simply running the GUI and moving the .xo into the kernel directory (to fit pathing). 
Next the idea is to run the Makefile: 

from Vitis_HLS_Kernel:

1) cd /src/
2) make all 

This should be creating a file at the full pathway Vitis_HLS_Kernel/src/build/processHits.xclbin (but is not currently)

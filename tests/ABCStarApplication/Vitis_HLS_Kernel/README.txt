Further documentation of this code and project can be found at: https://cds.cern.ch/record/2908246

The following is how to run the kernelization using this repo: 
from hpcfpga02:

1) source /opt/Xilinx/Vitis_HLS/2023.1/settings64.sh 
2) Clone Vitis_HLS_Kernel 
3) cd Vitis_HLS_Kernel/src
4) make all 

This creates both a .xo and .xclbin file. From here we want to mount the two files using the host.cpp to make sure the kernel works in application. As of right now I am still working on this.. 

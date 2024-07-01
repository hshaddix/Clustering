# Makefile

# Define the platform
PLATFORM := xilinx_u250_gen3x16_xdma_4_1_202210_1

# Host compiler global settings
CXX := g++
CXXFLAGS := -Wall -O0 -g -std=c++11 -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vitis/2023.1/include -I.

# Kernel compiler global settings
VPP := v++
VPPFLAGS := -l -t hw --platform $(PLATFORM)

# Include libraries
LDFLAGS := -L/opt/xilinx/xrt/lib -lOpenCL -lpthread -lrt -lstdc++ -L/opt/Xilinx/Vitis/2023.1/lib -lxilinxopencl -lxrt_coreutil

# Host files
HOST_SRCS := host.cpp xcl2.cpp

# Kernel files
KERNEL_XO := ../kernel/processHits.xo

# Build directories
BUILD_DIR := build

# Host executable
HOST_EXE := $(BUILD_DIR)/host

# Kernel binary
KERNEL_XCLBIN := $(BUILD_DIR)/processHits.xclbin

all: $(HOST_EXE) $(KERNEL_XCLBIN)

$(HOST_EXE): $(HOST_SRCS)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(KERNEL_XCLBIN): $(KERNEL_XO)
	mkdir -p $(BUILD_DIR)
	$(VPP) $(VPPFLAGS) -o $@ $^

clean:
	rm -rf $(BUILD_DIR) *~ *.o

.PHONY: all clean

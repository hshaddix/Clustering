# Makefile to build the Vitis kernel and host application

# Compiler and linker flags
CXX := g++
CXXFLAGS := -Wall -O0 -g -std=c++11 -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vitis/2023.1/include -I.
LDFLAGS := -L/opt/xilinx/xrt/lib -lOpenCL -lpthread -lrt -lstdc++ -L/opt/Xilinx/Vitis/2023.1/lib -lxilinxopencl -lxrt_coreutil

# Directories
KERNEL_DIR := ../kernel
BUILD_DIR := build
SRC_DIR := .

# Files
KERNEL_XO := $(KERNEL_DIR)/processHits.xo
KERNEL_XCLBIN := $(BUILD_DIR)/processHits.xclbin
HOST_SRC := $(SRC_DIR)/host.cpp $(SRC_DIR)/xcl2.cpp
HOST_EXE := $(BUILD_DIR)/host
CONFIG_FILE := $(SRC_DIR)/config.cfg

# Platform
PART := xilinx_u250_gen3x16_xdma_4_1_202210_1
PLATFORM := /opt/xilinx/platforms/$(PART)/$(PART).xpfm

all: $(HOST_EXE) $(KERNEL_XCLBIN)

.PHONY: host compile link clean all

# Compile the host application
$(HOST_EXE): $(HOST_SRC)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile the kernel
$(KERNEL_XO): $(KERNEL_DIR)/processHits.cpp $(KERNEL_DIR)/processHits.h
	mkdir -p $(BUILD_DIR)
	v++ -c -t hw --platform $(PLATFORM) -k processHits -I$(KERNEL_DIR) $(KERNEL_DIR)/processHits.cpp -o $(KERNEL_XO)

# Link the kernel to create the xclbin
$(KERNEL_XCLBIN): $(KERNEL_XO) $(CONFIG_FILE)
	mkdir -p $(BUILD_DIR)
	v++ -l -t hw --platform $(PLATFORM) --config $(CONFIG_FILE) $(KERNEL_XO) -o $(KERNEL_XCLBIN)

clean:
	rm -rf $(BUILD_DIR)

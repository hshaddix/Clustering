TARGET := hw
DEVICE := xilinx_u200_xdma_201830_2
XOCC := v++ -t $(TARGET) --platform $(DEVICE)

BINARY_CONTAINERS += _x.hw.xclbin
BINARY_CONTAINER_OBJS += kernel/processHits.hw.xo

all: $(BINARY_CONTAINERS) $(BINARY_CONTAINER_OBJS)

$(BINARY_CONTAINERS): $(BINARY_CONTAINER_OBJS)
    $(XOCC) -l --config ../src/host.cfg -o $@ $^

%.xo: %.cpp
    $(XOCC) -c --config ../src/host.cfg -k $(basename $(notdir $<)) -I../src -o $@ $<

clean:
    rm -f $(BINARY_CONTAINERS) $(BINARY_CONTAINER_OBJS)
    rm -f *.o *.xo *.xclbin

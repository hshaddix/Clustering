#include "xcl2.hpp"
#include <vector>
#include <iostream>
#include <bitset>

#define DATA_SIZE 4096

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];
    cl_int err;
    std::vector<short> inputData(DATA_SIZE);
    std::vector<short> outputData(DATA_SIZE);

    size_t input_size_in_bytes = inputData.size() * sizeof(short);
    size_t output_size_in_bytes = outputData.size() * sizeof(short);

    // Populate input data
    for (size_t i = 0; i < DATA_SIZE; ++i) {
        inputData[i] = i % 256;
    }

    // Get the Xilinx devices and program the device
    auto devices = xcl::get_xil_devices();
    auto device = devices[0];
    auto context = cl::Context(device, nullptr, nullptr, nullptr, &err);
    auto q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err);
    auto fileBuf = xcl::read_binary_file(binaryFile);
    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    auto program = cl::Program(context, {device}, bins, nullptr, &err);

    auto krnl = cl::Kernel(program, "processHits", &err);

    // Allocate Buffer in Global Memory
    cl::Buffer buffer_in(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, input_size_in_bytes, inputData.data(), &err);
    cl::Buffer buffer_out(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, output_size_in_bytes, outputData.data(), &err);

    // Set the kernel arguments
    krnl.setArg(0, buffer_in);
    krnl.setArg(1, buffer_out);

    // Copy input data to device global memory
    q.enqueueMigrateMemObjects({buffer_in}, 0);

    // Launch the Kernel
    q.enqueueTask(krnl);

    // Copy result from device global memory to host local memory
    q.enqueueMigrateMemObjects({buffer_out}, CL_MIGRATE_MEM_OBJECT_HOST);
    q.finish();

    // Print output
    for (size_t i = 0; i < DATA_SIZE; i++) {
        std::cout << "Output[" << i << "] = " << std::bitset<16>(outputData[i]) << std::endl;
    }

    return 0;
}

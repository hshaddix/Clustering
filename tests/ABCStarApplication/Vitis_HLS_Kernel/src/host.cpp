#include "xcl2.hpp"
#include <vector>
#include <iostream>
#include <bitset>

// Define the data size
#define DATA_SIZE 4096

// Function to set up OpenCL device
int setupDevice(std::vector<cl::Device>& devices, cl::Device& device) {
    cl_int err;
    std::vector<cl::Platform> platforms;
    // Get all available platforms (e.g., Xilinx, Intel, AMD)
    OCL_CHECK(err, err = cl::Platform::get(&platforms));

    // Find Xilinx platform
    for (size_t i = 0; i < platforms.size(); i++) {
        cl::Platform platform = platforms[i];
        std::string platformName = platform.getInfo<CL_PLATFORM_NAME>(&err);
        if (platformName == "Xilinx") {
  // Get devices for the Xilinx platform
            OCL_CHECK(err, err = platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices));
            device = devices[0]; // Use the first available device
            return EXIT_SUCCESS;
        }
    }
    std::cerr << "Error: Failed to find Xilinx platform or devices." << std::endl;
    return EXIT_FAILURE;
}

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

    // Set up OpenCL device
    std::vector<cl::Device> devices;
    cl::Device device;
    if (setupDevice(devices, device) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }
 // Create OpenCL context and command queue
    auto context = cl::Context(device, nullptr, nullptr, nullptr, &err);
    auto q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err);

    // Read binary file and create program
    auto fileBuf = xcl::read_binary_file(binaryFile);
    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    auto program = cl::Program(context, {device}, bins, nullptr, &err);

    // Create kernel
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

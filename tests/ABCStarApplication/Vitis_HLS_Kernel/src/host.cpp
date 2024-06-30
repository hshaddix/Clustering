#include "xcl2.hpp"
#include <vector>
#include <iostream>

// Helper function to check the adjacency of hits
bool areAdjacent(const int &pos1, const int &pos2, const int &size1) {
    return (pos1 + size1 == pos2);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN file>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];
    cl_int err;
    cl::Context context;
    cl::Kernel kernel;
    cl::CommandQueue q;

    // Initialize the OpenCL environment
    auto devices = xcl::get_xil_devices();
    auto fileBuf = xcl::read_binary_file(binaryFile);
    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    bool valid_device = false;

    for (unsigned int i = 0; i < devices.size(); i++) {
        auto device = devices[i];
        OCL_CHECK(err, context = cl::Context(device, nullptr, nullptr, nullptr, &err));
        OCL_CHECK(err, q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
        std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        cl::Program program(context, {device}, bins, nullptr, &err);
        if (err != CL_SUCCESS) {
            std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
        } else {
            std::cout << "Device[" << i << "]: program successful!\n";
            OCL_CHECK(err, kernel = cl::Kernel(program, "processHits", &err));
            valid_device = true;
            break;
        }
    }

    if (!valid_device) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    // Input data
    std::vector<ap_uint<16>> inputData = {
        // Add input data here
    };

    size_t input_size_in_bytes = inputData.size() * sizeof(ap_uint<16>);
    std::vector<ap_uint<16>> outputData(inputData.size());

    // Allocate device buffer
    OCL_CHECK(err, cl::Buffer buffer_input(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, input_size_in_bytes, inputData.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_output(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, input_size_in_bytes, outputData.data(), &err));

    // Set kernel arguments
    OCL_CHECK(err, err = kernel.setArg(0, buffer_input));
    OCL_CHECK(err, err = kernel.setArg(1, buffer_output));

    // Copy input data to device global memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_input}, 0));

    // Launch the kernel
    OCL_CHECK(err, err = q.enqueueTask(kernel));

    // Copy result from device to host
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output}, CL_MIGRATE_MEM_OBJECT_HOST));
    OCL_CHECK(err, err = q.finish());

    // Print results
    std::cout << "Output results:" << std::endl;
    for (size_t i = 0; i < outputData.size(); i++) {
        std::cout << "Output[" << i << "]: " << outputData[i].to_string(16) << std::endl;
    }

    return 0;
}

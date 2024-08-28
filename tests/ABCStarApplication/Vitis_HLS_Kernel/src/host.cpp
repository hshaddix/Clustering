#define CL_TARGET_OPENCL_VERSION 220  // Target OpenCL version 2.2
#include <CL/cl.h>
#include <vector>
#include <iostream>
#include <bitset>
#include <fstream>

// Define the data size
#define DATA_SIZE 4096

// Error checking macro
#define checkError(status, message) \
    if (status != CL_SUCCESS) { \
        std::cerr << "Error: " << message << " (" << status << ")" << std::endl; \
        exit(EXIT_FAILURE); \
    }

// Function to read binary file
std::vector<unsigned char> readBinaryFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open binary file " << filename << std::endl;
        exit(EXIT_FAILURE);
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<unsigned char> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        std::cerr << "Error: Unable to read binary file " << filename << std::endl;
        exit(EXIT_FAILURE);
    }
    return buffer;
}

// Function to set up OpenCL device
int setupDevice(cl_device_id& device) {
    cl_int status;
    cl_uint numPlatforms;
    status = clGetPlatformIDs(0, nullptr, &numPlatforms);
    checkError(status, "Failed to get platform IDs.");

    std::vector<cl_platform_id> platforms(numPlatforms);
    status = clGetPlatformIDs(numPlatforms, platforms.data(), nullptr);
    checkError(status, "Failed to get platform IDs.");

    bool found_device = false;
    for (size_t i = 0; i < platforms.size() && !found_device; ++i) {
        cl_platform_id platform = platforms[i];
        char platformName[128];
        status = clGetPlatformInfo(platform, CL_PLATFORM_NAME, 128, platformName, nullptr);
        checkError(status, "Failed to get platform name.");

        if (std::string(platformName) == "Xilinx") {
            cl_uint numDevices;
            status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ACCELERATOR, 0, nullptr, &numDevices);
            checkError(status, "Failed to get device IDs.");

            std::vector<cl_device_id> devices(numDevices);
            status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ACCELERATOR, numDevices, devices.data(), nullptr);
            checkError(status, "Failed to get device IDs.");

            for (size_t d = 0; d < devices.size(); ++d) {
                device = devices[d];
                char deviceName[128];
                status = clGetDeviceInfo(device, CL_DEVICE_NAME, 128, deviceName, nullptr);
                checkError(status, "Failed to get device name.");
                std::cout << "Found device: " << deviceName << std::endl;
                found_device = true;
                break;
            }
        }
    }

    if (!found_device) {
        std::cerr << "Error: Unable to find target device" << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];
    cl_int status;
    std::vector<short> inputData(DATA_SIZE);
    std::vector<short> outputData(DATA_SIZE);

    size_t input_size_in_bytes = inputData.size() * sizeof(short);
    size_t output_size_in_bytes = outputData.size() * sizeof(short);

    // Populate input data
    for (size_t i = 0; i < DATA_SIZE; ++i) {
        inputData[i] = i % 256;
    }

    // Set up OpenCL device
    cl_device_id device;
    if (setupDevice(device) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    // Create OpenCL context and command queue
    cl_context context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &status);
    checkError(status, "Failed to create context.");

    cl_command_queue_properties props[] = { CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0 };
    cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, props, &status);
    checkError(status, "Failed to create command queue with properties.");

    // Read binary file and create program
    std::vector<unsigned char> fileBuf = readBinaryFile(binaryFile);

    size_t binary_size = fileBuf.size();
    if (binary_size == 0) {
        std::cerr << "Error: Binary file is empty or could not be read." << std::endl;
        return EXIT_FAILURE;
    }

    const unsigned char* binary_data = fileBuf.data();
    cl_program program = clCreateProgramWithBinary(context, 1, &device, &binary_size, &binary_data, nullptr, &status);
    if (status != CL_SUCCESS || program == nullptr) {
        std::cerr << "Failed to create program with binary. Error code: " << status << std::endl;
        return EXIT_FAILURE;
    }

    status = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
    checkError(status, "Failed to build program.");

    // Create kernel
    cl_kernel kernel = clCreateKernel(program, "processHits", &status);
    checkError(status, "Failed to create kernel.");

    // Allocate Buffer in Global Memory
    cl_mem buffer_in = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, input_size_in_bytes, inputData.data(), &status);
    checkError(status, "Failed to create input buffer.");

    cl_mem buffer_out = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, output_size_in_bytes, outputData.data(), &status);
    checkError(status, "Failed to create output buffer.");

    // Set the kernel arguments
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer_in);
    checkError(status, "Failed to set kernel argument 0.");

    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &buffer_out);
    checkError(status, "Failed to set kernel argument 1.");

    // Copy input data to device global memory
    status = clEnqueueWriteBuffer(queue, buffer_in, CL_TRUE, 0, input_size_in_bytes, inputData.data(), 0, nullptr, nullptr);
    checkError(status, "Failed to enqueue write buffer.");

    // Launch the Kernel
    size_t globalWorkSize = DATA_SIZE;
    status = clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &globalWorkSize, nullptr, 0, nullptr, nullptr);
    checkError(status, "Failed to enqueue ND range kernel.");

    // Copy result from device global memory to host local memory
    status = clEnqueueReadBuffer(queue, buffer_out, CL_TRUE, 0, output_size_in_bytes, outputData.data(), 0, nullptr, nullptr);
    checkError(status, "Failed to enqueue read buffer.");
    clFinish(queue);

    // Print output
    for (size_t i = 0; i < DATA_SIZE; i++) {
        std::cout << "Output[" << i << "] = " << std::bitset<16>(outputData[i]) << std::endl;
    }

    // Cleanup
    clReleaseMemObject(buffer_in);
    clReleaseMemObject(buffer_out);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    return 0;
}

#include <CL/cl.hpp>
// filepath error above
#include <iostream>
#include <vector>

const int ARRAY_SIZE = 1024;

int main() {
    // Initialize input arrays
    std::vector<int> arrayA(ARRAY_SIZE, 1);
    std::vector<int> arrayB(ARRAY_SIZE, 2);
    
    // Initialize OpenCL context and command queue
    cl::Context context(CL_DEVICE_TYPE_GPU);
    std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
    cl::CommandQueue queue(context, devices[0]);
    
    // Create OpenCL buffers for input and output arrays
    cl::Buffer bufferA(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * ARRAY_SIZE, arrayA.data());
    cl::Buffer bufferB(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * ARRAY_SIZE, arrayB.data());
    cl::Buffer bufferC(context, CL_MEM_WRITE_ONLY, sizeof(int) * ARRAY_SIZE);
    
    // Read the OpenCL kernel source code from a file
    std::ifstream kernelFile("add_arrays_kernel.cl");
    std::string kernelSource(std::istreambuf_iterator<char>(kernelFile), (std::istreambuf_iterator<char>()));
    cl::Program::Sources sources(1, std::make_pair(kernelSource.c_str(), kernelSource.length() + 1));
    
    // Create OpenCL program and build it
    cl::Program program(context, sources);
    program.build(devices);
    
    // Create OpenCL kernel
    cl::Kernel kernel(program, "add_arrays");
    kernel.setArg(0, bufferA);
    kernel.setArg(1, bufferB);
    kernel.setArg(2, bufferC);
    
    // Enqueue the kernel for execution
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(ARRAY_SIZE), cl::NullRange);
    
    // Read the result back from the GPU
    std::vector<int> arrayC(ARRAY_SIZE);
    queue.enqueueReadBuffer(bufferC, CL_TRUE, 0, sizeof(int) * ARRAY_SIZE, arrayC.data());
    
    // Output the result
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        std::cout << "Result[" << i << "] = " << arrayC[i] << "\n";
    }

    return 0;
}

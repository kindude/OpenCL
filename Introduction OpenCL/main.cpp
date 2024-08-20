#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <CL/cl.hpp>
#include <time.h>
#include <cstdlib>
#include <conio.h>
using namespace std;

#define _CRT_SECURE_NO_WARNINGS

#define VECTOR_SIZE 8

const char* kernel_src =
"__kernel void kernelsum(__global int *a, __global int *b, __global int*c, __global int*d) {\n"
"    int i = get_global_id(0); \n"
"    a[i] = a[i] + b[i];\n"
"    c[i] = c[i] + d[i];\n"
"}";

#define LIST_SIZE 8

int main(void) {
    // Creating arrays
    int* a = new int[LIST_SIZE];
    int* b = new int[LIST_SIZE];
    int* c = new int[LIST_SIZE];
    int* d = new int[LIST_SIZE];
    for (int i = 0; i < LIST_SIZE; i++) {
        a[i] = rand() % 100;
        b[i] = rand() % 100;
        c[i] = rand() % 100;
        d[i] = rand() % 100;
    }
    printf("A:");
    for (int i = 0; i < LIST_SIZE; i++) printf("%d\t", a[i]);
    printf("\n");
    printf("B:");
    for (int i = 0; i < LIST_SIZE; i++) printf("%d\t", b[i]);
    printf("\n");
    printf("C:");
    for (int i = 0; i < LIST_SIZE; i++) printf("%d\t", c[i]);
    printf("\n");
    printf("D:");
    for (int i = 0; i < LIST_SIZE; i++) printf("%d\t", d[i]);
    printf("\n");

    // Receiving information about platforms and devices
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_uint num_devices;
    cl_uint num_platforms;
    cl_int err = clGetPlatformIDs(1, &platform_id, &num_platforms);
    err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &num_devices);

    // Creating context
    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);

    // Creating a command queue
    cl_command_queue queue = clCreateCommandQueue(context, device_id, 0, &err);

    // Creating buffer arrays for each array
    cl_mem a_buf = clCreateBuffer(context, CL_MEM_READ_ONLY, LIST_SIZE * sizeof(int), NULL, &err);
    cl_mem b_buf = clCreateBuffer(context, CL_MEM_READ_ONLY, LIST_SIZE * sizeof(int), NULL, &err);
    cl_mem c_buf = clCreateBuffer(context, CL_MEM_WRITE_ONLY, LIST_SIZE * sizeof(int), NULL, &err);
    cl_mem d_buf = clCreateBuffer(context, CL_MEM_WRITE_ONLY, LIST_SIZE * sizeof(int), NULL, &err);

    // Copying arrays
    err = clEnqueueWriteBuffer(queue, a_buf, CL_TRUE, 0, LIST_SIZE * sizeof(int), a, 0, NULL, NULL);
    err = clEnqueueWriteBuffer(queue, b_buf, CL_TRUE, 0, LIST_SIZE * sizeof(int), b, 0, NULL, NULL);
    err = clEnqueueWriteBuffer(queue, c_buf, CL_TRUE, 0, LIST_SIZE * sizeof(int), c, 0, NULL, NULL);
    err = clEnqueueWriteBuffer(queue, d_buf, CL_TRUE, 0, LIST_SIZE * sizeof(int), d, 0, NULL, NULL);
    // Creating a program
    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&kernel_src, NULL, &err);

    // Program compilation
    err = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    // Kernel creation
    cl_kernel kernel = clCreateKernel(program, "kernelsum", &err);

    // Adjusting kernel settings
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&a_buf);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&b_buf);
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&c_buf);
    err = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&d_buf);
    // Executing the kernel
    size_t global_size = LIST_SIZE;
    size_t local_size = 2;
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, &local_size, 0, NULL, NULL);

    // Reading from buffers
    err = clEnqueueReadBuffer(queue, a_buf, CL_TRUE, 0, LIST_SIZE * sizeof(int), a, 0, NULL, NULL);
    err = clEnqueueReadBuffer(queue, c_buf, CL_TRUE, 0, LIST_SIZE * sizeof(int), c, 0, NULL, NULL);
    // Displaying results
    printf("a after addition A + B:");
    for (int i = 0; i < LIST_SIZE; i++) printf("%d\t", a[i]);
    printf("\n");
    printf("c after addition C + D:");
    for (int i = 0; i < LIST_SIZE; i++) printf("%d\t", b[i]);
    printf("\n");

    

    int pr = 0;
    for (int i = 0; i < LIST_SIZE; i++)
        pr += a[i]*c[i];

    printf("Scalar multiplication after addition:%d ", pr);

    err = clFlush(queue);
    err = clFinish(queue);
    err = clReleaseKernel(kernel);
    err = clReleaseProgram(program);
    err = clReleaseMemObject(a_buf);
    err = clReleaseMemObject(b_buf);
    err = clReleaseMemObject(c_buf);
    err = clReleaseMemObject(d_buf);
    err = clReleaseCommandQueue(queue);
    err = clReleaseContext(context);

    return 0;
}

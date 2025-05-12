#include "WatershedFast_gpu.h"
#include <cuda_runtime.h>
#include <stdio.h> 
#include <stdexcept>
__constant__ unsigned char dirs[8] = {1,2,4,8,16,32,64,128};

__device__ bool moveToDownstreamCell(unsigned char* devDirGrid, int& r,  int& c, 
     int  height,  int  width)
{
    unsigned int index = r * width + c;

    unsigned char dir = devDirGrid[index];
    switch (dir) {
    case 1:
        c++;
        break;
    case 2:
        c++; r++;
        break;
    case 4:
        r++;
        break;
    case 8:
        c--; r++;
        break;
    case 16:
        c--;
        break;
    case 32:
        c--; r--;
        break;
    case 64:
        r--;
        break;
    case 128:
        c++; r--;
        break;
    };

    if (r < 0 || r >= height || c < 0 || c >= width) return false; //Outside
    if (devDirGrid[r * width + c] == 0) return false; // NO_DATA
    return true;
}

__global__ void calculateWatershed(unsigned char* devDirGrid, int* devWsGrid, unsigned int  height, unsigned int  width)
{
    int  c = blockIdx.x * blockDim.x + threadIdx.x;
    int  r = blockIdx.y * blockDim.y + threadIdx.y;

    if (c >= width || r >= height) return;
    unsigned int indexOrg = r * width + c;

    if (devDirGrid[indexOrg] == 0) return;  // NODATA
    do
    {
        int id = devWsGrid[r * width + c];
        if (id != 0) // not nodata
        {
            devWsGrid[indexOrg] = id;
            break;
        }
    } while (moveToDownstreamCell(devDirGrid, r, c, height, width));
}

void WatershedFlowPathTraversalGPU(unsigned char* devDirGrid, int* devWsGrid, int height, int width)
{
    dim3 threadsPerBlock(16, 16);
    //dim3 threadsPerBlock(32, 32);
    dim3 numBlocks((width + threadsPerBlock.x -1)/ threadsPerBlock.x, (height+ threadsPerBlock.y-1) / threadsPerBlock.y);
    calculateWatershed <<<numBlocks, threadsPerBlock >>> (devDirGrid, devWsGrid,height,width);
 
    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess) {
        throw std::runtime_error(cudaGetErrorString(err));
    }
}

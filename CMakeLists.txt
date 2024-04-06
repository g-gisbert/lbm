cmake_minimum_required(VERSION 3.16)

#set(CUDA_TOOLKIT_ROOT_DIR "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.4")
project(efs LANGUAGES CXX CUDA)
#find_package(CUDA REQUIRED)
#project(efs LANGUAGES CXX)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

include_directories(${GLEW_INCLUDE_DIRS})
link_libraries(${GLEW_LIBRARIES})


add_executable(efs src/main.cpp)
target_link_libraries(efs PRIVATE glfw)
target_link_libraries(efs PRIVATE ${OPENGL_LIBRARIES})
target_include_directories(efs PRIVATE ${OPENGL_INCLUDE_DIRS})
target_include_directories(efs PUBLIC "/mnt/d/dev/eulerianFluidSimulation/deps/glew-cmake/include")


#[[cmake_minimum_required(VERSION 3.12)

project(YourOptiXProject)

# Set the path to OptiX installation
set(OptiX_INSTALL_DIR "/home/full/docu/NVIDIA-OptiX-SDK-8.0.0-linux64-x86_64/")

# Set the path to CUDA toolkit installation
set(CUDA_TOOLKIT_ROOT_DIR "/mnt/c/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.4")

# Set the path to OptiX SDK
set(OptiX_SDK_DIR "${OptiX_INSTALL_DIR}/SDK")

# Set the path to CUDA SDK
set(CUDA_SDK_DIR "${CUDA_TOOLKIT_ROOT_DIR}/samples/common/inc")

# Set CMake to find CUDA
find_package(CUDA REQUIRED)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(OptiX REQUIRED)
include_directories(${OptiX_INCLUDE})

# Set include directories for CUDA and OptiX
include_directories(${CUDA_INCLUDE_DIRS})
include_directories("${OptiX_SDK_DIR}/include")

# Set link directories for CUDA and OptiX
link_directories("${OptiX_INSTALL_DIR}/lib64")
link_directories("${CUDA_TOOLKIT_ROOT_DIR}/lib64")

# Set compiler flags for CUDA
set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} -std=c++11")

# Add your source files
set(SOURCE_FILES main.cu)

# Source files
set(SOURCES
        main.cu
        # Add your other source files here
        )

# CUDA files
set(CUDA_SOURCES
        main.cu
        # Add your other CUDA source files here
        )

# CUDA compilation
cuda_add_library(CudaKernels ${CUDA_SOURCES})

# Executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Linking
target_link_libraries(${PROJECT_NAME} CudaKernels ${OptiX_LIBRARIES})]]
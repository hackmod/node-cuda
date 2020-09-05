#include "bindings.hpp"
#include "ctx.hpp"
#include "device.hpp"
#include "stream.hpp"
#include "function.hpp"
#include "mem.hpp"
#include "module.hpp"
#include "nvrtc.hpp"
#include "types.h"

#define JS_CU_CONSTANT(name) Nan::Set(target, JS_STR( #name ), JS_INT(CU_ ## name))
#define JS_CUDA_CONSTANT(name) Nan::Set(target, JS_STR( #name ), JS_INT(CUDA_ ## name))
#define JS_CUDA_ERROR(name) Nan::Set(target, JS_STR( #name ), Nan::Error(JS_STR(NodeCuda::getExceptionMessage(CUDA_ ## name))) )

#define NODE_DEFINE_CONSTANT_VALUE(target, name, value)          \
  Nan::Set(target, Nan::New<v8::String>(name).ToLocalChecked(),  \
                   Nan::New<v8::Integer>((unsigned int)value))

using namespace NodeCuda;

namespace NodeCuda {

  static NAN_GETTER(GetDriverVersion);
  static NAN_GETTER(GetDeviceCount);

}

NAN_MODULE_INIT(init) {
  // Initiailze the cuda driver api
  cuInit(0);

  // These methods don't need instances
  Nan::SetAccessor(target, Nan::New("driverVersion").ToLocalChecked(), GetDriverVersion);
  Nan::SetAccessor(target, Nan::New("deviceCount").ToLocalChecked(), GetDeviceCount);

  // init types
  Types::init(target);

  // Initialize driver api bindings
  Ctx::Initialize(target);
  Device::Initialize(target);
  Stream::Initialize(target);
  NodeCuda::Function::Initialize(target);
  Mem::Initialize(target);
  NodeCuda::Module::Initialize(target);
  NodeCuda::Nvrtc::Initialize(target);

  /**
   * Platform-dependent byte sizes
   */
  NODE_DEFINE_CONSTANT_VALUE(target, "size_CHAR", sizeof(char));
  NODE_DEFINE_CONSTANT_VALUE(target, "size_SHORT", sizeof(short));
  NODE_DEFINE_CONSTANT_VALUE(target, "size_INT", sizeof(int));
  NODE_DEFINE_CONSTANT_VALUE(target, "size_LONG", sizeof(long));
  NODE_DEFINE_CONSTANT_VALUE(target, "size_FLOAT", sizeof(float));
  NODE_DEFINE_CONSTANT_VALUE(target, "size_DOUBLE", sizeof(double));
  NODE_DEFINE_CONSTANT_VALUE(target, "size_HALF", sizeof(float) >> 1);

  JS_CUDA_CONSTANT(SUCCESS);
  JS_CUDA_ERROR(ERROR_INVALID_VALUE);
  JS_CUDA_ERROR(ERROR_OUT_OF_MEMORY);
  JS_CUDA_ERROR(ERROR_NOT_INITIALIZED);
  JS_CUDA_ERROR(ERROR_DEINITIALIZED);
  JS_CUDA_ERROR(ERROR_PROFILER_DISABLED);
  JS_CUDA_ERROR(ERROR_PROFILER_NOT_INITIALIZED);
  JS_CUDA_ERROR(ERROR_PROFILER_ALREADY_STARTED);
  JS_CUDA_ERROR(ERROR_PROFILER_ALREADY_STOPPED);
  JS_CUDA_ERROR(ERROR_NO_DEVICE);
  JS_CUDA_ERROR(ERROR_INVALID_DEVICE);
  JS_CUDA_ERROR(ERROR_INVALID_IMAGE);
  JS_CUDA_ERROR(ERROR_INVALID_CONTEXT);
  JS_CUDA_ERROR(ERROR_CONTEXT_ALREADY_CURRENT);
  JS_CUDA_ERROR(ERROR_MAP_FAILED);
  JS_CUDA_ERROR(ERROR_UNMAP_FAILED);
  JS_CUDA_ERROR(ERROR_ARRAY_IS_MAPPED);
  JS_CUDA_ERROR(ERROR_ALREADY_MAPPED);
  JS_CUDA_ERROR(ERROR_NO_BINARY_FOR_GPU);
  JS_CUDA_ERROR(ERROR_ALREADY_ACQUIRED);
  JS_CUDA_ERROR(ERROR_NOT_MAPPED);
  JS_CUDA_ERROR(ERROR_NOT_MAPPED_AS_ARRAY);
  JS_CUDA_ERROR(ERROR_NOT_MAPPED_AS_POINTER);
  JS_CUDA_ERROR(ERROR_ECC_UNCORRECTABLE);
  JS_CUDA_ERROR(ERROR_UNSUPPORTED_LIMIT);
  JS_CUDA_ERROR(ERROR_CONTEXT_ALREADY_IN_USE);
  JS_CUDA_ERROR(ERROR_PEER_ACCESS_UNSUPPORTED);
  JS_CUDA_ERROR(ERROR_INVALID_PTX);
  JS_CUDA_ERROR(ERROR_INVALID_GRAPHICS_CONTEXT);
  JS_CUDA_ERROR(ERROR_NVLINK_UNCORRECTABLE);
  JS_CUDA_ERROR(ERROR_JIT_COMPILER_NOT_FOUND);
  JS_CUDA_ERROR(ERROR_INVALID_SOURCE);
  JS_CUDA_ERROR(ERROR_FILE_NOT_FOUND);
  JS_CUDA_ERROR(ERROR_SHARED_OBJECT_SYMBOL_NOT_FOUND);
  JS_CUDA_ERROR(ERROR_SHARED_OBJECT_INIT_FAILED);
  JS_CUDA_ERROR(ERROR_OPERATING_SYSTEM);
  JS_CUDA_ERROR(ERROR_INVALID_HANDLE);
  JS_CUDA_ERROR(ERROR_NOT_FOUND);
#if __CUDA_API_VERSION >= 10010
  JS_CUDA_ERROR(ERROR_ILLEGAL_STATE);
#endif
  JS_CUDA_ERROR(ERROR_NOT_READY);
  JS_CUDA_ERROR(ERROR_ILLEGAL_ADDRESS);
  JS_CUDA_ERROR(ERROR_LAUNCH_OUT_OF_RESOURCES);
  JS_CUDA_ERROR(ERROR_LAUNCH_TIMEOUT);
  JS_CUDA_ERROR(ERROR_LAUNCH_INCOMPATIBLE_TEXTURING);
  JS_CUDA_ERROR(ERROR_PEER_ACCESS_ALREADY_ENABLED);
  JS_CUDA_ERROR(ERROR_PEER_ACCESS_NOT_ENABLED);
  JS_CUDA_ERROR(ERROR_PRIMARY_CONTEXT_ACTIVE);
  JS_CUDA_ERROR(ERROR_CONTEXT_IS_DESTROYED);
  JS_CUDA_ERROR(ERROR_ASSERT);
  JS_CUDA_ERROR(ERROR_TOO_MANY_PEERS);
  JS_CUDA_ERROR(ERROR_HOST_MEMORY_ALREADY_REGISTERED);
  JS_CUDA_ERROR(ERROR_HOST_MEMORY_NOT_REGISTERED);
  JS_CUDA_ERROR(ERROR_HARDWARE_STACK_ERROR);
  JS_CUDA_ERROR(ERROR_ILLEGAL_INSTRUCTION);
  JS_CUDA_ERROR(ERROR_MISALIGNED_ADDRESS);
  JS_CUDA_ERROR(ERROR_INVALID_ADDRESS_SPACE);
  JS_CUDA_ERROR(ERROR_INVALID_PC);
  JS_CUDA_ERROR(ERROR_LAUNCH_FAILED);
  JS_CUDA_ERROR(ERROR_COOPERATIVE_LAUNCH_TOO_LARGE);
  JS_CUDA_ERROR(ERROR_NOT_PERMITTED);
  JS_CUDA_ERROR(ERROR_NOT_SUPPORTED);
#if __CUDA_API_VERSION >= 10010
  JS_CUDA_ERROR(ERROR_SYSTEM_NOT_READY);
  JS_CUDA_ERROR(ERROR_SYSTEM_DRIVER_MISMATCH);
  JS_CUDA_ERROR(ERROR_COMPAT_NOT_SUPPORTED_ON_DEVICE);
  JS_CUDA_ERROR(ERROR_STREAM_CAPTURE_UNSUPPORTED);
  JS_CUDA_ERROR(ERROR_STREAM_CAPTURE_INVALIDATED);
  JS_CUDA_ERROR(ERROR_STREAM_CAPTURE_MERGE);
  JS_CUDA_ERROR(ERROR_STREAM_CAPTURE_UNMATCHED);
  JS_CUDA_ERROR(ERROR_STREAM_CAPTURE_UNJOINED);
  JS_CUDA_ERROR(ERROR_STREAM_CAPTURE_ISOLATION);
  JS_CUDA_ERROR(ERROR_STREAM_CAPTURE_IMPLICIT);
  JS_CUDA_ERROR(ERROR_CAPTURED_EVENT);
  JS_CUDA_ERROR(ERROR_STREAM_CAPTURE_WRONG_THREAD);
#endif
  JS_CUDA_ERROR(ERROR_UNKNOWN);

  JS_CU_CONSTANT(IPC_MEM_LAZY_ENABLE_PEER_ACCESS);
  JS_CU_CONSTANT(MEM_ATTACH_GLOBAL);
  JS_CU_CONSTANT(MEM_ATTACH_HOST);
  JS_CU_CONSTANT(MEM_ATTACH_SINGLE);
  JS_CU_CONSTANT(CTX_SCHED_AUTO);
  JS_CU_CONSTANT(CTX_SCHED_SPIN);
  JS_CU_CONSTANT(CTX_SCHED_YIELD);
  JS_CU_CONSTANT(CTX_SCHED_BLOCKING_SYNC);
  JS_CU_CONSTANT(CTX_BLOCKING_SYNC);
  JS_CU_CONSTANT(CTX_SCHED_MASK);
  JS_CU_CONSTANT(CTX_MAP_HOST);
  JS_CU_CONSTANT(CTX_LMEM_RESIZE_TO_MAX);
  JS_CU_CONSTANT(CTX_FLAGS_MASK);
  JS_CU_CONSTANT(STREAM_DEFAULT);
  JS_CU_CONSTANT(STREAM_NON_BLOCKING);
  JS_CU_CONSTANT(EVENT_DEFAULT);
  JS_CU_CONSTANT(EVENT_BLOCKING_SYNC);
  JS_CU_CONSTANT(EVENT_DISABLE_TIMING);
  JS_CU_CONSTANT(EVENT_INTERPROCESS);
  JS_CU_CONSTANT(STREAM_WAIT_VALUE_GEQ);
  JS_CU_CONSTANT(STREAM_WAIT_VALUE_EQ);
  JS_CU_CONSTANT(STREAM_WAIT_VALUE_AND);
  JS_CU_CONSTANT(STREAM_WAIT_VALUE_NOR);
  JS_CU_CONSTANT(STREAM_WAIT_VALUE_FLUSH);
  JS_CU_CONSTANT(STREAM_WRITE_VALUE_DEFAULT);
  JS_CU_CONSTANT(STREAM_WRITE_VALUE_NO_MEMORY_BARRIER);
  JS_CU_CONSTANT(STREAM_MEM_OP_WAIT_VALUE_32);
  JS_CU_CONSTANT(STREAM_MEM_OP_WRITE_VALUE_32);
  JS_CU_CONSTANT(STREAM_MEM_OP_WAIT_VALUE_64);
  JS_CU_CONSTANT(STREAM_MEM_OP_WRITE_VALUE_64);
  JS_CU_CONSTANT(STREAM_MEM_OP_FLUSH_REMOTE_WRITES);
  JS_CU_CONSTANT(OCCUPANCY_DEFAULT);
  JS_CU_CONSTANT(OCCUPANCY_DISABLE_CACHING_OVERRIDE);
  JS_CU_CONSTANT(AD_FORMAT_UNSIGNED_INT8);
  JS_CU_CONSTANT(AD_FORMAT_UNSIGNED_INT16);
  JS_CU_CONSTANT(AD_FORMAT_UNSIGNED_INT32);
  JS_CU_CONSTANT(AD_FORMAT_SIGNED_INT8);
  JS_CU_CONSTANT(AD_FORMAT_SIGNED_INT16);
  JS_CU_CONSTANT(AD_FORMAT_SIGNED_INT32);
  JS_CU_CONSTANT(AD_FORMAT_HALF);
  JS_CU_CONSTANT(AD_FORMAT_FLOAT);
  JS_CU_CONSTANT(TR_ADDRESS_MODE_WRAP);
  JS_CU_CONSTANT(TR_ADDRESS_MODE_CLAMP);
  JS_CU_CONSTANT(TR_ADDRESS_MODE_MIRROR);
  JS_CU_CONSTANT(TR_ADDRESS_MODE_BORDER);
  JS_CU_CONSTANT(TR_FILTER_MODE_POINT);
  JS_CU_CONSTANT(TR_FILTER_MODE_LINEAR);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAX_THREADS_PER_BLOCK);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_X);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_Y);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_Z);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAX_GRID_DIM_X);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAX_GRID_DIM_Y);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAX_GRID_DIM_Z);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAX_SHARED_MEMORY_PER_BLOCK);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_SHARED_MEMORY_PER_BLOCK);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_TOTAL_CONSTANT_MEMORY);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_WARP_SIZE);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAX_PITCH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAX_REGISTERS_PER_BLOCK);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_REGISTERS_PER_BLOCK);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_CLOCK_RATE);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_TEXTURE_ALIGNMENT);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_GPU_OVERLAP);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_KERNEL_EXEC_TIMEOUT);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_INTEGRATED);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_CAN_MAP_HOST_MEMORY);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_COMPUTE_MODE);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE1D_WIDTH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_WIDTH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_HEIGHT);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE3D_WIDTH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE3D_HEIGHT);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE3D_DEPTH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_LAYERED_WIDTH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_LAYERED_HEIGHT);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_LAYERED_LAYERS);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_ARRAY_WIDTH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_ARRAY_HEIGHT);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_ARRAY_NUMSLICES);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_SURFACE_ALIGNMENT);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_CONCURRENT_KERNELS);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_ECC_ENABLED);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_PCI_BUS_ID);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_PCI_DEVICE_ID);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_TCC_DRIVER);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_GLOBAL_MEMORY_BUS_WIDTH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_L2_CACHE_SIZE);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAX_THREADS_PER_MULTIPROCESSOR);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_ASYNC_ENGINE_COUNT);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_UNIFIED_ADDRESSING);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE1D_LAYERED_WIDTH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE1D_LAYERED_LAYERS);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_CAN_TEX2D_GATHER);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_GATHER_WIDTH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_GATHER_HEIGHT);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE3D_WIDTH_ALTERNATE);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE3D_HEIGHT_ALTERNATE);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE3D_DEPTH_ALTERNATE);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_PCI_DOMAIN_ID);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_TEXTURE_PITCH_ALIGNMENT);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURECUBEMAP_WIDTH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURECUBEMAP_LAYERED_WIDTH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURECUBEMAP_LAYERED_LAYERS);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_SURFACE1D_WIDTH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_SURFACE2D_WIDTH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_SURFACE2D_HEIGHT);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_SURFACE3D_WIDTH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_SURFACE3D_HEIGHT);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_SURFACE3D_DEPTH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_SURFACE1D_LAYERED_WIDTH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_SURFACE1D_LAYERED_LAYERS);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_SURFACE2D_LAYERED_WIDTH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_SURFACE2D_LAYERED_HEIGHT);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_SURFACE2D_LAYERED_LAYERS);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_SURFACECUBEMAP_WIDTH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_SURFACECUBEMAP_LAYERED_WIDTH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_SURFACECUBEMAP_LAYERED_LAYERS);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE1D_LINEAR_WIDTH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_LINEAR_WIDTH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_LINEAR_HEIGHT);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_LINEAR_PITCH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_MIPMAPPED_WIDTH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_MIPMAPPED_HEIGHT);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MAJOR);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MINOR);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE1D_MIPMAPPED_WIDTH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_STREAM_PRIORITIES_SUPPORTED);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_GLOBAL_L1_CACHE_SUPPORTED);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_LOCAL_L1_CACHE_SUPPORTED);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAX_SHARED_MEMORY_PER_MULTIPROCESSOR);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAX_REGISTERS_PER_MULTIPROCESSOR);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MANAGED_MEMORY);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MULTI_GPU_BOARD);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MULTI_GPU_BOARD_GROUP_ID);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_HOST_NATIVE_ATOMIC_SUPPORTED);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_SINGLE_TO_DOUBLE_PRECISION_PERF_RATIO);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_PAGEABLE_MEMORY_ACCESS);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_CONCURRENT_MANAGED_ACCESS);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_COMPUTE_PREEMPTION_SUPPORTED);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_CAN_USE_HOST_POINTER_FOR_REGISTERED_MEM);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_CAN_USE_STREAM_MEM_OPS);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_CAN_USE_64_BIT_STREAM_MEM_OPS);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_CAN_USE_STREAM_WAIT_VALUE_NOR);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_COOPERATIVE_LAUNCH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_COOPERATIVE_MULTI_DEVICE_LAUNCH);
  JS_CU_CONSTANT(DEVICE_ATTRIBUTE_MAX_SHARED_MEMORY_PER_BLOCK_OPTIN);
  JS_CU_CONSTANT(POINTER_ATTRIBUTE_CONTEXT);
  JS_CU_CONSTANT(POINTER_ATTRIBUTE_MEMORY_TYPE);
  JS_CU_CONSTANT(POINTER_ATTRIBUTE_DEVICE_POINTER);
  JS_CU_CONSTANT(POINTER_ATTRIBUTE_HOST_POINTER);
  JS_CU_CONSTANT(POINTER_ATTRIBUTE_P2P_TOKENS);
  JS_CU_CONSTANT(POINTER_ATTRIBUTE_SYNC_MEMOPS);
  JS_CU_CONSTANT(POINTER_ATTRIBUTE_BUFFER_ID);
  JS_CU_CONSTANT(POINTER_ATTRIBUTE_IS_MANAGED);
  JS_CU_CONSTANT(FUNC_ATTRIBUTE_MAX_THREADS_PER_BLOCK);
  JS_CU_CONSTANT(FUNC_ATTRIBUTE_SHARED_SIZE_BYTES);
  JS_CU_CONSTANT(FUNC_ATTRIBUTE_CONST_SIZE_BYTES);
  JS_CU_CONSTANT(FUNC_ATTRIBUTE_LOCAL_SIZE_BYTES);
  JS_CU_CONSTANT(FUNC_ATTRIBUTE_NUM_REGS);
  JS_CU_CONSTANT(FUNC_ATTRIBUTE_PTX_VERSION);
  JS_CU_CONSTANT(FUNC_ATTRIBUTE_BINARY_VERSION);
  JS_CU_CONSTANT(FUNC_ATTRIBUTE_CACHE_MODE_CA);
  JS_CU_CONSTANT(FUNC_ATTRIBUTE_MAX_DYNAMIC_SHARED_SIZE_BYTES);
  JS_CU_CONSTANT(FUNC_ATTRIBUTE_PREFERRED_SHARED_MEMORY_CARVEOUT);
  JS_CU_CONSTANT(FUNC_CACHE_PREFER_NONE);
  JS_CU_CONSTANT(FUNC_CACHE_PREFER_SHARED);
  JS_CU_CONSTANT(FUNC_CACHE_PREFER_L1);
  JS_CU_CONSTANT(FUNC_CACHE_PREFER_EQUAL);
  JS_CU_CONSTANT(SHARED_MEM_CONFIG_DEFAULT_BANK_SIZE);
  JS_CU_CONSTANT(SHARED_MEM_CONFIG_FOUR_BYTE_BANK_SIZE);
  JS_CU_CONSTANT(SHARED_MEM_CONFIG_EIGHT_BYTE_BANK_SIZE);
  JS_CU_CONSTANT(SHAREDMEM_CARVEOUT_DEFAULT);
  JS_CU_CONSTANT(SHAREDMEM_CARVEOUT_MAX_SHARED);
  JS_CU_CONSTANT(SHAREDMEM_CARVEOUT_MAX_L1);
  JS_CU_CONSTANT(MEMORYTYPE_HOST);
  JS_CU_CONSTANT(MEMORYTYPE_DEVICE);
  JS_CU_CONSTANT(MEMORYTYPE_ARRAY);
  JS_CU_CONSTANT(MEMORYTYPE_UNIFIED);
  JS_CU_CONSTANT(COMPUTEMODE_DEFAULT);
  JS_CU_CONSTANT(COMPUTEMODE_PROHIBITED);
  JS_CU_CONSTANT(COMPUTEMODE_EXCLUSIVE_PROCESS);
  JS_CU_CONSTANT(MEM_ADVISE_SET_READ_MOSTLY);
  JS_CU_CONSTANT(MEM_ADVISE_UNSET_READ_MOSTLY);
  JS_CU_CONSTANT(MEM_ADVISE_SET_PREFERRED_LOCATION);
  JS_CU_CONSTANT(MEM_ADVISE_UNSET_PREFERRED_LOCATION);
  JS_CU_CONSTANT(MEM_ADVISE_SET_ACCESSED_BY);
  JS_CU_CONSTANT(MEM_ADVISE_UNSET_ACCESSED_BY);
  JS_CU_CONSTANT(MEM_RANGE_ATTRIBUTE_READ_MOSTLY);
  JS_CU_CONSTANT(MEM_RANGE_ATTRIBUTE_PREFERRED_LOCATION);
  JS_CU_CONSTANT(MEM_RANGE_ATTRIBUTE_ACCESSED_BY);
  JS_CU_CONSTANT(MEM_RANGE_ATTRIBUTE_LAST_PREFETCH_LOCATION);
  JS_CU_CONSTANT(JIT_MAX_REGISTERS);
  JS_CU_CONSTANT(JIT_THREADS_PER_BLOCK);
  JS_CU_CONSTANT(JIT_WALL_TIME);
  JS_CU_CONSTANT(JIT_INFO_LOG_BUFFER);
  JS_CU_CONSTANT(JIT_INFO_LOG_BUFFER_SIZE_BYTES);
  JS_CU_CONSTANT(JIT_ERROR_LOG_BUFFER);
  JS_CU_CONSTANT(JIT_ERROR_LOG_BUFFER_SIZE_BYTES);
  JS_CU_CONSTANT(JIT_OPTIMIZATION_LEVEL);
  JS_CU_CONSTANT(JIT_TARGET_FROM_CUCONTEXT);
  JS_CU_CONSTANT(JIT_TARGET);
  JS_CU_CONSTANT(JIT_FALLBACK_STRATEGY);
  JS_CU_CONSTANT(JIT_GENERATE_DEBUG_INFO);
  JS_CU_CONSTANT(JIT_LOG_VERBOSE);
  JS_CU_CONSTANT(JIT_GENERATE_LINE_INFO);
  JS_CU_CONSTANT(JIT_CACHE_MODE);
  JS_CU_CONSTANT(JIT_NEW_SM3X_OPT);
  JS_CU_CONSTANT(JIT_FAST_COMPILE);
  JS_CU_CONSTANT(JIT_NUM_OPTIONS);
  JS_CU_CONSTANT(JIT_CACHE_OPTION_NONE);
  JS_CU_CONSTANT(JIT_CACHE_OPTION_CG);
  JS_CU_CONSTANT(JIT_CACHE_OPTION_CA);
  JS_CU_CONSTANT(JIT_INPUT_CUBIN);
  JS_CU_CONSTANT(JIT_INPUT_PTX);
  JS_CU_CONSTANT(JIT_INPUT_FATBINARY);
  JS_CU_CONSTANT(JIT_INPUT_OBJECT);
  JS_CU_CONSTANT(JIT_INPUT_LIBRARY);
  JS_CU_CONSTANT(JIT_NUM_INPUT_TYPES);
  JS_CU_CONSTANT(TARGET_COMPUTE_20);
  JS_CU_CONSTANT(TARGET_COMPUTE_21);
  JS_CU_CONSTANT(TARGET_COMPUTE_30);
  JS_CU_CONSTANT(TARGET_COMPUTE_32);
  JS_CU_CONSTANT(TARGET_COMPUTE_35);
  JS_CU_CONSTANT(TARGET_COMPUTE_37);
  JS_CU_CONSTANT(TARGET_COMPUTE_50);
  JS_CU_CONSTANT(TARGET_COMPUTE_52);
  JS_CU_CONSTANT(TARGET_COMPUTE_53);
  JS_CU_CONSTANT(TARGET_COMPUTE_60);
  JS_CU_CONSTANT(TARGET_COMPUTE_61);
  JS_CU_CONSTANT(TARGET_COMPUTE_62);
  JS_CU_CONSTANT(TARGET_COMPUTE_70);
  JS_CU_CONSTANT(PREFER_PTX);
  JS_CU_CONSTANT(JIT_CACHE_OPTION_NONE);
  JS_CU_CONSTANT(JIT_INPUT_CUBIN);
  JS_CU_CONSTANT(GRAPHICS_REGISTER_FLAGS_NONE);
  JS_CU_CONSTANT(GRAPHICS_REGISTER_FLAGS_READ_ONLY);
  JS_CU_CONSTANT(GRAPHICS_REGISTER_FLAGS_WRITE_DISCARD);
  JS_CU_CONSTANT(GRAPHICS_REGISTER_FLAGS_SURFACE_LDST);
  JS_CU_CONSTANT(GRAPHICS_REGISTER_FLAGS_TEXTURE_GATHER);
  JS_CU_CONSTANT(GRAPHICS_MAP_RESOURCE_FLAGS_NONE);
  JS_CU_CONSTANT(GRAPHICS_MAP_RESOURCE_FLAGS_READ_ONLY);
  JS_CU_CONSTANT(GRAPHICS_MAP_RESOURCE_FLAGS_WRITE_DISCARD);
  JS_CU_CONSTANT(CUBEMAP_FACE_POSITIVE_X);
  JS_CU_CONSTANT(CUBEMAP_FACE_NEGATIVE_X);
  JS_CU_CONSTANT(CUBEMAP_FACE_POSITIVE_Y);
  JS_CU_CONSTANT(CUBEMAP_FACE_NEGATIVE_Y);
  JS_CU_CONSTANT(CUBEMAP_FACE_POSITIVE_Z);
  JS_CU_CONSTANT(CUBEMAP_FACE_NEGATIVE_Z);
  JS_CU_CONSTANT(LIMIT_STACK_SIZE);
  JS_CU_CONSTANT(LIMIT_PRINTF_FIFO_SIZE);
  JS_CU_CONSTANT(LIMIT_MALLOC_HEAP_SIZE);
  JS_CU_CONSTANT(LIMIT_DEV_RUNTIME_SYNC_DEPTH);
  JS_CU_CONSTANT(LIMIT_DEV_RUNTIME_PENDING_LAUNCH_COUNT);
  JS_CU_CONSTANT(RESOURCE_TYPE_ARRAY);
  JS_CU_CONSTANT(RESOURCE_TYPE_MIPMAPPED_ARRAY);
  JS_CU_CONSTANT(RESOURCE_TYPE_LINEAR);
  JS_CU_CONSTANT(RESOURCE_TYPE_PITCH2D);
  JS_CU_CONSTANT(DEVICE_P2P_ATTRIBUTE_PERFORMANCE_RANK);
  JS_CU_CONSTANT(DEVICE_P2P_ATTRIBUTE_ACCESS_SUPPORTED);
  JS_CU_CONSTANT(DEVICE_P2P_ATTRIBUTE_NATIVE_ATOMIC_SUPPORTED);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_NONE);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_UINT_1X8);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_UINT_2X8);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_UINT_4X8);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_SINT_1X8);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_SINT_2X8);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_SINT_4X8);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_UINT_1X16);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_UINT_2X16);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_UINT_4X16);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_SINT_1X16);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_SINT_2X16);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_SINT_4X16);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_UINT_1X32);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_UINT_2X32);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_UINT_4X32);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_SINT_1X32);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_SINT_2X32);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_SINT_4X32);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_FLOAT_1X16);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_FLOAT_2X16);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_FLOAT_4X16);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_FLOAT_1X32);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_FLOAT_2X32);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_FLOAT_4X32);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_UNSIGNED_BC1);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_UNSIGNED_BC2);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_UNSIGNED_BC3);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_UNSIGNED_BC4);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_SIGNED_BC4);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_UNSIGNED_BC5);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_SIGNED_BC5);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_UNSIGNED_BC6H);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_SIGNED_BC6H);
  JS_CU_CONSTANT(RES_VIEW_FORMAT_UNSIGNED_BC7);
  JS_CU_CONSTANT(MEMORYTYPE_HOST);
  JS_CU_CONSTANT(MEMORYTYPE_DEVICE);
  JS_CU_CONSTANT(MEMORYTYPE_ARRAY);
  JS_CU_CONSTANT(MEMORYTYPE_UNIFIED);
  JS_CU_CONSTANT(MEMORYTYPE_HOST);
  JS_CU_CONSTANT(MEMORYTYPE_DEVICE);
  JS_CU_CONSTANT(MEMORYTYPE_ARRAY);
  JS_CU_CONSTANT(MEMORYTYPE_UNIFIED);
  JS_CU_CONSTANT(MEMORYTYPE_HOST);
  JS_CU_CONSTANT(MEMORYTYPE_DEVICE);
  JS_CU_CONSTANT(MEMORYTYPE_ARRAY);
  JS_CU_CONSTANT(MEMORYTYPE_UNIFIED);
  JS_CU_CONSTANT(MEMORYTYPE_HOST);
  JS_CU_CONSTANT(MEMORYTYPE_DEVICE);
  JS_CU_CONSTANT(MEMORYTYPE_ARRAY);
  JS_CU_CONSTANT(MEMORYTYPE_UNIFIED);
  JS_CU_CONSTANT(MEMORYTYPE_HOST);
  JS_CU_CONSTANT(MEMORYTYPE_DEVICE);
  JS_CU_CONSTANT(MEMORYTYPE_ARRAY);
  JS_CU_CONSTANT(MEMORYTYPE_UNIFIED);
  JS_CU_CONSTANT(AD_FORMAT_UNSIGNED_INT8);
  JS_CU_CONSTANT(AD_FORMAT_UNSIGNED_INT16);
  JS_CU_CONSTANT(AD_FORMAT_UNSIGNED_INT32);
  JS_CU_CONSTANT(AD_FORMAT_SIGNED_INT8);
  JS_CU_CONSTANT(AD_FORMAT_SIGNED_INT16);
  JS_CU_CONSTANT(AD_FORMAT_SIGNED_INT32);
  JS_CU_CONSTANT(AD_FORMAT_HALF);
  JS_CU_CONSTANT(AD_FORMAT_FLOAT);
  JS_CU_CONSTANT(AD_FORMAT_UNSIGNED_INT8);
  JS_CU_CONSTANT(AD_FORMAT_UNSIGNED_INT16);
  JS_CU_CONSTANT(AD_FORMAT_UNSIGNED_INT32);
  JS_CU_CONSTANT(AD_FORMAT_SIGNED_INT8);
  JS_CU_CONSTANT(AD_FORMAT_SIGNED_INT16);
  JS_CU_CONSTANT(AD_FORMAT_SIGNED_INT32);
  JS_CU_CONSTANT(AD_FORMAT_HALF);
  JS_CU_CONSTANT(AD_FORMAT_FLOAT);
  JS_CU_CONSTANT(AD_FORMAT_UNSIGNED_INT8);
  JS_CU_CONSTANT(AD_FORMAT_UNSIGNED_INT16);
  JS_CU_CONSTANT(AD_FORMAT_UNSIGNED_INT32);
  JS_CU_CONSTANT(AD_FORMAT_SIGNED_INT8);
  JS_CU_CONSTANT(AD_FORMAT_SIGNED_INT16);
  JS_CU_CONSTANT(AD_FORMAT_SIGNED_INT32);
  JS_CU_CONSTANT(AD_FORMAT_HALF);
  JS_CU_CONSTANT(AD_FORMAT_FLOAT);
  JS_CU_CONSTANT(TR_ADDRESS_MODE_WRAP);
  JS_CU_CONSTANT(TR_ADDRESS_MODE_CLAMP);
  JS_CU_CONSTANT(TR_ADDRESS_MODE_MIRROR);
  JS_CU_CONSTANT(TR_ADDRESS_MODE_BORDER);
  JS_CU_CONSTANT(TR_FILTER_MODE_POINT);
  JS_CU_CONSTANT(TR_FILTER_MODE_LINEAR);
  JS_CU_CONSTANT(TR_FILTER_MODE_POINT);
  JS_CU_CONSTANT(TR_FILTER_MODE_LINEAR);
  JS_CU_CONSTANT(RESOURCE_TYPE_ARRAY);
  JS_CU_CONSTANT(RESOURCE_TYPE_MIPMAPPED_ARRAY);
  JS_CU_CONSTANT(RESOURCE_TYPE_LINEAR);
  JS_CU_CONSTANT(RESOURCE_TYPE_PITCH2D);
  JS_CU_CONSTANT(TR_ADDRESS_MODE_WRAP);
  JS_CU_CONSTANT(TR_ADDRESS_MODE_CLAMP);
  JS_CU_CONSTANT(TR_ADDRESS_MODE_MIRROR);
  JS_CU_CONSTANT(TR_ADDRESS_MODE_BORDER);
  JS_CU_CONSTANT(TR_FILTER_MODE_POINT);
  JS_CU_CONSTANT(TR_FILTER_MODE_LINEAR);
}

NAN_GETTER(NodeCuda::GetDriverVersion) {
  int driverVersion = 0;
  cuDriverGetVersion(&driverVersion);
  info.GetReturnValue().Set(Nan::New<Integer>(driverVersion));
}

NAN_GETTER(NodeCuda::GetDeviceCount) {
  int count = 0;
  cuDeviceGetCount(&count);
  info.GetReturnValue().Set(Nan::New<Integer>(count));
}

NODE_MODULE(cuda, init);

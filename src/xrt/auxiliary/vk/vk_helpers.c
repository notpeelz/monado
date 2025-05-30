// Copyright 2019-2024, Collabora, Ltd.
// SPDX-License-Identifier: BSL-1.0
/*!
 * @file
 * @brief  Common Vulkan code.
 *
 * Note that some sections of this are generated
 * by `scripts/generate_vk_helpers.py` - lists of functions
 * and of optional extensions to check for. In those,
 * please update the script and run it, instead of editing
 * directly in this file. The generated parts are delimited
 * by special comments.
 *
 * @author Jakob Bornecrantz <jakob@collabora.com>
 * @author Lubosz Sarnecki <lubosz.sarnecki@collabora.com>
 * @author Moshi Turner <moshiturner@protonmail.com>
 * @author Korcan Hussein <korcan.hussein@collabora.com>
 * @ingroup aux_vk
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/u_debug.h"
#include "util/u_handles.h"
#include "util/u_misc.h"

#include "vk/vk_helpers.h"

#include <xrt/xrt_handles.h>


/*
 *
 * Small internal helpers.
 *
 */

#define CHAIN(STRUCT, NEXT)                                                                                            \
	do {                                                                                                           \
		(STRUCT).pNext = NEXT;                                                                                 \
		NEXT = (VkBaseInStructure *)&(STRUCT);                                                                 \
	} while (false)


/*
 *
 * String helper functions.
 *
 */

#define ENUM_TO_STR(r)                                                                                                 \
	case r: return #r

XRT_CHECK_RESULT const char *
vk_result_string(VkResult code)
{
	switch (code) {
		ENUM_TO_STR(VK_SUCCESS);
		ENUM_TO_STR(VK_NOT_READY);
		ENUM_TO_STR(VK_TIMEOUT);
		ENUM_TO_STR(VK_EVENT_SET);
		ENUM_TO_STR(VK_EVENT_RESET);
		ENUM_TO_STR(VK_INCOMPLETE);
		ENUM_TO_STR(VK_ERROR_OUT_OF_HOST_MEMORY);
		ENUM_TO_STR(VK_ERROR_OUT_OF_DEVICE_MEMORY);
		ENUM_TO_STR(VK_ERROR_INITIALIZATION_FAILED);
		ENUM_TO_STR(VK_ERROR_DEVICE_LOST);
		ENUM_TO_STR(VK_ERROR_MEMORY_MAP_FAILED);
		ENUM_TO_STR(VK_ERROR_LAYER_NOT_PRESENT);
		ENUM_TO_STR(VK_ERROR_EXTENSION_NOT_PRESENT);
		ENUM_TO_STR(VK_ERROR_FEATURE_NOT_PRESENT);
		ENUM_TO_STR(VK_ERROR_INCOMPATIBLE_DRIVER);
		ENUM_TO_STR(VK_ERROR_TOO_MANY_OBJECTS);
		ENUM_TO_STR(VK_ERROR_FORMAT_NOT_SUPPORTED);
		ENUM_TO_STR(VK_ERROR_FRAGMENTED_POOL);
#ifdef VK_VERSION_1_1
		ENUM_TO_STR(VK_ERROR_OUT_OF_POOL_MEMORY);
		ENUM_TO_STR(VK_ERROR_INVALID_EXTERNAL_HANDLE);
#endif
#ifdef VK_VERSION_1_2
		ENUM_TO_STR(VK_ERROR_UNKNOWN); // Only defined in 1.2 and above headers.
		ENUM_TO_STR(VK_ERROR_FRAGMENTATION);
		ENUM_TO_STR(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
#endif
#ifdef VK_VERSION_1_3
		ENUM_TO_STR(VK_PIPELINE_COMPILE_REQUIRED);
#endif
#ifdef VK_KHR_surface
		ENUM_TO_STR(VK_ERROR_SURFACE_LOST_KHR);
		ENUM_TO_STR(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
#endif
#ifdef VK_KHR_swapchain
		ENUM_TO_STR(VK_SUBOPTIMAL_KHR);
		ENUM_TO_STR(VK_ERROR_OUT_OF_DATE_KHR);
#endif
#ifdef VK_KHR_display_swapchain
		ENUM_TO_STR(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
#endif
#ifdef VK_EXT_debug_report
		ENUM_TO_STR(VK_ERROR_VALIDATION_FAILED_EXT);
#endif
#ifdef VK_NV_glsl_shader
		ENUM_TO_STR(VK_ERROR_INVALID_SHADER_NV);
#endif
#if defined(VK_ENABLE_BETA_EXTENSIONS) && defined(VK_KHR_video_queue)
		ENUM_TO_STR(VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR);
		ENUM_TO_STR(VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR);
		ENUM_TO_STR(VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR);
		ENUM_TO_STR(VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR);
		ENUM_TO_STR(VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR);
		ENUM_TO_STR(VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR);
#endif
#ifdef VK_EXT_image_drm_format_modifier
		ENUM_TO_STR(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
#endif
#ifdef VK_KHR_global_priority
		ENUM_TO_STR(VK_ERROR_NOT_PERMITTED_KHR);
#endif
#ifdef VK_EXT_full_screen_exclusive
		ENUM_TO_STR(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
#endif
#ifdef VK_KHR_deferred_host_operations
		ENUM_TO_STR(VK_THREAD_IDLE_KHR);
#endif
#ifdef VK_KHR_deferred_host_operations
		ENUM_TO_STR(VK_THREAD_DONE_KHR);
#endif
#ifdef VK_KHR_deferred_host_operations
		ENUM_TO_STR(VK_OPERATION_DEFERRED_KHR);
#endif
#ifdef VK_KHR_deferred_host_operations
		ENUM_TO_STR(VK_OPERATION_NOT_DEFERRED_KHR);
#endif
#ifdef VK_EXT_image_compression_control
		ENUM_TO_STR(VK_ERROR_COMPRESSION_EXHAUSTED_EXT);
#endif
#if defined(VK_KHR_maintenance1) && !defined(VK_VERSION_1_1)
		ENUM_TO_STR(VK_ERROR_OUT_OF_POOL_MEMORY_KHR);
#endif
#if defined(VK_KHR_external_memory) && !defined(VK_VERSION_1_1)
		ENUM_TO_STR(VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR);
#endif
#if defined(VK_EXT_descriptor_indexing) && !defined(VK_VERSION_1_2)
		ENUM_TO_STR(VK_ERROR_FRAGMENTATION_EXT);
#endif
#if defined(VK_EXT_global_priority) && !defined(VK_KHR_global_priority)
		ENUM_TO_STR(VK_ERROR_NOT_PERMITTED_EXT);
#endif
#if defined(VK_EXT_buffer_device_address) && !defined(VK_VERSION_1_2)
		ENUM_TO_STR(VK_ERROR_INVALID_DEVICE_ADDRESS_EXT);
		// VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR = VK_ERROR_INVALID_DEVICE_ADDRESS_EXT
#endif
#if defined(VK_EXT_pipeline_creation_cache_control) && !defined(VK_VERSION_1_3)
		ENUM_TO_STR(VK_PIPELINE_COMPILE_REQUIRED_EXT);
		// VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT = VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT
#endif
	default: return "UNKNOWN RESULT";
	}
}

XRT_CHECK_RESULT const char *
vk_object_type_string(VkObjectType type)
{
	switch (type) {
		ENUM_TO_STR(VK_OBJECT_TYPE_UNKNOWN);
		ENUM_TO_STR(VK_OBJECT_TYPE_INSTANCE);
		ENUM_TO_STR(VK_OBJECT_TYPE_PHYSICAL_DEVICE);
		ENUM_TO_STR(VK_OBJECT_TYPE_DEVICE);
		ENUM_TO_STR(VK_OBJECT_TYPE_QUEUE);
		ENUM_TO_STR(VK_OBJECT_TYPE_SEMAPHORE);
		ENUM_TO_STR(VK_OBJECT_TYPE_COMMAND_BUFFER);
		ENUM_TO_STR(VK_OBJECT_TYPE_FENCE);
		ENUM_TO_STR(VK_OBJECT_TYPE_DEVICE_MEMORY);
		ENUM_TO_STR(VK_OBJECT_TYPE_BUFFER);
		ENUM_TO_STR(VK_OBJECT_TYPE_IMAGE);
		ENUM_TO_STR(VK_OBJECT_TYPE_EVENT);
		ENUM_TO_STR(VK_OBJECT_TYPE_QUERY_POOL);
		ENUM_TO_STR(VK_OBJECT_TYPE_BUFFER_VIEW);
		ENUM_TO_STR(VK_OBJECT_TYPE_IMAGE_VIEW);
		ENUM_TO_STR(VK_OBJECT_TYPE_SHADER_MODULE);
		ENUM_TO_STR(VK_OBJECT_TYPE_PIPELINE_CACHE);
		ENUM_TO_STR(VK_OBJECT_TYPE_PIPELINE_LAYOUT);
		ENUM_TO_STR(VK_OBJECT_TYPE_RENDER_PASS);
		ENUM_TO_STR(VK_OBJECT_TYPE_PIPELINE);
		ENUM_TO_STR(VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT);
		ENUM_TO_STR(VK_OBJECT_TYPE_SAMPLER);
		ENUM_TO_STR(VK_OBJECT_TYPE_DESCRIPTOR_POOL);
		ENUM_TO_STR(VK_OBJECT_TYPE_DESCRIPTOR_SET);
		ENUM_TO_STR(VK_OBJECT_TYPE_FRAMEBUFFER);
		ENUM_TO_STR(VK_OBJECT_TYPE_COMMAND_POOL);
#ifdef VK_VERSION_1_1
		ENUM_TO_STR(VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE);
#elif defined(VK_KHR_descriptor_update_template)
		ENUM_TO_STR(VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_KHR);
#endif
#ifdef VK_VERSION_1_1
		ENUM_TO_STR(VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION);
#elif defined(VK_KHR_sampler_ycbcr_conversion)
		ENUM_TO_STR(VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION_KHR);
#endif
#ifdef VK_VERSION_1_3
		ENUM_TO_STR(VK_OBJECT_TYPE_PRIVATE_DATA_SLOT);
#elif defined(VK_EXT_private_data)
		ENUM_TO_STR(VK_OBJECT_TYPE_PRIVATE_DATA_SLOT_EXT);
#endif
#ifdef VK_KHR_surface
		ENUM_TO_STR(VK_OBJECT_TYPE_SURFACE_KHR);
#endif
#ifdef VK_KHR_swapchain
		ENUM_TO_STR(VK_OBJECT_TYPE_SWAPCHAIN_KHR);
#endif
#ifdef VK_KHR_display
		ENUM_TO_STR(VK_OBJECT_TYPE_DISPLAY_KHR);
		ENUM_TO_STR(VK_OBJECT_TYPE_DISPLAY_MODE_KHR);
#endif
#ifdef VK_EXT_debug_report
		ENUM_TO_STR(VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT);
#endif
#ifdef VK_KHR_video_queue
		ENUM_TO_STR(VK_OBJECT_TYPE_VIDEO_SESSION_KHR);
		ENUM_TO_STR(VK_OBJECT_TYPE_VIDEO_SESSION_PARAMETERS_KHR);
#endif
#ifdef VK_NVX_binary_import
		ENUM_TO_STR(VK_OBJECT_TYPE_CU_MODULE_NVX);
		ENUM_TO_STR(VK_OBJECT_TYPE_CU_FUNCTION_NVX);
#endif
#ifdef VK_EXT_debug_utils
		ENUM_TO_STR(VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT);
#endif
#ifdef VK_KHR_acceleration_structure
		ENUM_TO_STR(VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR);
#endif
#ifdef VK_EXT_validation_cache
		ENUM_TO_STR(VK_OBJECT_TYPE_VALIDATION_CACHE_EXT);
#endif
#ifdef VK_NV_ray_tracing
		ENUM_TO_STR(VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_NV);
#endif
#ifdef VK_INTEL_performance_query
		ENUM_TO_STR(VK_OBJECT_TYPE_PERFORMANCE_CONFIGURATION_INTEL);
#endif
#ifdef VK_KHR_deferred_host_operations
		ENUM_TO_STR(VK_OBJECT_TYPE_DEFERRED_OPERATION_KHR);
#endif
#ifdef VK_NV_device_generated_commands
		ENUM_TO_STR(VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NV);
#endif
#ifdef VK_NV_cuda_kernel_launch
		ENUM_TO_STR(VK_OBJECT_TYPE_CUDA_MODULE_NV);
		ENUM_TO_STR(VK_OBJECT_TYPE_CUDA_FUNCTION_NV);
#endif
#ifdef VK_FUCHSIA_buffer_collection
		ENUM_TO_STR(VK_OBJECT_TYPE_BUFFER_COLLECTION_FUCHSIA);
#endif
#ifdef VK_EXT_opacity_micromap
		ENUM_TO_STR(VK_OBJECT_TYPE_MICROMAP_EXT);
#endif
#ifdef VK_NV_optical_flow
		ENUM_TO_STR(VK_OBJECT_TYPE_OPTICAL_FLOW_SESSION_NV);
#endif
#ifdef VK_EXT_shader_object
		ENUM_TO_STR(VK_OBJECT_TYPE_SHADER_EXT);
#endif
	default: return "UNKNOWN OBJECT TYPE";
	}
}

XRT_CHECK_RESULT const char *
vk_physical_device_type_string(VkPhysicalDeviceType device_type)
{
	switch (device_type) {
		ENUM_TO_STR(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
		ENUM_TO_STR(VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
		ENUM_TO_STR(VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU);
		ENUM_TO_STR(VK_PHYSICAL_DEVICE_TYPE_CPU);
	default: return "UNKNOWN PHYSICAL DEVICE TYPE";
	}
}

XRT_CHECK_RESULT const char *
vk_format_string(VkFormat code)
{
	switch (code) {
		ENUM_TO_STR(VK_FORMAT_UNDEFINED);
		ENUM_TO_STR(VK_FORMAT_R4G4_UNORM_PACK8);
		ENUM_TO_STR(VK_FORMAT_R4G4B4A4_UNORM_PACK16);
		ENUM_TO_STR(VK_FORMAT_B4G4R4A4_UNORM_PACK16);
		ENUM_TO_STR(VK_FORMAT_R5G6B5_UNORM_PACK16);
		ENUM_TO_STR(VK_FORMAT_B5G6R5_UNORM_PACK16);
		ENUM_TO_STR(VK_FORMAT_R5G5B5A1_UNORM_PACK16);
		ENUM_TO_STR(VK_FORMAT_B5G5R5A1_UNORM_PACK16);
		ENUM_TO_STR(VK_FORMAT_A1R5G5B5_UNORM_PACK16);
		ENUM_TO_STR(VK_FORMAT_R8_UNORM);
		ENUM_TO_STR(VK_FORMAT_R8_SNORM);
		ENUM_TO_STR(VK_FORMAT_R8_USCALED);
		ENUM_TO_STR(VK_FORMAT_R8_SSCALED);
		ENUM_TO_STR(VK_FORMAT_R8_UINT);
		ENUM_TO_STR(VK_FORMAT_R8_SINT);
		ENUM_TO_STR(VK_FORMAT_R8_SRGB);
		ENUM_TO_STR(VK_FORMAT_R8G8_UNORM);
		ENUM_TO_STR(VK_FORMAT_R8G8_SNORM);
		ENUM_TO_STR(VK_FORMAT_R8G8_USCALED);
		ENUM_TO_STR(VK_FORMAT_R8G8_SSCALED);
		ENUM_TO_STR(VK_FORMAT_R8G8_UINT);
		ENUM_TO_STR(VK_FORMAT_R8G8_SINT);
		ENUM_TO_STR(VK_FORMAT_R8G8_SRGB);
		ENUM_TO_STR(VK_FORMAT_R8G8B8_UNORM);
		ENUM_TO_STR(VK_FORMAT_R8G8B8_SNORM);
		ENUM_TO_STR(VK_FORMAT_R8G8B8_USCALED);
		ENUM_TO_STR(VK_FORMAT_R8G8B8_SSCALED);
		ENUM_TO_STR(VK_FORMAT_R8G8B8_UINT);
		ENUM_TO_STR(VK_FORMAT_R8G8B8_SINT);
		ENUM_TO_STR(VK_FORMAT_R8G8B8_SRGB);
		ENUM_TO_STR(VK_FORMAT_B8G8R8_UNORM);
		ENUM_TO_STR(VK_FORMAT_B8G8R8_SNORM);
		ENUM_TO_STR(VK_FORMAT_B8G8R8_USCALED);
		ENUM_TO_STR(VK_FORMAT_B8G8R8_SSCALED);
		ENUM_TO_STR(VK_FORMAT_B8G8R8_UINT);
		ENUM_TO_STR(VK_FORMAT_B8G8R8_SINT);
		ENUM_TO_STR(VK_FORMAT_B8G8R8_SRGB);
		ENUM_TO_STR(VK_FORMAT_R8G8B8A8_UNORM);
		ENUM_TO_STR(VK_FORMAT_R8G8B8A8_SNORM);
		ENUM_TO_STR(VK_FORMAT_R8G8B8A8_USCALED);
		ENUM_TO_STR(VK_FORMAT_R8G8B8A8_SSCALED);
		ENUM_TO_STR(VK_FORMAT_R8G8B8A8_UINT);
		ENUM_TO_STR(VK_FORMAT_R8G8B8A8_SINT);
		ENUM_TO_STR(VK_FORMAT_R8G8B8A8_SRGB);
		ENUM_TO_STR(VK_FORMAT_B8G8R8A8_UNORM);
		ENUM_TO_STR(VK_FORMAT_B8G8R8A8_SNORM);
		ENUM_TO_STR(VK_FORMAT_B8G8R8A8_USCALED);
		ENUM_TO_STR(VK_FORMAT_B8G8R8A8_SSCALED);
		ENUM_TO_STR(VK_FORMAT_B8G8R8A8_UINT);
		ENUM_TO_STR(VK_FORMAT_B8G8R8A8_SINT);
		ENUM_TO_STR(VK_FORMAT_B8G8R8A8_SRGB);
		ENUM_TO_STR(VK_FORMAT_A8B8G8R8_UNORM_PACK32);
		ENUM_TO_STR(VK_FORMAT_A8B8G8R8_SNORM_PACK32);
		ENUM_TO_STR(VK_FORMAT_A8B8G8R8_USCALED_PACK32);
		ENUM_TO_STR(VK_FORMAT_A8B8G8R8_SSCALED_PACK32);
		ENUM_TO_STR(VK_FORMAT_A8B8G8R8_UINT_PACK32);
		ENUM_TO_STR(VK_FORMAT_A8B8G8R8_SINT_PACK32);
		ENUM_TO_STR(VK_FORMAT_A8B8G8R8_SRGB_PACK32);
		ENUM_TO_STR(VK_FORMAT_A2R10G10B10_UNORM_PACK32);
		ENUM_TO_STR(VK_FORMAT_A2R10G10B10_SNORM_PACK32);
		ENUM_TO_STR(VK_FORMAT_A2R10G10B10_USCALED_PACK32);
		ENUM_TO_STR(VK_FORMAT_A2R10G10B10_SSCALED_PACK32);
		ENUM_TO_STR(VK_FORMAT_A2R10G10B10_UINT_PACK32);
		ENUM_TO_STR(VK_FORMAT_A2R10G10B10_SINT_PACK32);
		ENUM_TO_STR(VK_FORMAT_A2B10G10R10_UNORM_PACK32);
		ENUM_TO_STR(VK_FORMAT_A2B10G10R10_SNORM_PACK32);
		ENUM_TO_STR(VK_FORMAT_A2B10G10R10_USCALED_PACK32);
		ENUM_TO_STR(VK_FORMAT_A2B10G10R10_SSCALED_PACK32);
		ENUM_TO_STR(VK_FORMAT_A2B10G10R10_UINT_PACK32);
		ENUM_TO_STR(VK_FORMAT_A2B10G10R10_SINT_PACK32);
		ENUM_TO_STR(VK_FORMAT_R16_UNORM);
		ENUM_TO_STR(VK_FORMAT_R16_SNORM);
		ENUM_TO_STR(VK_FORMAT_R16_USCALED);
		ENUM_TO_STR(VK_FORMAT_R16_SSCALED);
		ENUM_TO_STR(VK_FORMAT_R16_UINT);
		ENUM_TO_STR(VK_FORMAT_R16_SINT);
		ENUM_TO_STR(VK_FORMAT_R16_SFLOAT);
		ENUM_TO_STR(VK_FORMAT_R16G16_UNORM);
		ENUM_TO_STR(VK_FORMAT_R16G16_SNORM);
		ENUM_TO_STR(VK_FORMAT_R16G16_USCALED);
		ENUM_TO_STR(VK_FORMAT_R16G16_SSCALED);
		ENUM_TO_STR(VK_FORMAT_R16G16_UINT);
		ENUM_TO_STR(VK_FORMAT_R16G16_SINT);
		ENUM_TO_STR(VK_FORMAT_R16G16_SFLOAT);
		ENUM_TO_STR(VK_FORMAT_R16G16B16_UNORM);
		ENUM_TO_STR(VK_FORMAT_R16G16B16_SNORM);
		ENUM_TO_STR(VK_FORMAT_R16G16B16_USCALED);
		ENUM_TO_STR(VK_FORMAT_R16G16B16_SSCALED);
		ENUM_TO_STR(VK_FORMAT_R16G16B16_UINT);
		ENUM_TO_STR(VK_FORMAT_R16G16B16_SINT);
		ENUM_TO_STR(VK_FORMAT_R16G16B16_SFLOAT);
		ENUM_TO_STR(VK_FORMAT_R16G16B16A16_UNORM);
		ENUM_TO_STR(VK_FORMAT_R16G16B16A16_SNORM);
		ENUM_TO_STR(VK_FORMAT_R16G16B16A16_USCALED);
		ENUM_TO_STR(VK_FORMAT_R16G16B16A16_SSCALED);
		ENUM_TO_STR(VK_FORMAT_R16G16B16A16_UINT);
		ENUM_TO_STR(VK_FORMAT_R16G16B16A16_SINT);
		ENUM_TO_STR(VK_FORMAT_R16G16B16A16_SFLOAT);
		ENUM_TO_STR(VK_FORMAT_R32_UINT);
		ENUM_TO_STR(VK_FORMAT_R32_SINT);
		ENUM_TO_STR(VK_FORMAT_R32_SFLOAT);
		ENUM_TO_STR(VK_FORMAT_R32G32_UINT);
		ENUM_TO_STR(VK_FORMAT_R32G32_SINT);
		ENUM_TO_STR(VK_FORMAT_R32G32_SFLOAT);
		ENUM_TO_STR(VK_FORMAT_R32G32B32_UINT);
		ENUM_TO_STR(VK_FORMAT_R32G32B32_SINT);
		ENUM_TO_STR(VK_FORMAT_R32G32B32_SFLOAT);
		ENUM_TO_STR(VK_FORMAT_R32G32B32A32_UINT);
		ENUM_TO_STR(VK_FORMAT_R32G32B32A32_SINT);
		ENUM_TO_STR(VK_FORMAT_R32G32B32A32_SFLOAT);
		ENUM_TO_STR(VK_FORMAT_R64_UINT);
		ENUM_TO_STR(VK_FORMAT_R64_SINT);
		ENUM_TO_STR(VK_FORMAT_R64_SFLOAT);
		ENUM_TO_STR(VK_FORMAT_R64G64_UINT);
		ENUM_TO_STR(VK_FORMAT_R64G64_SINT);
		ENUM_TO_STR(VK_FORMAT_R64G64_SFLOAT);
		ENUM_TO_STR(VK_FORMAT_R64G64B64_UINT);
		ENUM_TO_STR(VK_FORMAT_R64G64B64_SINT);
		ENUM_TO_STR(VK_FORMAT_R64G64B64_SFLOAT);
		ENUM_TO_STR(VK_FORMAT_R64G64B64A64_UINT);
		ENUM_TO_STR(VK_FORMAT_R64G64B64A64_SINT);
		ENUM_TO_STR(VK_FORMAT_R64G64B64A64_SFLOAT);
		ENUM_TO_STR(VK_FORMAT_B10G11R11_UFLOAT_PACK32);
		ENUM_TO_STR(VK_FORMAT_E5B9G9R9_UFLOAT_PACK32);
		ENUM_TO_STR(VK_FORMAT_D16_UNORM);
		ENUM_TO_STR(VK_FORMAT_X8_D24_UNORM_PACK32);
		ENUM_TO_STR(VK_FORMAT_D32_SFLOAT);
		ENUM_TO_STR(VK_FORMAT_S8_UINT);
		ENUM_TO_STR(VK_FORMAT_D16_UNORM_S8_UINT);
		ENUM_TO_STR(VK_FORMAT_D24_UNORM_S8_UINT);
		ENUM_TO_STR(VK_FORMAT_D32_SFLOAT_S8_UINT);
		ENUM_TO_STR(VK_FORMAT_BC1_RGB_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_BC1_RGB_SRGB_BLOCK);
		ENUM_TO_STR(VK_FORMAT_BC1_RGBA_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_BC1_RGBA_SRGB_BLOCK);
		ENUM_TO_STR(VK_FORMAT_BC2_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_BC2_SRGB_BLOCK);
		ENUM_TO_STR(VK_FORMAT_BC3_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_BC3_SRGB_BLOCK);
		ENUM_TO_STR(VK_FORMAT_BC4_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_BC4_SNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_BC5_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_BC5_SNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_BC6H_UFLOAT_BLOCK);
		ENUM_TO_STR(VK_FORMAT_BC6H_SFLOAT_BLOCK);
		ENUM_TO_STR(VK_FORMAT_BC7_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_BC7_SRGB_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK);
		ENUM_TO_STR(VK_FORMAT_EAC_R11_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_EAC_R11_SNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_EAC_R11G11_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_EAC_R11G11_SNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_4x4_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_4x4_SRGB_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_5x4_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_5x4_SRGB_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_5x5_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_5x5_SRGB_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_6x5_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_6x5_SRGB_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_6x6_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_6x6_SRGB_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_8x5_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_8x5_SRGB_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_8x6_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_8x6_SRGB_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_8x8_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_8x8_SRGB_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_10x5_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_10x5_SRGB_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_10x6_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_10x6_SRGB_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_10x8_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_10x8_SRGB_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_10x10_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_10x10_SRGB_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_12x10_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_12x10_SRGB_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_12x12_UNORM_BLOCK);
		ENUM_TO_STR(VK_FORMAT_ASTC_12x12_SRGB_BLOCK);
#ifdef VK_VERSION_1_1
		ENUM_TO_STR(VK_FORMAT_G8B8G8R8_422_UNORM);
		ENUM_TO_STR(VK_FORMAT_B8G8R8G8_422_UNORM);
		ENUM_TO_STR(VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM);
		ENUM_TO_STR(VK_FORMAT_G8_B8R8_2PLANE_420_UNORM);
		ENUM_TO_STR(VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM);
		ENUM_TO_STR(VK_FORMAT_G8_B8R8_2PLANE_422_UNORM);
		ENUM_TO_STR(VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM);
		ENUM_TO_STR(VK_FORMAT_R10X6_UNORM_PACK16);
		ENUM_TO_STR(VK_FORMAT_R10X6G10X6_UNORM_2PACK16);
		ENUM_TO_STR(VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16);
		ENUM_TO_STR(VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16);
		ENUM_TO_STR(VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16);
		ENUM_TO_STR(VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16);
		ENUM_TO_STR(VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16);
		ENUM_TO_STR(VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16);
		ENUM_TO_STR(VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16);
		ENUM_TO_STR(VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16);
		ENUM_TO_STR(VK_FORMAT_R12X4_UNORM_PACK16);
		ENUM_TO_STR(VK_FORMAT_R12X4G12X4_UNORM_2PACK16);
		ENUM_TO_STR(VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16);
		ENUM_TO_STR(VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16);
		ENUM_TO_STR(VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16);
		ENUM_TO_STR(VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16);
		ENUM_TO_STR(VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16);
		ENUM_TO_STR(VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16);
		ENUM_TO_STR(VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16);
		ENUM_TO_STR(VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16);
		ENUM_TO_STR(VK_FORMAT_G16B16G16R16_422_UNORM);
		ENUM_TO_STR(VK_FORMAT_B16G16R16G16_422_UNORM);
		ENUM_TO_STR(VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM);
		ENUM_TO_STR(VK_FORMAT_G16_B16R16_2PLANE_420_UNORM);
		ENUM_TO_STR(VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM);
		ENUM_TO_STR(VK_FORMAT_G16_B16R16_2PLANE_422_UNORM);
		ENUM_TO_STR(VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM);
#endif // VK_VERSION_1_1
#ifdef VK_IMG_format_pvrtc
		ENUM_TO_STR(VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG);
		ENUM_TO_STR(VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG);
		ENUM_TO_STR(VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG);
		ENUM_TO_STR(VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG);
		ENUM_TO_STR(VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG);
		ENUM_TO_STR(VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG);
		ENUM_TO_STR(VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG);
		ENUM_TO_STR(VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG);
#endif // VK_IMG_format_pvrtc
#ifdef VK_EXT_texture_compression_astc_hdr
		ENUM_TO_STR(VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT);
		ENUM_TO_STR(VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT);
		ENUM_TO_STR(VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT);
		ENUM_TO_STR(VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT);
		ENUM_TO_STR(VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT);
		ENUM_TO_STR(VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT);
		ENUM_TO_STR(VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT);
		ENUM_TO_STR(VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT);
		ENUM_TO_STR(VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT);
		ENUM_TO_STR(VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT);
		ENUM_TO_STR(VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT);
		ENUM_TO_STR(VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT);
		ENUM_TO_STR(VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT);
		ENUM_TO_STR(VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT);
#endif // VK_EXT_texture_compression_astc_hdr
#ifdef VK_EXT_4444_formats
		ENUM_TO_STR(VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT);
		ENUM_TO_STR(VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT);
#endif // VK_EXT_4444_formats
	default: return "UNKNOWN FORMAT";
	}
}

XRT_CHECK_RESULT const char *
vk_sharing_mode_string(VkSharingMode code)
{
	switch (code) {
		ENUM_TO_STR(VK_SHARING_MODE_EXCLUSIVE);
		ENUM_TO_STR(VK_SHARING_MODE_CONCURRENT);
	default: return "UNKNOWN SHARING MODE";
	}
}

XRT_CHECK_RESULT const char *
vk_present_mode_string(VkPresentModeKHR code)
{
	switch (code) {
		ENUM_TO_STR(VK_PRESENT_MODE_FIFO_KHR);
		ENUM_TO_STR(VK_PRESENT_MODE_MAILBOX_KHR);
		ENUM_TO_STR(VK_PRESENT_MODE_IMMEDIATE_KHR);
		ENUM_TO_STR(VK_PRESENT_MODE_FIFO_RELAXED_KHR);
		ENUM_TO_STR(VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR);
		ENUM_TO_STR(VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR);
	default: return "UNKNOWN MODE";
	}
}

XRT_CHECK_RESULT const char *
vk_color_space_string(VkColorSpaceKHR code)
{
	switch (code) {
		ENUM_TO_STR(VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
#ifdef VK_EXT_swapchain_colorspace
		ENUM_TO_STR(VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT);
		ENUM_TO_STR(VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT);
		ENUM_TO_STR(VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT);
		ENUM_TO_STR(VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT);
		ENUM_TO_STR(VK_COLOR_SPACE_BT709_LINEAR_EXT);
		ENUM_TO_STR(VK_COLOR_SPACE_BT709_NONLINEAR_EXT);
		ENUM_TO_STR(VK_COLOR_SPACE_BT2020_LINEAR_EXT);
		ENUM_TO_STR(VK_COLOR_SPACE_HDR10_ST2084_EXT);
		ENUM_TO_STR(VK_COLOR_SPACE_DOLBYVISION_EXT);
		ENUM_TO_STR(VK_COLOR_SPACE_HDR10_HLG_EXT);
		ENUM_TO_STR(VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT);
		ENUM_TO_STR(VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT);
		ENUM_TO_STR(VK_COLOR_SPACE_PASS_THROUGH_EXT);
		ENUM_TO_STR(VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT);
#endif
#ifdef VK_AMD_display_native_hdr
		ENUM_TO_STR(VK_COLOR_SPACE_DISPLAY_NATIVE_AMD);
#endif
	default: return "UNKNOWN COLOR SPACE";
	}
}

XRT_CHECK_RESULT const char *
vk_power_state_string(VkDisplayPowerStateEXT code)
{
	switch (code) {
		ENUM_TO_STR(VK_DISPLAY_POWER_STATE_OFF_EXT);
		ENUM_TO_STR(VK_DISPLAY_POWER_STATE_SUSPEND_EXT);
		ENUM_TO_STR(VK_DISPLAY_POWER_STATE_ON_EXT);
	default: return "UNKNOWN MODE";
	}
}

XRT_CHECK_RESULT const char *
vk_format_feature_flag_string(VkFormatFeatureFlagBits bits, bool null_on_unknown)
{
	switch (bits) {
		ENUM_TO_STR(VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
		ENUM_TO_STR(VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT);
		ENUM_TO_STR(VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
		ENUM_TO_STR(VK_FORMAT_FEATURE_TRANSFER_SRC_BIT);
		ENUM_TO_STR(VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
		ENUM_TO_STR(VK_FORMAT_R5G6B5_UNORM_PACK16);
	default:
		if (bits == 0) {
			return "FORMAT FEATURE: NO BITS SET";
		} else if (bits & (bits - 1)) {
			return "FORMAT FEATURE: MULTIPLE BITS SET";
		} else {
			return null_on_unknown ? NULL : "FORMAT FEATURE: UNKNOWN BIT";
		}
	}
}

XRT_CHECK_RESULT const char *
vk_image_usage_flag_string(VkImageUsageFlagBits bits, bool null_on_unknown)
{
	switch (bits) {
		ENUM_TO_STR(VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
		ENUM_TO_STR(VK_IMAGE_USAGE_TRANSFER_DST_BIT);
		ENUM_TO_STR(VK_IMAGE_USAGE_SAMPLED_BIT);
		ENUM_TO_STR(VK_IMAGE_USAGE_STORAGE_BIT);
		ENUM_TO_STR(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
		ENUM_TO_STR(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
		ENUM_TO_STR(VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT);
		ENUM_TO_STR(VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
#ifdef VK_KHR_video_decode_queue
		ENUM_TO_STR(VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR);
		ENUM_TO_STR(VK_IMAGE_USAGE_VIDEO_DECODE_SRC_BIT_KHR);
		ENUM_TO_STR(VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR);
#endif
#ifdef VK_EXT_fragment_density_map
		ENUM_TO_STR(VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT);
#endif
#ifdef VK_KHR_fragment_shading_rate
		ENUM_TO_STR(VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR);
#endif
#ifdef VK_EXT_host_image_copy
		ENUM_TO_STR(VK_IMAGE_USAGE_HOST_TRANSFER_BIT_EXT);
#endif
#ifdef VK_EXT_attachment_feedback_loop_layout
		ENUM_TO_STR(VK_IMAGE_USAGE_ATTACHMENT_FEEDBACK_LOOP_BIT_EXT);
#endif
#ifdef VK_HUAWEI_invocation_mask
		ENUM_TO_STR(VK_IMAGE_USAGE_INVOCATION_MASK_BIT_HUAWEI);
#endif
#ifdef VK_QCOM_image_processing
		ENUM_TO_STR(VK_IMAGE_USAGE_SAMPLE_WEIGHT_BIT_QCOM);
#endif
#ifdef VK_QCOM_image_processing
		ENUM_TO_STR(VK_IMAGE_USAGE_SAMPLE_BLOCK_MATCH_BIT_QCOM);
#endif
#if defined(VK_NV_shading_rate_image) && !defined(VK_KHR_fragment_shading_rate)
		ENUM_TO_STR(VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV);
#endif
	default:
		if (bits == 0) {
			return "IMAGE USAGE: NO BITS SET";
		} else if (bits & (bits - 1)) {
			return "IMAGE USAGE: MULTIPLE BITS SET";
		} else {
			return null_on_unknown ? NULL : "IMAGE USAGE: UNKNOWN BIT";
		}
	}
}

XRT_CHECK_RESULT const char *
vk_composite_alpha_flag_string(VkCompositeAlphaFlagBitsKHR bits, bool null_on_unknown)
{
	switch (bits) {
		ENUM_TO_STR(VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR);
		ENUM_TO_STR(VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR);
		ENUM_TO_STR(VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR);
		ENUM_TO_STR(VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR);
	default:
		if (bits == 0) {
			return "COMPOSITE ALPHA: NO BITS SET";
		} else if (bits & (bits - 1)) {
			return "COMPOSITE ALPHA: MULTIPLE BITS SET";
		} else {
			return null_on_unknown ? NULL : "COMPOSITE ALPHA: UNKNOWN BIT";
		}
	}
}

XRT_CHECK_RESULT const char *
vk_surface_transform_flag_string(VkSurfaceTransformFlagBitsKHR bits, bool null_on_unknown)
{
	switch (bits) {
		ENUM_TO_STR(VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR);
		ENUM_TO_STR(VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR);
		ENUM_TO_STR(VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR);
		ENUM_TO_STR(VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR);
		ENUM_TO_STR(VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR);
		ENUM_TO_STR(VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR);
		ENUM_TO_STR(VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR);
		ENUM_TO_STR(VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR);
		ENUM_TO_STR(VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR);
	default:
		if (bits == 0) {
			return "SURFACE TRANSFORM: NO BITS SET";
		} else if (bits & (bits - 1)) {
			return "SURFACE TRANSFORM: MULTIPLE BITS SET";
		} else {
			return null_on_unknown ? NULL : "SURFACE TRANSFORM: UNKNOWN BIT";
		}
	}
}

#ifdef VK_KHR_display
XRT_CHECK_RESULT const char *
vk_display_plane_alpha_flag_string(VkDisplayPlaneAlphaFlagBitsKHR bits, bool null_on_unknown)
{
	switch (bits) {
		ENUM_TO_STR(VK_DISPLAY_PLANE_ALPHA_OPAQUE_BIT_KHR);
		ENUM_TO_STR(VK_DISPLAY_PLANE_ALPHA_GLOBAL_BIT_KHR);
		ENUM_TO_STR(VK_DISPLAY_PLANE_ALPHA_PER_PIXEL_BIT_KHR);
		ENUM_TO_STR(VK_DISPLAY_PLANE_ALPHA_PER_PIXEL_PREMULTIPLIED_BIT_KHR);
	default:
		if (bits == 0) {
			return "DISPLAY PLANE ALPHA: NO BITS SET";
		} else if (bits & (bits - 1)) {
			return "DISPLAY PLANE ALPHA: MULTIPLE BITS SET";
		} else {
			return null_on_unknown ? NULL : "DISPLAY PLANE ALPHA: UNKNOWN BIT";
		}
	}
}
#endif

XRT_CHECK_RESULT const char *
xrt_swapchain_usage_flag_string(enum xrt_swapchain_usage_bits bits, bool null_on_unknown)
{
	switch (bits) {
		ENUM_TO_STR(XRT_SWAPCHAIN_USAGE_COLOR);
		ENUM_TO_STR(XRT_SWAPCHAIN_USAGE_DEPTH_STENCIL);
		ENUM_TO_STR(XRT_SWAPCHAIN_USAGE_UNORDERED_ACCESS);
		ENUM_TO_STR(XRT_SWAPCHAIN_USAGE_TRANSFER_SRC);
		ENUM_TO_STR(XRT_SWAPCHAIN_USAGE_TRANSFER_DST);
		ENUM_TO_STR(XRT_SWAPCHAIN_USAGE_SAMPLED);
		ENUM_TO_STR(XRT_SWAPCHAIN_USAGE_MUTABLE_FORMAT);
		ENUM_TO_STR(XRT_SWAPCHAIN_USAGE_INPUT_ATTACHMENT);
	default:
		if (bits == 0) {
			return "XRT SWAPCHAIN USAGE: NO BITS SET";
		} else if (bits & (bits - 1)) {
			return "XRT SWAPCHAIN USAGE: MULTIPLE BITS SET";
		} else {
			return null_on_unknown ? NULL : "XRT SWAPCHAIN USAGE: UNKNOWN BIT";
		}
	}
}


/*
 *
 * Functions.
 *
 */

bool
vk_get_memory_type(struct vk_bundle *vk, uint32_t type_bits, VkMemoryPropertyFlags memory_props, uint32_t *out_type_id)
{

	uint32_t i_supported = type_bits;
	for (uint32_t i = 0; i < vk->device_memory_props.memoryTypeCount; i++) {
		uint32_t propertyFlags = vk->device_memory_props.memoryTypes[i].propertyFlags;
		if ((i_supported & 1) == 1) {
			if ((propertyFlags & memory_props) == memory_props) {
				*out_type_id = i;
				return true;
			}
		}
		i_supported >>= 1;
	}

	VK_DEBUG(vk, "Could not find memory type!");

	VK_TRACE(vk, "Requested flags: %d (type bits %d with %d memory types)", memory_props, type_bits,
	         vk->device_memory_props.memoryTypeCount);

	i_supported = type_bits;
	VK_TRACE(vk, "Supported flags:");
	for (uint32_t i = 0; i < vk->device_memory_props.memoryTypeCount; i++) {
		uint32_t propertyFlags = vk->device_memory_props.memoryTypes[i].propertyFlags;
		if ((i_supported & 1) == 1) {
			VK_TRACE(vk, "    %d", propertyFlags);
		}
		i_supported >>= 1;
	}

	return false;
}

XRT_CHECK_RESULT VkResult
vk_alloc_and_bind_image_memory(struct vk_bundle *vk,
                               VkImage image,
                               const VkMemoryRequirements *requirements,
                               const void *pNext_for_allocate,
                               const char *caller_name,
                               VkDeviceMemory *out_mem)
{
	uint32_t memory_type_index = UINT32_MAX;
	bool bret = vk_get_memory_type(          //
	    vk,                                  // vk_bundle
	    requirements->memoryTypeBits,        // type_bits
	    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, // memory_props
	    &memory_type_index);                 // out_type_id
	if (!bret) {
		VK_ERROR(vk, "(%s) vk_get_memory_type: false\n\tFailed to find a matching memory type.", caller_name);
		return VK_ERROR_OUT_OF_DEVICE_MEMORY;
	}

	VkMemoryAllocateInfo alloc_info = {
	    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
	    .pNext = pNext_for_allocate,
	    .allocationSize = requirements->size,
	    .memoryTypeIndex = memory_type_index,
	};

	VkDeviceMemory device_memory = VK_NULL_HANDLE;
	VkResult ret = vk->vkAllocateMemory(vk->device, &alloc_info, NULL, &device_memory);
	if (ret != VK_SUCCESS) {
		VK_ERROR(vk, "(%s) vkAllocateMemory: %s", caller_name, vk_result_string(ret));
		return ret;
	}

	// Bind the memory to the image.
	ret = vk->vkBindImageMemory(vk->device, image, device_memory, 0);
	if (ret != VK_SUCCESS) {
		// Clean up memory
		vk->vkFreeMemory(vk->device, device_memory, NULL);
		VK_ERROR(vk, "(%s) vkBindImageMemory: %s", caller_name, vk_result_string(ret));
		return ret;
	}

	*out_mem = device_memory;
	return ret;
}

static VkResult
create_image_simple(struct vk_bundle *vk,
                    VkExtent2D extent,
                    VkFormat format,
                    VkImageCreateFlags create,
                    VkImageUsageFlags usage,
                    VkBaseInStructure *next_chain,
                    VkDeviceMemory *out_mem,
                    VkImage *out_image)
{
	VkImageCreateInfo image_info = {
	    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
	    .pNext = next_chain,
	    .imageType = VK_IMAGE_TYPE_2D,
	    .format = format,
	    .flags = create,
	    .extent =
	        {
	            .width = extent.width,
	            .height = extent.height,
	            .depth = 1,
	        },
	    .mipLevels = 1,
	    .arrayLayers = 1,
	    .samples = VK_SAMPLE_COUNT_1_BIT,
	    .tiling = VK_IMAGE_TILING_OPTIMAL,
	    .usage = usage,
	    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	    .queueFamilyIndexCount = 0,
	    .pQueueFamilyIndices = NULL,
	    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};

	VkImage image;
	VkResult ret = vk->vkCreateImage(vk->device, &image_info, NULL, &image);
	if (ret != VK_SUCCESS) {
		VK_ERROR(vk, "vkCreateImage: %s", vk_result_string(ret));
		// Nothing to cleanup
		return ret;
	}

	VkMemoryRequirements requirements = {0};
	vk->vkGetImageMemoryRequirements(vk->device, image, &requirements);

	ret = vk_alloc_and_bind_image_memory( //
	    vk,                               // vk_bundle
	    image,                            // image
	    &requirements,                    // max_size
	    NULL,                             // pNext_for_allocate
	    __func__,                         // caller_name
	    out_mem);                         // out_mem
	if (ret != VK_SUCCESS) {
		// Clean up image
		vk->vkDestroyImage(vk->device, image, NULL);
		return ret;
	}

	*out_image = image;

	return ret;
}

VkResult
vk_create_image_simple(struct vk_bundle *vk,
                       VkExtent2D extent,
                       VkFormat format,
                       VkImageUsageFlags usage,
                       VkDeviceMemory *out_mem,
                       VkImage *out_image)
{
	VkImageCreateFlags create = 0;

	return create_image_simple( //
	    vk,                     //
	    extent,                 // extent
	    format,                 // format
	    create,                 // create
	    usage,                  // usage
	    NULL,                   // next_chain
	    out_mem,                // out_mem
	    out_image);             // out_image
}

VkResult
vk_create_image_mutable_rgba(
    struct vk_bundle *vk, VkExtent2D extent, VkImageUsageFlags usage, VkDeviceMemory *out_mem, VkImage *out_image)
{
	VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
	VkImageCreateFlags create = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
	VkBaseInStructure *next_chain = NULL;

#ifdef VK_KHR_image_format_list
	VkFormat formats[2] = {
	    VK_FORMAT_R8G8B8A8_UNORM,
	    VK_FORMAT_R8G8B8A8_SRGB,
	};

	VkImageFormatListCreateInfoKHR image_format_list_create_info = {
	    .sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_LIST_CREATE_INFO_KHR,
	    .pNext = next_chain,
	    .viewFormatCount = ARRAY_SIZE(formats),
	    .pViewFormats = formats,
	};

	if (vk->has_KHR_image_format_list) {
		CHAIN(image_format_list_create_info, next_chain);
	}
#endif

	return create_image_simple( //
	    vk,                     //
	    extent,                 // extent
	    format,                 // format
	    create,                 // create
	    usage,                  // usage
	    next_chain,             // next_chain
	    out_mem,                // out_mem
	    out_image);             // out_image
}

VkResult
vk_create_image_advanced(struct vk_bundle *vk,
                         VkExtent3D extent,
                         VkFormat format,
                         VkImageTiling image_tiling,
                         VkImageUsageFlags image_usage_flags,
                         VkMemoryPropertyFlags memory_property_flags,
                         VkDeviceMemory *out_mem,
                         VkImage *out_image)
{
	VkResult ret = VK_SUCCESS;
	VkImage image = VK_NULL_HANDLE;
	VkDeviceMemory device_memory = VK_NULL_HANDLE;

	VkImageCreateInfo image_info = {
	    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
	    .imageType = extent.depth > 1 ? VK_IMAGE_TYPE_3D : VK_IMAGE_TYPE_2D,
	    .format = format,
	    .extent = extent,
	    .mipLevels = 1,
	    .arrayLayers = 1,
	    .samples = VK_SAMPLE_COUNT_1_BIT,
	    .tiling = image_tiling,
	    .usage = image_usage_flags,
	    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	    .queueFamilyIndexCount = 0,
	    .pQueueFamilyIndices = NULL,
	    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};


	ret = vk->vkCreateImage(vk->device, &image_info, NULL, &image);
	if (ret != VK_SUCCESS) {
		VK_ERROR(vk, "vkCreateImage: %s", vk_result_string(ret));
		// Nothing to cleanup
		return ret;
	}

	VkMemoryRequirements memory_requirements;
	vk->vkGetImageMemoryRequirements( //
	    vk->device,                   // device
	    image,                        // image
	    &memory_requirements);        // pMemoryRequirements

	uint32_t memory_type_index = UINT32_MAX;
	bool bret = vk_get_memory_type(         //
	    vk,                                 // vk_bundle
	    memory_requirements.memoryTypeBits, // type_bits
	    memory_property_flags,              // memory_props
	    &memory_type_index);                // out_type_id
	if (!bret) {
		VK_ERROR(vk, "vk_get_memory_type: false\n\tFailed to find a matching memory type.");
		ret = VK_ERROR_OUT_OF_DEVICE_MEMORY;
		goto err_image;
	}

	// vkAllocateMemory argument
	VkMemoryAllocateInfo memory_allocate_info = {
	    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
	    .allocationSize = memory_requirements.size,
	    .memoryTypeIndex = memory_type_index,
	};

	ret = vk->vkAllocateMemory( //
	    vk->device,             // device
	    &memory_allocate_info,  // pAllocateInfo
	    NULL,                   // pAllocator
	    &device_memory);        // pMemory
	if (ret != VK_SUCCESS) {
		VK_ERROR(vk, "vkAllocateMemory: %s", vk_result_string(ret));
		goto err_image;
	}

	ret = vk->vkBindImageMemory( //
	    vk->device,              // device
	    image,                   // image
	    device_memory,           // memory
	    0);                      // memoryOffset
	if (ret != VK_SUCCESS) {
		VK_ERROR(vk, "vkBindImageMemory: %s", vk_result_string(ret));
		goto err_memory;
	}

	*out_image = image;
	*out_mem = device_memory;

	return ret;

err_memory:
	vk->vkFreeMemory(vk->device, device_memory, NULL);
err_image:
	vk->vkDestroyImage(vk->device, image, NULL);

	return ret;
}

// - vk_csci_get_image_external_handle_type (usually but not always a constant)
// - vk_csci_get_image_external_support
//   - vkGetPhysicalDeviceImageFormatProperties2
// - vkCreateImage
// - vkGetImageMemoryRequirements
// - maybe vkGetAndroidHardwareBufferPropertiesANDROID
// - vk_alloc_and_bind_image_memory
XRT_CHECK_RESULT VkResult
vk_create_image_from_native(struct vk_bundle *vk,
                            const struct xrt_swapchain_create_info *info,
                            struct xrt_image_native *image_native,
                            VkImage *out_image,
                            VkDeviceMemory *out_mem)
{
	VkResult ret = VK_SUCCESS;

	// This is the format we allocate the image in, can be changed further down.
	VkFormat image_format = (VkFormat)info->format;

#ifdef XRT_GRAPHICS_BUFFER_HANDLE_IS_AHARDWAREBUFFER
	/*
	 * Some Vulkan drivers will natively support importing and exporting
	 * SRGB formats (Qualcomm Adreno) even though technically the
	 * AHardwareBuffer support for sRGB is... awkward (not inherent).
	 * While others (arm Mali) does not support importing and exporting
	 * sRGB formats. So we need to create the image without sRGB and
	 * then create the image views with sRGB which is allowed by the
	 * Vulkan spec. It seems to be safe to do with on all drivers,
	 * so to reduce the logic do that instead.
	 */
	if (image_format == VK_FORMAT_R8G8B8A8_SRGB) {
		image_format = VK_FORMAT_R8G8B8A8_UNORM;
	}
#endif

	VkImageUsageFlags image_usage = vk_csci_get_image_usage_flags( //
	    vk,                                                        //
	    image_format,                                              //
	    info->bits);                                               //
	if (image_usage == 0) {
		U_LOG_E("vk_create_image_from_native: Unsupported swapchain usage flags");
		return VK_ERROR_FEATURE_NOT_PRESENT;
	}

	VkExternalMemoryHandleTypeFlags handle_type = vk_csci_get_image_external_handle_type(vk, image_native);

	bool importable = false;
	vk_csci_get_image_external_support(vk, image_format, info->bits, handle_type, &importable, NULL);

	if (!importable) {
		VK_ERROR(vk, "External memory handle is not importable");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkImageCreateFlags image_create_flags = 0;
	// Set the image create mutable flag if usage mutable is given.
	const bool has_mutable_usage = (info->bits & XRT_SWAPCHAIN_USAGE_MUTABLE_FORMAT) != 0;
	if (has_mutable_usage) {
		image_create_flags |= VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
	}

	const bool has_create_protected_content = (info->create & XRT_SWAPCHAIN_CREATE_PROTECTED_CONTENT) != 0;
	if (has_create_protected_content) {
		image_create_flags |= VK_IMAGE_CREATE_PROTECTED_BIT;
	}

	// In->pNext
	VkExternalMemoryImageCreateInfoKHR external_memory_image_create_info = {
	    .sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO_KHR,
	    .handleTypes = handle_type,
	};

#ifdef VK_KHR_image_format_list
	VkImageFormatListCreateInfoKHR image_format_list_create_info = {
	    .sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_LIST_CREATE_INFO_KHR,
	    .pNext = NULL,
	    .viewFormatCount = info->format_count,
	    .pViewFormats = info->formats,
	};
	const bool has_mutable_format_list =
	    has_mutable_usage && vk->has_KHR_image_format_list && info->format_count > 0;
	if (has_mutable_format_list) {
		external_memory_image_create_info.pNext = &image_format_list_create_info;
	}
#endif

	// In
	VkImageCreateInfo vk_info = {
	    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
	    .pNext = &external_memory_image_create_info,
	    .flags = image_create_flags,
	    .imageType = VK_IMAGE_TYPE_2D,
	    .format = image_format,
	    .extent = {.width = info->width, .height = info->height, .depth = 1},
	    .mipLevels = info->mip_count,
	    .arrayLayers = info->array_size,
	    .samples = VK_SAMPLE_COUNT_1_BIT,
	    .tiling = VK_IMAGE_TILING_OPTIMAL,
	    .usage = image_usage,
	    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};

	VkImage image = VK_NULL_HANDLE;
	ret = vk->vkCreateImage(vk->device, &vk_info, NULL, &image);
	if (ret != VK_SUCCESS) {
		VK_ERROR(vk, "vkCreateImage: %s", vk_result_string(ret));
		// Nothing to cleanup
		return ret;
	}

	VkMemoryRequirements requirements = {0};
	vk->vkGetImageMemoryRequirements(vk->device, image, &requirements);

#if defined(XRT_GRAPHICS_BUFFER_HANDLE_IS_FD)
	VkImportMemoryFdInfoKHR import_memory_info = {
	    .sType = VK_STRUCTURE_TYPE_IMPORT_MEMORY_FD_INFO_KHR,
	    .handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT_KHR,
	    .fd = image_native->handle,
	};

	// TODO memoryTypeBits from VkMemoryFdPropertiesKHR
#elif defined(XRT_GRAPHICS_BUFFER_HANDLE_IS_AHARDWAREBUFFER)
	VkImportAndroidHardwareBufferInfoANDROID import_memory_info = {
	    .sType = VK_STRUCTURE_TYPE_IMPORT_ANDROID_HARDWARE_BUFFER_INFO_ANDROID,
	    .pNext = NULL,
	    .buffer = image_native->handle,
	};

	VkAndroidHardwareBufferPropertiesANDROID ahb_props = {
	    .sType = VK_STRUCTURE_TYPE_ANDROID_HARDWARE_BUFFER_PROPERTIES_ANDROID,
	};

	ret = vk->vkGetAndroidHardwareBufferPropertiesANDROID(vk->device, image_native->handle, &ahb_props);
	if (ret != VK_SUCCESS) {
		VK_ERROR(vk, "vkGetAndroidHardwareBufferPropertiesANDROID: %s", vk_result_string(ret));
		return ret;
	}

	requirements.size = ahb_props.allocationSize;
	requirements.memoryTypeBits = ahb_props.memoryTypeBits;
#elif defined(XRT_GRAPHICS_BUFFER_HANDLE_IS_WIN32_HANDLE)
	VkImportMemoryWin32HandleInfoKHR import_memory_info = {
	    .sType = VK_STRUCTURE_TYPE_IMPORT_MEMORY_WIN32_HANDLE_INFO_KHR,
	    .pNext = NULL,
	    .handleType = handle_type,
	    .handle = image_native->handle,
	};

	// TODO memoryTypeBits from VkMemoryWin32HandlePropertiesKHR
#else
#error "need port"
#endif

	if (handle_type == VK_EXTERNAL_MEMORY_HANDLE_TYPE_ANDROID_HARDWARE_BUFFER_BIT_ANDROID) {
		/*
		 * Skip check in this case
		 * VUID-VkMemoryAllocateInfo-allocationSize-02383
		 * For AHardwareBuffer handles, the alloc size must be the size returned by
		 * vkGetAndroidHardwareBufferPropertiesANDROID for the Android hardware buffer
		 */
	} else if ((handle_type & (VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_TEXTURE_BIT |
	                           VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_TEXTURE_KMT_BIT |
	                           VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D12_RESOURCE_BIT)) != 0) {

		/*
		 * For VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_TEXTURE_KMT_BIT and friends,
		 * the size must be queried by the implementation (See VkMemoryAllocateInfo manual page)
		 * so skip size check
		 */
	} else if (requirements.size == 0) {
		/*
		 * VUID-VkMemoryAllocateInfo-allocationSize-07899
		 * For any handles other than AHardwareBuffer, size must be greater than 0
		 */
		VK_ERROR(vk, "size must be greater than 0");

	} else if (requirements.size > image_native->size) {
		VK_ERROR(vk, "size mismatch, exported %" PRIu64 " but requires %" PRIu64, image_native->size,
		         requirements.size);
		return VK_ERROR_OUT_OF_DEVICE_MEMORY;
	} else if (requirements.size < image_native->size) {
		// it's OK if we have more memory than we need, APIs can round up
	}

	VkMemoryDedicatedAllocateInfoKHR dedicated_memory_info = {
	    .sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO_KHR,
	    .pNext = &import_memory_info,
	    .image = image,
	    .buffer = VK_NULL_HANDLE,
	};

	ret = vk_alloc_and_bind_image_memory( //
	    vk,                               // vk_bundle
	    image,                            // image
	    &requirements,                    // requirements
	    &dedicated_memory_info,           // pNext_for_allocate
	    __func__,                         // caller_name
	    out_mem);                         // out_mem

#if defined(XRT_GRAPHICS_BUFFER_HANDLE_CONSUMED_BY_VULKAN_IMPORT)
	// We have consumed this fd now, make sure it's not freed again.
	image_native->handle = XRT_GRAPHICS_BUFFER_HANDLE_INVALID;
#elif defined(XRT_GRAPHICS_BUFFER_HANDLE_REFERENCE_ADDED_BY_VULKAN_IMPORT)
	// Some platforms need an explicit unref (Android)
	u_graphics_buffer_unref(&image_native->handle);
#else
#error "Need port!"
#endif

	if (ret != VK_SUCCESS) {
		vk->vkDestroyImage(vk->device, image, NULL);
		return ret;
	}

	*out_image = image;
	return ret;
}

#if defined(XRT_GRAPHICS_BUFFER_HANDLE_IS_FD)

static VkResult
get_device_memory_handle(struct vk_bundle *vk, VkDeviceMemory device_memory, xrt_graphics_buffer_handle_t *out_handle)
{
	// vkGetMemoryFdKHR parameter
	VkMemoryGetFdInfoKHR fd_info = {
	    .sType = VK_STRUCTURE_TYPE_MEMORY_GET_FD_INFO_KHR,
	    .memory = device_memory,
	    .handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT_KHR,
	};

	int fd;
	VkResult ret = vk->vkGetMemoryFdKHR(vk->device, &fd_info, &fd);
	if (ret != VK_SUCCESS) {
		// COMP_ERROR(c, "->image - vkGetMemoryFdKHR: %s",
		//           vk_result_string(ret));
		return ret;
	}

	*out_handle = fd;

	return ret;
}

#elif defined(XRT_GRAPHICS_BUFFER_HANDLE_IS_AHARDWAREBUFFER)

static VkResult
get_device_memory_handle(struct vk_bundle *vk, VkDeviceMemory device_memory, xrt_graphics_buffer_handle_t *out_handle)
{
	// vkGetMemoryAndroidHardwareBufferANDROID parameter
	VkMemoryGetAndroidHardwareBufferInfoANDROID ahb_info = {
	    .sType = VK_STRUCTURE_TYPE_MEMORY_GET_ANDROID_HARDWARE_BUFFER_INFO_ANDROID,
	    .pNext = NULL,
	    .memory = device_memory,
	};

	AHardwareBuffer *buf = NULL;
	VkResult ret = vk->vkGetMemoryAndroidHardwareBufferANDROID(vk->device, &ahb_info, &buf);
	if (ret != VK_SUCCESS) {
		return ret;
	}

	*out_handle = buf;

	return ret;
}

#elif defined(XRT_GRAPHICS_BUFFER_HANDLE_IS_WIN32_HANDLE)

static VkResult
get_device_memory_handle(struct vk_bundle *vk, VkDeviceMemory device_memory, xrt_graphics_buffer_handle_t *out_handle)
{
	// vkGetMemoryWin32HandleKHR parameter
	VkMemoryGetWin32HandleInfoKHR win32_info = {
	    .sType = VK_STRUCTURE_TYPE_MEMORY_GET_WIN32_HANDLE_INFO_KHR,
	    .pNext = NULL,
	    .memory = device_memory,
	    .handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT_KHR,
	};

	HANDLE handle = NULL;
	VkResult ret = vk->vkGetMemoryWin32HandleKHR(vk->device, &win32_info, &handle);
	if (ret != VK_SUCCESS) {
		return ret;
	}

	*out_handle = handle;

	return ret;
}
#else
#error "Needs port"
#endif

XRT_CHECK_RESULT VkResult
vk_get_native_handle_from_device_memory(struct vk_bundle *vk,
                                        VkDeviceMemory device_memory,
                                        xrt_graphics_buffer_handle_t *out_handle)
{
	return get_device_memory_handle(vk, device_memory, out_handle);
}

VkResult
vk_create_sampler(struct vk_bundle *vk, VkSamplerAddressMode clamp_mode, VkSampler *out_sampler)
{
	VkSampler sampler;
	VkResult ret;

	VkSamplerCreateInfo info = {
	    .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
	    .magFilter = VK_FILTER_LINEAR,
	    .minFilter = VK_FILTER_LINEAR,
	    .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
	    .addressModeU = clamp_mode,
	    .addressModeV = clamp_mode,
	    .addressModeW = clamp_mode,
	    .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
	    .unnormalizedCoordinates = VK_FALSE,
	};

	ret = vk->vkCreateSampler(vk->device, &info, NULL, &sampler);
	if (ret != VK_SUCCESS) {
		VK_ERROR(vk, "vkCreateSampler: %s", vk_result_string(ret));
		return ret;
	}

	*out_sampler = sampler;

	return VK_SUCCESS;
}


/*
 *
 * Image view code.
 *
 */

static VkResult
create_view(struct vk_bundle *vk,
            VkImage image,
            VkImageViewType type,
            VkFormat format,
            VkImageSubresourceRange subresource_range,
            VkComponentMapping components,
            VkBaseInStructure *next_chain,
            VkImageView *out_view)
{
	VkImageView view;
	VkResult ret;

	VkImageViewCreateInfo imageView = {
	    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
	    .pNext = next_chain,
	    .image = image,
	    .viewType = type,
	    .format = format,
	    .components = components,
	    .subresourceRange = subresource_range,
	};

	ret = vk->vkCreateImageView(vk->device, &imageView, NULL, &view);
	if (ret != VK_SUCCESS) {
		VK_ERROR(vk, "vkCreateImageView: %s", vk_result_string(ret));
		return ret;
	}

	*out_view = view;

	return VK_SUCCESS;
}

static VkResult
create_view_default_swizzle(struct vk_bundle *vk,
                            VkImage image,
                            VkImageViewType type,
                            VkFormat format,
                            VkImageSubresourceRange subresource_range,
                            VkBaseInStructure *next_chain,
                            VkImageView *out_view)
{
	VkComponentMapping components = {
	    .r = VK_COMPONENT_SWIZZLE_R,
	    .g = VK_COMPONENT_SWIZZLE_G,
	    .b = VK_COMPONENT_SWIZZLE_B,
	    .a = VK_COMPONENT_SWIZZLE_A,
	};

	return create_view(    //
	    vk,                // vk_bundle
	    image,             // image
	    type,              // type
	    format,            // format
	    subresource_range, // subresource_range
	    components,        // components
	    next_chain,        // next_chain
	    out_view);         // out_view
}

VkResult
vk_create_view(struct vk_bundle *vk,
               VkImage image,
               VkImageViewType type,
               VkFormat format,
               VkImageSubresourceRange subresource_range,
               VkImageView *out_view)
{
	return create_view_default_swizzle( //
	    vk,                             // vk_bundle
	    image,                          // image
	    type,                           // type
	    format,                         // format
	    subresource_range,              // subresource_range
	    NULL,                           // next_chain
	    out_view);                      // out_view
}

VkResult
vk_create_view_swizzle(struct vk_bundle *vk,
                       VkImage image,
                       VkImageViewType type,
                       VkFormat format,
                       VkImageSubresourceRange subresource_range,
                       VkComponentMapping components,
                       VkImageView *out_view)
{
	return create_view(    //
	    vk,                // vk_bundle
	    image,             // image
	    type,              // type
	    format,            // format
	    subresource_range, // subresource_range
	    components,        // components
	    NULL,              // next_chain
	    out_view);         // out_view
}

VkResult
vk_create_view_usage(struct vk_bundle *vk,
                     VkImage image,
                     VkImageViewType type,
                     VkFormat format,
                     VkImageUsageFlags image_usage,
                     VkImageSubresourceRange subresource_range,
                     VkImageView *out_view)
{
	VkBaseInStructure *next_chain = NULL;

	/*
	 * @todo Handle Vulkan 1.0 instance without VK_KHR_maintenance2 on GPUs that don't support srgb with storage
	 * usage.
	 */
#ifdef VK_KHR_maintenance2
	VkImageViewUsageCreateInfo image_view_usage_create_info = {
	    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_USAGE_CREATE_INFO,
	    .pNext = next_chain,
	    .usage = image_usage,
	};

	if (vk->has_KHR_maintenance2 || vk->version >= VK_VERSION_1_1) {
		CHAIN(image_view_usage_create_info, next_chain);
	} else {
		VK_WARN(vk,
		        "Using Vulkan 1.0 instance without VK_KHR_maintenance2 support, can't use usage image view.");
	}
#endif

	return create_view_default_swizzle( //
	    vk,                             // vk_bundle
	    image,                          // image
	    type,                           // type
	    format,                         // format
	    subresource_range,              // subresource_range
	    next_chain,                     // next_chain
	    out_view);                      // out_view
}


/*
 *
 * Descriptor pool code.
 *
 */

bool
vk_init_descriptor_pool(struct vk_bundle *vk,
                        const VkDescriptorPoolSize *pool_sizes,
                        uint32_t pool_size_count,
                        uint32_t set_count,
                        VkDescriptorPool *out_descriptor_pool)
{
	VkDescriptorPoolCreateInfo info = {
	    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
	    .maxSets = set_count,
	    .poolSizeCount = pool_size_count,
	    .pPoolSizes = pool_sizes,
	};

	VkResult res = vk->vkCreateDescriptorPool(vk->device, &info, NULL, out_descriptor_pool);
	VK_CHK_WITH_RET(res, "vkCreateDescriptorPool", false);

	return true;
}

bool
vk_allocate_descriptor_sets(struct vk_bundle *vk,
                            VkDescriptorPool descriptor_pool,
                            uint32_t count,
                            const VkDescriptorSetLayout *set_layout,
                            VkDescriptorSet *sets)
{
	VkDescriptorSetAllocateInfo alloc_info = {
	    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
	    .descriptorPool = descriptor_pool,
	    .descriptorSetCount = count,
	    .pSetLayouts = set_layout,
	};

	VkResult res = vk->vkAllocateDescriptorSets(vk->device, &alloc_info, sets);
	VK_CHK_WITH_RET(res, "vkAllocateDescriptorSets", false);

	return true;
}


/*
 *
 * Buffer code.
 *
 */

bool
vk_buffer_init(struct vk_bundle *vk,
               VkDeviceSize size,
               VkBufferUsageFlags usage,
               VkMemoryPropertyFlags properties,
               VkBuffer *out_buffer,
               VkDeviceMemory *out_mem)
{
	VkBufferCreateInfo buffer_info = {
	    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
	    .size = size,
	    .usage = usage,
	};

	VkResult res = vk->vkCreateBuffer(vk->device, &buffer_info, NULL, out_buffer);
	VK_CHK_WITH_RET(res, "vkCreateBuffer", false);

	VkMemoryRequirements requirements;
	vk->vkGetBufferMemoryRequirements(vk->device, *out_buffer, &requirements);

	uint32_t memory_type_index = UINT32_MAX;
	bool bret = vk_get_memory_type(  //
	    vk,                          // vk_bundle
	    requirements.memoryTypeBits, // type_bits
	    properties,                  // memory_props
	    &memory_type_index);         // out_type_id
	if (!bret) {
		VK_ERROR(vk, "vk_get_memory_type: false\n\tFailed to find a matching memory type.");
		return VK_ERROR_OUT_OF_DEVICE_MEMORY;
	}

	// vkAllocateMemory argument
	VkMemoryAllocateInfo alloc_info = {
	    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
	    .allocationSize = requirements.size,
	    .memoryTypeIndex = memory_type_index,
	};

	res = vk->vkAllocateMemory(vk->device, &alloc_info, NULL, out_mem);
	VK_CHK_WITH_RET(res, "vkAllocateMemory", false);

	res = vk->vkBindBufferMemory(vk->device, *out_buffer, *out_mem, 0);
	VK_CHK_WITH_RET(res, "vkBindBufferMemory", false);

	return true;
}

void
vk_buffer_destroy(struct vk_buffer *self, struct vk_bundle *vk)
{
	vk->vkDestroyBuffer(vk->device, self->handle, NULL);
	vk->vkFreeMemory(vk->device, self->memory, NULL);
}

bool
vk_update_buffer(struct vk_bundle *vk, float *buffer, size_t buffer_size, VkDeviceMemory memory)
{
	void *tmp;
	VkResult res = vk->vkMapMemory(vk->device, memory, 0, VK_WHOLE_SIZE, 0, &tmp);
	VK_CHK_WITH_RET(res, "vkMapMemory", false);

	memcpy(tmp, buffer, buffer_size);

	VkMappedMemoryRange memory_range = {
	    .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
	    .memory = memory,
	    .size = VK_WHOLE_SIZE,
	};

	res = vk->vkFlushMappedMemoryRanges(vk->device, 1, &memory_range);
	VK_CHK_WITH_RET(res, "vkFlushMappedMemoryRanges", false);

	vk->vkUnmapMemory(vk->device, memory);

	return true;
}


/*
 *
 * Command buffer code.
 *
 */

void
vk_cmd_image_barrier_locked(struct vk_bundle *vk,
                            VkCommandBuffer cmd_buffer,
                            VkImage image,
                            VkAccessFlags src_access_mask,
                            VkAccessFlags dst_access_mask,
                            VkImageLayout old_image_layout,
                            VkImageLayout new_image_layout,
                            VkPipelineStageFlags src_stage_mask,
                            VkPipelineStageFlags dst_stage_mask,
                            VkImageSubresourceRange subresource_range)
{
	VkImageMemoryBarrier image_memory_barrier = {
	    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
	    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
	    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
	    .srcAccessMask = src_access_mask,
	    .dstAccessMask = dst_access_mask,
	    .oldLayout = old_image_layout,
	    .newLayout = new_image_layout,
	    .image = image,
	    .subresourceRange = subresource_range,
	};

	vk->vkCmdPipelineBarrier(   //
	    cmd_buffer,             // commandBuffer
	    src_stage_mask,         // srcStageMask
	    dst_stage_mask,         // dstStageMask
	    0,                      // dependencyFlags
	    0,                      // memoryBarrierCount
	    NULL,                   // pMemoryBarriers
	    0,                      // bufferMemoryBarrierCount
	    NULL,                   // pBufferMemoryBarriers
	    1,                      // imageMemoryBarrierCount
	    &image_memory_barrier); // pImageMemoryBarriers
}

void
vk_cmd_image_barrier_gpu_locked(struct vk_bundle *vk,
                                VkCommandBuffer cmd_buffer,
                                VkImage image,
                                VkAccessFlags src_access_mask,
                                VkAccessFlags dst_access_mask,
                                VkImageLayout old_layout,
                                VkImageLayout new_layout,
                                VkImageSubresourceRange subresource_range)
{
	vk_cmd_image_barrier_locked(            //
	    vk,                                 // vk_bundle
	    cmd_buffer,                         // cmd_buffer
	    image,                              // image
	    src_access_mask,                    // src_access_mask
	    dst_access_mask,                    // dst_access_mask
	    old_layout,                         // old_image_layout
	    new_layout,                         // new_image_layout
	    VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, // src_stage_mask
	    VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, // dst_stage_mask
	    subresource_range);                 // subresource_range
}

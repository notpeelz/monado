// Copyright 2019-2025, Collabora, Ltd.
// SPDX-License-Identifier: BSL-1.0
/*!
 * @file
 * @brief  Flags helpers for compositor swapchain images.
 *
 * These functions all concern only the compositor swapchain images that are
 * shared between the compositor and the application. That's why they are
 * grouped together and they are here because they need to be shared between
 * the @ref vk_image_collection and @ref comp_swapchain code so that they apply
 * the same flags everywhere.
 *
 * CSCI = Compositor SwapChain Images.
 *
 * @author Jakob Bornecrantz <jakob@collabora.com>
 * @author Christoph Haag <christoph.haag@collabora.com>
 * @author Benjamin Saunders <ben.e.saunders@gmail.com>
 * @ingroup aux_vk
 */

#include "xrt/xrt_handles.h"

#include "util/u_misc.h"
#include "util/u_debug.h"

#include "vk/vk_helpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef XRT_GRAPHICS_BUFFER_HANDLE_IS_AHARDWAREBUFFER
#include "android/android_ahardwarebuffer_allocator.h"
#endif

/*
 *
 * Helpers.
 *
 */

static bool
check_feature(VkFormat format,
              enum xrt_swapchain_usage_bits usage,
              VkFormatFeatureFlags format_features,
              VkFormatFeatureFlags flag)
{
	if ((format_features & flag) == 0) {
		U_LOG_E("vk_csci_get_image_usage_flags: %s requested but %s not supported for format %s (%08x) (%08x)",
		        xrt_swapchain_usage_flag_string(usage, false), vk_format_feature_flag_string(flag, false),
		        vk_format_string(format), format_features, flag);
		return false;
	}
	return true;
}


/*
 *
 * 'Exported' functions.
 *
 */

VkExternalMemoryHandleTypeFlags
vk_cb_get_buffer_external_handle_type(struct vk_bundle *vk)
{
#if defined(XRT_GRAPHICS_BUFFER_HANDLE_IS_FD)
	return VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT_KHR;
#elif defined(XRT_GRAPHICS_BUFFER_HANDLE_IS_AHARDWAREBUFFER)
	return VK_EXTERNAL_MEMORY_HANDLE_TYPE_ANDROID_HARDWARE_BUFFER_BIT_ANDROID;
#elif defined(XRT_GRAPHICS_BUFFER_HANDLE_IS_WIN32_HANDLE)
	return VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
#else
#error "need port"
#endif
}

VkAccessFlags
vk_csci_get_barrier_access_mask(enum xrt_swapchain_usage_bits bits)
{
	VkAccessFlags result = 0;
	if ((bits & XRT_SWAPCHAIN_USAGE_UNORDERED_ACCESS) != 0) {
		result |= VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
		if ((bits & XRT_SWAPCHAIN_USAGE_COLOR) != 0) {
			result |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		}
		if ((bits & XRT_SWAPCHAIN_USAGE_DEPTH_STENCIL) != 0) {
			result |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		}
	}
	if ((bits & XRT_SWAPCHAIN_USAGE_COLOR) != 0) {
		result |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}
	if ((bits & XRT_SWAPCHAIN_USAGE_DEPTH_STENCIL) != 0) {
		result |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}
	if ((bits & XRT_SWAPCHAIN_USAGE_TRANSFER_SRC) != 0) {
		result |= VK_ACCESS_TRANSFER_READ_BIT;
	}
	if ((bits & XRT_SWAPCHAIN_USAGE_TRANSFER_DST) != 0) {
		result |= VK_ACCESS_TRANSFER_WRITE_BIT;
	}
	if ((bits & XRT_SWAPCHAIN_USAGE_SAMPLED) != 0) {
		result |= VK_ACCESS_SHADER_READ_BIT;
	}
	return result;
}



VkImageLayout
vk_csci_get_barrier_optimal_layout(VkFormat format)
{
	// clang-format off
#define CASE_COLOR(FORMAT) case VK_FORMAT_##FORMAT: return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
#define CASE_DS(FORMAT) case VK_FORMAT_##FORMAT: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	// clang-format on

	switch (format) {
		VK_CSCI_FORMATS(CASE_COLOR, CASE_DS, CASE_DS, CASE_DS)
	default: //
		assert(false && !"Format not supported!");
		return VK_IMAGE_LAYOUT_UNDEFINED;
	}

#undef CASE_COLOR
#undef CASE_DS
}

VkImageAspectFlags
vk_csci_get_barrier_aspect_mask(VkFormat format)
{
	// clang-format off
#define CASE_COLOR(FORMAT) case VK_FORMAT_##FORMAT: return VK_IMAGE_ASPECT_COLOR_BIT;
#define CASE_DS(FORMAT) case VK_FORMAT_##FORMAT: return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
#define CASE_D(FORMAT) case VK_FORMAT_##FORMAT: return VK_IMAGE_ASPECT_DEPTH_BIT;
#define CASE_S(FORMAT) case VK_FORMAT_##FORMAT: return VK_IMAGE_ASPECT_STENCIL_BIT;
	// clang-format on

	switch (format) {
		VK_CSCI_FORMATS(CASE_COLOR, CASE_DS, CASE_D, CASE_S)
	default: //
		assert(false && !"Format not supported!");
		return 0;
	}

#undef CASE_COLOR
#undef CASE_DS
#undef CASE_D
#undef CASE_S
}

VkImageAspectFlags
vk_csci_get_image_view_aspect(VkFormat format, enum xrt_swapchain_usage_bits bits)
{
	// clang-format off
#define CASE_COLOR(FORMAT) case VK_FORMAT_##FORMAT: return VK_IMAGE_ASPECT_COLOR_BIT;
#define CASE_DS(FORMAT) case VK_FORMAT_##FORMAT: return VK_IMAGE_ASPECT_DEPTH_BIT; // We only want to sample the depth.
#define CASE_D(FORMAT) case VK_FORMAT_##FORMAT: return VK_IMAGE_ASPECT_DEPTH_BIT;
#define CASE_S(FORMAT) case VK_FORMAT_##FORMAT: return VK_IMAGE_ASPECT_STENCIL_BIT;
	// clang-format on

	switch (format) {
		VK_CSCI_FORMATS(CASE_COLOR, CASE_DS, CASE_D, CASE_S)
	default: //
		assert(false && !"Format not supported!");
		return 0;
	}

#undef CASE_COLOR
#undef CASE_DS
#undef CASE_D
#undef CASE_S
}

VkImageUsageFlags
vk_csci_get_image_usage_flags(struct vk_bundle *vk, VkFormat format, enum xrt_swapchain_usage_bits bits)
{
	VkFormatProperties prop;
	vk->vkGetPhysicalDeviceFormatProperties(vk->physical_device, format, &prop);

	VkImageUsageFlags image_usage = 0;

#define TEST(XRT_BIT, VK_FORMAT_BIT, VK_USAGE_BIT)                                                                     \
	if ((bits & XRT_BIT) != 0) {                                                                                   \
		if (!check_feature(format, XRT_BIT, prop.optimalTilingFeatures, VK_FORMAT_BIT)) {                      \
			return 0;                                                                                      \
		}                                                                                                      \
		image_usage |= VK_USAGE_BIT;                                                                           \
	}

	// clang-format off
	TEST(XRT_SWAPCHAIN_USAGE_COLOR,            VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT,         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
	TEST(XRT_SWAPCHAIN_USAGE_INPUT_ATTACHMENT, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT,         VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT)
	TEST(XRT_SWAPCHAIN_USAGE_DEPTH_STENCIL,    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
	TEST(XRT_SWAPCHAIN_USAGE_TRANSFER_SRC,     VK_FORMAT_FEATURE_TRANSFER_SRC_BIT,             VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
	TEST(XRT_SWAPCHAIN_USAGE_TRANSFER_DST,     VK_FORMAT_FEATURE_TRANSFER_DST_BIT,             VK_IMAGE_USAGE_TRANSFER_DST_BIT)
	TEST(XRT_SWAPCHAIN_USAGE_SAMPLED,          VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT,            VK_IMAGE_USAGE_SAMPLED_BIT)
	TEST(XRT_SWAPCHAIN_USAGE_UNORDERED_ACCESS, VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT,            VK_IMAGE_USAGE_STORAGE_BIT)
	// clang-format on

#undef TEST

	/*
	 * Should not be handled here.
	 *
	 * XRT_SWAPCHAIN_USAGE_MUTABLE_FORMAT
	 */

	return image_usage;
}

VkExternalMemoryHandleTypeFlags
vk_csci_get_image_external_handle_type(struct vk_bundle *vk, struct xrt_image_native *xin)
{
#if defined(XRT_GRAPHICS_BUFFER_HANDLE_IS_FD)
	return VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT_KHR;
#elif defined(XRT_GRAPHICS_BUFFER_HANDLE_IS_AHARDWAREBUFFER)
	return VK_EXTERNAL_MEMORY_HANDLE_TYPE_ANDROID_HARDWARE_BUFFER_BIT_ANDROID;
#elif defined(XRT_GRAPHICS_BUFFER_HANDLE_IS_WIN32_HANDLE)
	return (xin && xin->is_dxgi_handle) ? VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_TEXTURE_KMT_BIT
	                                    : VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
#else
#error "need port"
#endif
}

void
vk_csci_get_image_external_support(struct vk_bundle *vk,
                                   VkFormat image_format,
                                   enum xrt_swapchain_usage_bits bits,
                                   VkExternalMemoryHandleTypeFlags handle_type,
                                   bool *out_importable,
                                   bool *out_exportable)
{
	VkImageUsageFlags image_usage = vk_csci_get_image_usage_flags(vk, image_format, bits);

	// In->pNext
	VkPhysicalDeviceExternalImageFormatInfo external_image_format_info = {
	    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_IMAGE_FORMAT_INFO,
	    .handleType = handle_type,
	};

	// In
	VkPhysicalDeviceImageFormatInfo2 format_info = {
	    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2,
	    .pNext = &external_image_format_info,
	    .format = image_format,
	    .type = VK_IMAGE_TYPE_2D,
	    .tiling = VK_IMAGE_TILING_OPTIMAL,
	    .usage = image_usage,
	};

	// Out->pNext
	VkExternalImageFormatProperties external_format_properties = {
	    .sType = VK_STRUCTURE_TYPE_EXTERNAL_IMAGE_FORMAT_PROPERTIES,
	};

	// Out
	VkImageFormatProperties2 format_properties = {
	    .sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2,
	    .pNext = &external_format_properties,
	};

	VkResult ret =
	    vk->vkGetPhysicalDeviceImageFormatProperties2(vk->physical_device, &format_info, &format_properties);
	if (ret != VK_SUCCESS) {
		VK_ERROR(vk, "vkGetPhysicalDeviceImageFormatProperties2: %s", vk_result_string(ret));
		if (out_importable) {
			*out_importable = false;
		}
		if (out_exportable) {
			*out_exportable = false;
		}
		return;
	}

	VkExternalMemoryFeatureFlags features =
	    external_format_properties.externalMemoryProperties.externalMemoryFeatures;

	if (out_importable) {
		*out_importable = (features & VK_EXTERNAL_MEMORY_FEATURE_IMPORTABLE_BIT) != 0;
	}
	if (out_exportable) {
		*out_exportable = (features & VK_EXTERNAL_MEMORY_FEATURE_EXPORTABLE_BIT) != 0;
	}
}

#if defined(XRT_GRAPHICS_BUFFER_HANDLE_IS_AHARDWAREBUFFER)
static bool
has_ahardware_buffer_format_conversion(VkFormat format)
{
	/*
	 * Format mappings from official Android docs:
	 *
	 * | AHardwareBuffer Format                    | Vulkan format                                |
	 * |-------------------------------------------|----------------------------------------------|
	 * | AHARDWAREBUFFER_FORMAT_BLOB               | N/A                                          |
	 * | AHARDWAREBUFFER_FORMAT_D16_UNORM          | VK_FORMAT_D16_UNORM                          |
	 * | AHARDWAREBUFFER_FORMAT_D24_UNORM          | VK_FORMAT_X8_D24_UNORM_PACK32                |
	 * | AHARDWAREBUFFER_FORMAT_D24_UNORM_S8_UINT  | VK_FORMAT_D24_UNORM_S8_UINT                  |
	 * | AHARDWAREBUFFER_FORMAT_D32_FLOAT          | VK_FORMAT_D32_SFLOAT                         |
	 * | AHARDWAREBUFFER_FORMAT_D32_FLOAT_S8_UINT  | VK_FORMAT_D32_SFLOAT_S8_UINT                 |
	 * | AHARDWAREBUFFER_FORMAT_R10G10B10A10_UNORM | VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16 |
	 * | AHARDWAREBUFFER_FORMAT_R10G10B10A2_UNORM  | VK_FORMAT_A2B10G10R10_UNORM_PACK32           |
	 * | AHARDWAREBUFFER_FORMAT_R16G16B16A16_FLOAT | VK_FORMAT_R16G16B16A16_SFLOAT                |
	 * | AHARDWAREBUFFER_FORMAT_R16G16_UINT        | VK_FORMAT_R16G16_UINT                        |
	 * | AHARDWAREBUFFER_FORMAT_R16_UINT           | VK_FORMAT_R16_UINT                           |
	 * | AHARDWAREBUFFER_FORMAT_R5G6B5_UNORM       | VK_FORMAT_R5G6B5_UNORM_PACK16                |
	 * | AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM     | VK_FORMAT_R8G8B8A8_UNORM                     |
	 * | AHARDWAREBUFFER_FORMAT_R8G8B8X8_UNORM     | VK_FORMAT_R8G8B8A8_UNORM                     |
	 * | AHARDWAREBUFFER_FORMAT_R8G8B8_UNORM       | VK_FORMAT_R8G8B8_UNORM                       |
	 * | AHARDWAREBUFFER_FORMAT_R8_UNORM           | VK_FORMAT_R8_UNORM                           |
	 * | AHARDWAREBUFFER_FORMAT_S8_UINT            | VK_FORMAT_S8_UINT                            |
	 * | AHARDWAREBUFFER_FORMAT_Y8Cb8Cr8_420       | N/A                                          |
	 * | AHARDWAREBUFFER_FORMAT_YCbCr_P010         | N/A                                          |
	 *
	 * The VK_FORMAT_R8G8B8A8_SRGB format maps to AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM.
	 */

	switch (format) {
	case VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16: return true; // AHARDWAREBUFFER_FORMAT_R10G10B10A10_UNORM
	case VK_FORMAT_D16_UNORM: return true;                          // AHARDWAREBUFFER_FORMAT_D16_UNORM
	case VK_FORMAT_X8_D24_UNORM_PACK32: return true;                // AHARDWAREBUFFER_FORMAT_D24_UNORM
	case VK_FORMAT_D24_UNORM_S8_UINT: return true;                  // AHARDWAREBUFFER_FORMAT_D24_UNORM_S8_UINT
	case VK_FORMAT_D32_SFLOAT: return true;                         // AHARDWAREBUFFER_FORMAT_D32_FLOAT
	case VK_FORMAT_D32_SFLOAT_S8_UINT: return true;                 // AHARDWAREBUFFER_FORMAT_D32_FLOAT_S8_UINT
	case VK_FORMAT_A2B10G10R10_UNORM_PACK32: return true;           // AHARDWAREBUFFER_FORMAT_R10G10B10A2_UNORM
	case VK_FORMAT_R16G16B16A16_SFLOAT: return true;                // AHARDWAREBUFFER_FORMAT_R16G16B16A16_FLOAT
	case VK_FORMAT_R16G16_UINT: return true;                        // AHARDWAREBUFFER_FORMAT_R16G16_UINT
	case VK_FORMAT_R16_UINT: return true;                           // AHARDWAREBUFFER_FORMAT_R16_UINT
	case VK_FORMAT_R5G6B5_UNORM_PACK16: return true;                // AHARDWAREBUFFER_FORMAT_R5G6B5_UNORM
	case VK_FORMAT_R8G8B8A8_UNORM: return true;                     // AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM
	case VK_FORMAT_R8G8B8A8_SRGB: return true;                      // AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM
	case VK_FORMAT_R8G8B8_UNORM: return true;                       // AHARDWAREBUFFER_FORMAT_R8G8B8_UNORM
	case VK_FORMAT_R8_UNORM: return true;                           // AHARDWAREBUFFER_FORMAT_R8_UNORM
	case VK_FORMAT_S8_UINT: return true;                            // AHARDWAREBUFFER_FORMAT_S8_UINT
	default: return false;
	}
}
#endif

bool
vk_csci_is_format_supported(struct vk_bundle *vk, VkFormat format, enum xrt_swapchain_usage_bits xbits)
{
	/*
	 * First check if the format is supported at all.
	 */

#if defined(XRT_GRAPHICS_BUFFER_HANDLE_IS_AHARDWAREBUFFER)
	if (!has_ahardware_buffer_format_conversion(format)) {
		VK_DEBUG(vk, "Format '%s' does not map to a AHardwareBuffer format!", vk_format_string(format));
		return false;
	}

	if (!ahardwarebuffer_is_supported(format, xbits)) {
		VK_DEBUG(vk, "Format '%s' is not supported.", vk_format_string(format));
		return false;
	}
#endif

	VkFormatProperties prop;
	vk->vkGetPhysicalDeviceFormatProperties(vk->physical_device, format, &prop);
	const VkFormatFeatureFlagBits bits = prop.optimalTilingFeatures;

	if ((bits & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) == 0) {
		VK_DEBUG(vk, "Format '%s' cannot be sampled from in optimal layout!", vk_format_string(format));
		return false;
	}

	if ((xbits & XRT_SWAPCHAIN_USAGE_COLOR) != 0 && (bits & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) == 0) {
		VK_DEBUG(vk, "Color format '%s' cannot be used as render target in optimal layout!",
		         vk_format_string(format));
		return false;
	}

	if ((xbits & XRT_SWAPCHAIN_USAGE_DEPTH_STENCIL) != 0 &&
	    (bits & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == 0) {
		VK_DEBUG(vk, "Depth/stencil format '%s' cannot be used as render target in optimal layout!",
		         vk_format_string(format));
		return false;
	}


	/*
	 * Check exportability.
	 */

	VkExternalMemoryHandleTypeFlags handle_type = vk_csci_get_image_external_handle_type(vk, NULL);
	VkResult ret;

	VkImageUsageFlags usage = vk_csci_get_image_usage_flags(vk, format, xbits);

	// In->pNext
	VkPhysicalDeviceExternalImageFormatInfo external_image_format_info = {
	    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_IMAGE_FORMAT_INFO,
	    .handleType = handle_type,
	};

	// In
	VkPhysicalDeviceImageFormatInfo2 format_info = {
	    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2,
	    .pNext = &external_image_format_info,
	    .format = format,
	    .type = VK_IMAGE_TYPE_2D,
	    .tiling = VK_IMAGE_TILING_OPTIMAL,
	    .usage = usage,
	};

	// Out->pNext
	VkExternalImageFormatProperties external_format_properties = {
	    .sType = VK_STRUCTURE_TYPE_EXTERNAL_IMAGE_FORMAT_PROPERTIES,
	};

	// Out
	VkImageFormatProperties2 format_properties = {
	    .sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2,
	    .pNext = &external_format_properties,
	};

	ret = vk->vkGetPhysicalDeviceImageFormatProperties2(vk->physical_device, &format_info, &format_properties);
	if (ret == VK_ERROR_FORMAT_NOT_SUPPORTED) {
		VK_DEBUG(vk, "Format '%s' as external image is not supported!", vk_format_string(format));
		return false;
	} else if (ret != VK_SUCCESS) {
		// This is not a expected path.
		VK_ERROR(vk, "vkGetPhysicalDeviceImageFormatProperties2: %s for format '%s'", vk_result_string(ret),
		         vk_format_string(format));
		return false;
	}

	VkExternalMemoryFeatureFlags features =
	    external_format_properties.externalMemoryProperties.externalMemoryFeatures;

	if ((features & VK_EXTERNAL_MEMORY_FEATURE_IMPORTABLE_BIT) == 0) {
		VK_DEBUG(vk, "Format '%s' is not importable!", vk_format_string(format));
		return false;
	}

	if ((features & VK_EXTERNAL_MEMORY_FEATURE_EXPORTABLE_BIT) == 0) {
		VK_DEBUG(vk, "Format '%s' is not exportable!", vk_format_string(format));
		return false;
	}

	return true;
}

// Copyright 2019-2023, Collabora, Ltd.
// SPDX-License-Identifier: BSL-1.0
/*!
 * @file
 * @brief  OpenGL client side glue to compositor implementation.
 * @author Jakob Bornecrantz <jakob@collabora.com>
 * @ingroup comp_client
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "xrt/xrt_config_os.h"
#include "util/u_misc.h"

#include <xrt/xrt_config_have.h>
#if defined(XRT_HAVE_EGL)
#include "ogl/egl_api.h"
#endif
#if defined(XRT_HAVE_OPENGL) || defined(XRT_HAVE_OPENGLES)
#include "ogl/ogl_api.h"
#endif

#include "ogl/ogl_helpers.h"

#include "client/comp_gl_client.h"

#include "util/u_logging.h"
#include "util/u_trace_marker.h"



/*
 *
 * Helpers.
 *
 */

/*!
 * Down-cast helper.
 * @private @memberof client_gl_swapchain
 */
static inline struct client_gl_swapchain *
client_gl_swapchain(struct xrt_swapchain *xsc)
{
	return (struct client_gl_swapchain *)xsc;
}

static inline struct xrt_swapchain *
to_native_swapchain(struct xrt_swapchain *xsc)
{
	return &client_gl_swapchain(xsc)->xscn->base;
}

static inline struct xrt_compositor *
to_native_compositor(struct xrt_compositor *xc)
{
	return &client_gl_compositor(xc)->xcn->base;
}

static int64_t
gl_format_to_vk(int64_t format)
{
	switch (format) {
	case GL_RGB8: return 23 /*VK_FORMAT_R8G8B8_UNORM*/; // Should not be used, colour precision.
	case GL_SRGB8: return 29 /*VK_FORMAT_R8G8B8_SRGB*/;
	case GL_RGBA8: return 37 /*VK_FORMAT_R8G8B8A8_UNORM*/; // Should not be used, colour precision.
	case GL_SRGB8_ALPHA8: return 43 /*VK_FORMAT_R8G8B8A8_SRGB*/;
	case GL_RGB10_A2: return 64 /*VK_FORMAT_A2B10G10R10_UNORM_PACK32*/;
	case GL_RGB16: return 84 /*VK_FORMAT_R16G16B16_UNORM*/;
	case GL_RGB16F: return 90 /*VK_FORMAT_R16G16B16_SFLOAT*/;
	case GL_RGBA16: return 91 /*VK_FORMAT_R16G16B16A16_UNORM*/;
	case GL_RGBA16F: return 97 /*VK_FORMAT_R16G16B16A16_SFLOAT*/;
	case GL_DEPTH_COMPONENT16: return 124 /*VK_FORMAT_D16_UNORM*/;
	case GL_DEPTH_COMPONENT32F: return 126 /*VK_FORMAT_D32_SFLOAT*/;
	case GL_DEPTH24_STENCIL8: return 129 /*VK_FORMAT_D24_UNORM_S8_UINT*/;
	case GL_DEPTH32F_STENCIL8: return 130 /*VK_FORMAT_D32_SFLOAT_S8_UINT*/;
	default: U_LOG_W("Cannot convert GL format %" PRIu64 " to VK format!", format); return 0;
	}
}

static int64_t
vk_format_to_gl(int64_t format)
{
	switch (format) {
	case 4 /*   VK_FORMAT_R5G6B5_UNORM_PACK16      */: return 0;       // GL_RGB565?
	case 23 /*  VK_FORMAT_R8G8B8_UNORM             */: return GL_RGB8; // Should not be used, colour precision.
	case 29 /*  VK_FORMAT_R8G8B8_SRGB              */: return GL_SRGB8;
	case 30 /*  VK_FORMAT_B8G8R8_UNORM             */: return 0;
	case 37 /*  VK_FORMAT_R8G8B8A8_UNORM           */: return GL_RGBA8; // Should not be used, colour precision.
	case 43 /*  VK_FORMAT_R8G8B8A8_SRGB            */: return GL_SRGB8_ALPHA8;
	case 44 /*  VK_FORMAT_B8G8R8A8_UNORM           */: return 0;
	case 50 /*  VK_FORMAT_B8G8R8A8_SRGB            */: return 0;
	case 64 /*  VK_FORMAT_A2B10G10R10_UNORM_PACK32 */: return GL_RGB10_A2;
	case 84 /*  VK_FORMAT_R16G16B16_UNORM          */: return GL_RGB16;
	case 90 /*  VK_FORMAT_R16G16B16_SFLOAT         */: return GL_RGB16F;
	case 91 /*  VK_FORMAT_R16G16B16A16_UNORM       */: return GL_RGBA16;
	case 97 /*  VK_FORMAT_R16G16B16A16_SFLOAT      */: return GL_RGBA16F;
	case 100 /* VK_FORMAT_R32_SFLOAT               */: return 0;
	case 124 /* VK_FORMAT_D16_UNORM                */: return GL_DEPTH_COMPONENT16;
	case 125 /* VK_FORMAT_X8_D24_UNORM_PACK32      */: return 0; // GL_DEPTH_COMPONENT24?
	case 126 /* VK_FORMAT_D32_SFLOAT               */: return GL_DEPTH_COMPONENT32F;
	case 127 /* VK_FORMAT_S8_UINT                  */: return 0; // GL_STENCIL_INDEX8?
	case 129 /* VK_FORMAT_D24_UNORM_S8_UINT        */: return GL_DEPTH24_STENCIL8;
	case 130 /* VK_FORMAT_D32_SFLOAT_S8_UINT       */: return GL_DEPTH32F_STENCIL8;
	default: U_LOG_W("Cannot convert VK format %" PRIu64 " to GL format!", format); return 0;
	}
}

/*!
 * Called with the right context made current.
 */
static xrt_graphics_sync_handle_t
handle_fencing_or_finish(struct client_gl_compositor *c)
{
	xrt_graphics_sync_handle_t sync_handle = XRT_GRAPHICS_SYNC_HANDLE_INVALID;
	xrt_result_t xret = XRT_SUCCESS;

	if (c->insert_fence != NULL) {
		COMP_TRACE_IDENT(insert_fence);

		xret = c->insert_fence(&c->base.base, &sync_handle);
		if (xret != XRT_SUCCESS) {
			U_LOG_E("Failed to insert a fence");
		}
	}

	// Fallback to glFinish if we haven't inserted a fence.
	if (sync_handle == XRT_GRAPHICS_SYNC_HANDLE_INVALID) {
		COMP_TRACE_IDENT(glFinish);

		glFinish();
	}

	return sync_handle;
}


/*
 *
 * Swapchain functions.
 *
 */

static xrt_result_t
client_gl_swapchain_acquire_image(struct xrt_swapchain *xsc, uint32_t *out_index)
{
	// Pipe down call into native swapchain.
	return xrt_swapchain_acquire_image(to_native_swapchain(xsc), out_index);
}

static xrt_result_t
client_gl_swapchain_wait_image(struct xrt_swapchain *xsc, int64_t timeout_ns, uint32_t index)
{
	// Pipe down call into native swapchain.
	return xrt_swapchain_wait_image(to_native_swapchain(xsc), timeout_ns, index);
}

static xrt_result_t
client_gl_swapchain_barrier_image(struct xrt_swapchain *xsc, enum xrt_barrier_direction direction, uint32_t index)
{
	return XRT_SUCCESS;
}

static xrt_result_t
client_gl_swapchain_release_image(struct xrt_swapchain *xsc, uint32_t index)
{
	// Pipe down call into native swapchain.
	return xrt_swapchain_release_image(to_native_swapchain(xsc), index);
}


/*
 *
 * Compositor functions.
 *
 */

static xrt_result_t
client_gl_compositor_begin_session(struct xrt_compositor *xc, const struct xrt_begin_session_info *info)
{
	// Pipe down call into native compositor.
	return xrt_comp_begin_session(to_native_compositor(xc), info);
}

static xrt_result_t
client_gl_compositor_end_session(struct xrt_compositor *xc)
{
	// Pipe down call into native compositor.
	return xrt_comp_end_session(to_native_compositor(xc));
}

static xrt_result_t
client_gl_compositor_wait_frame(struct xrt_compositor *xc,
                                int64_t *out_frame_id,
                                int64_t *predicted_display_time,
                                int64_t *predicted_display_period)
{
	// Pipe down call into native compositor.
	return xrt_comp_wait_frame(    //
	    to_native_compositor(xc),  //
	    out_frame_id,              //
	    predicted_display_time,    //
	    predicted_display_period); //
}

static xrt_result_t
client_gl_compositor_begin_frame(struct xrt_compositor *xc, int64_t frame_id)
{
	// Pipe down call into native compositor.
	return xrt_comp_begin_frame(to_native_compositor(xc), frame_id);
}

static xrt_result_t
client_gl_compositor_discard_frame(struct xrt_compositor *xc, int64_t frame_id)
{
	// Pipe down call into native compositor.
	return xrt_comp_discard_frame(to_native_compositor(xc), frame_id);
}

static xrt_result_t
client_gl_compositor_layer_begin(struct xrt_compositor *xc, const struct xrt_layer_frame_data *data)
{
	// Pipe down call into native compositor.
	return xrt_comp_layer_begin(to_native_compositor(xc), data);
}

static xrt_result_t
client_gl_compositor_layer_projection(struct xrt_compositor *xc,
                                      struct xrt_device *xdev,
                                      struct xrt_swapchain *xsc[XRT_MAX_VIEWS],
                                      const struct xrt_layer_data *data)
{
	struct xrt_compositor *xcn;
	struct xrt_swapchain *xscn[XRT_MAX_VIEWS];

	xcn = to_native_compositor(xc);
	assert(data->type == XRT_LAYER_PROJECTION);
	for (uint32_t i = 0; i < data->view_count; ++i) {
		xscn[i] = &client_gl_swapchain(xsc[i])->xscn->base;
	}
	struct xrt_layer_data d = *data;
	d.flip_y = !d.flip_y;

	return xrt_comp_layer_projection(xcn, xdev, xscn, &d);
}

static xrt_result_t
client_gl_compositor_layer_projection_depth(struct xrt_compositor *xc,
                                            struct xrt_device *xdev,
                                            struct xrt_swapchain *xsc[XRT_MAX_VIEWS],
                                            struct xrt_swapchain *d_xsc[XRT_MAX_VIEWS],
                                            const struct xrt_layer_data *data)
{
	struct xrt_compositor *xcn;
	struct xrt_swapchain *xscn[XRT_MAX_VIEWS];
	struct xrt_swapchain *d_xscn[XRT_MAX_VIEWS];

	assert(data->type == XRT_LAYER_PROJECTION_DEPTH);

	xcn = to_native_compositor(xc);
	for (uint32_t i = 0; i < data->view_count; ++i) {
		xscn[i] = to_native_swapchain(xsc[i]);
		d_xscn[i] = to_native_swapchain(d_xsc[i]);
	}

	struct xrt_layer_data d = *data;
	d.flip_y = !d.flip_y;

	return xrt_comp_layer_projection_depth(xcn, xdev, xscn, d_xscn, &d);
}

static xrt_result_t
client_gl_compositor_layer_quad(struct xrt_compositor *xc,
                                struct xrt_device *xdev,
                                struct xrt_swapchain *xsc,
                                const struct xrt_layer_data *data)
{
	struct xrt_compositor *xcn;
	struct xrt_swapchain *xscfb;

	assert(data->type == XRT_LAYER_QUAD);

	xcn = to_native_compositor(xc);
	xscfb = to_native_swapchain(xsc);

	struct xrt_layer_data d = *data;
	d.flip_y = !d.flip_y;

	return xrt_comp_layer_quad(xcn, xdev, xscfb, &d);
}

static xrt_result_t
client_gl_compositor_layer_cube(struct xrt_compositor *xc,
                                struct xrt_device *xdev,
                                struct xrt_swapchain *xsc,
                                const struct xrt_layer_data *data)
{
	struct xrt_compositor *xcn;
	struct xrt_swapchain *xscfb;

	assert(data->type == XRT_LAYER_CUBE);

	xcn = to_native_compositor(xc);
	xscfb = to_native_swapchain(xsc);

	struct xrt_layer_data d = *data;
	d.flip_y = !d.flip_y;

	return xrt_comp_layer_cube(xcn, xdev, xscfb, &d);
}

static xrt_result_t
client_gl_compositor_layer_cylinder(struct xrt_compositor *xc,
                                    struct xrt_device *xdev,
                                    struct xrt_swapchain *xsc,
                                    const struct xrt_layer_data *data)
{
	struct xrt_compositor *xcn;
	struct xrt_swapchain *xscfb;

	assert(data->type == XRT_LAYER_CYLINDER);

	xcn = to_native_compositor(xc);
	xscfb = to_native_swapchain(xsc);

	struct xrt_layer_data d = *data;
	d.flip_y = !d.flip_y;

	return xrt_comp_layer_cylinder(xcn, xdev, xscfb, &d);
}

static xrt_result_t
client_gl_compositor_layer_equirect1(struct xrt_compositor *xc,
                                     struct xrt_device *xdev,
                                     struct xrt_swapchain *xsc,
                                     const struct xrt_layer_data *data)
{
	struct xrt_compositor *xcn;
	struct xrt_swapchain *xscfb;

	assert(data->type == XRT_LAYER_EQUIRECT1);

	xcn = to_native_compositor(xc);
	xscfb = to_native_swapchain(xsc);

	struct xrt_layer_data d = *data;
	d.flip_y = !d.flip_y;

	return xrt_comp_layer_equirect1(xcn, xdev, xscfb, &d);
}

static xrt_result_t
client_gl_compositor_layer_equirect2(struct xrt_compositor *xc,
                                     struct xrt_device *xdev,
                                     struct xrt_swapchain *xsc,
                                     const struct xrt_layer_data *data)
{
	struct xrt_compositor *xcn;
	struct xrt_swapchain *xscfb;

	assert(data->type == XRT_LAYER_EQUIRECT2);

	xcn = to_native_compositor(xc);
	xscfb = to_native_swapchain(xsc);

	struct xrt_layer_data d = *data;
	d.flip_y = !d.flip_y;

	return xrt_comp_layer_equirect2(xcn, xdev, xscfb, &d);
}

static xrt_result_t
client_gl_compositor_layer_passthrough(struct xrt_compositor *xc,
                                       struct xrt_device *xdev,
                                       const struct xrt_layer_data *data)
{
	struct client_gl_compositor *c = client_gl_compositor(xc);

	assert(data->type == XRT_LAYER_PASSTHROUGH);

	struct xrt_layer_data d = *data;
	d.flip_y = !d.flip_y;

	return xrt_comp_layer_passthrough(&c->xcn->base, xdev, &d);
}

static xrt_result_t
client_gl_compositor_layer_commit(struct xrt_compositor *xc, xrt_graphics_sync_handle_t sync_handle)
{
	COMP_TRACE_MARKER();

	struct client_gl_compositor *c = client_gl_compositor(xc);

	if (c->renderdoc_enabled) {
		glDebugMessageInsert(GL_DEBUG_SOURCE_THIRD_PARTY, GL_DEBUG_TYPE_MARKER, 1,
		                     GL_DEBUG_SEVERITY_NOTIFICATION, -1, "vr-marker,frame_end,type,application");
	}

	// We make the sync object, not st/oxr which is our user.
	assert(!xrt_graphics_sync_handle_is_valid(sync_handle));

	sync_handle = XRT_GRAPHICS_SYNC_HANDLE_INVALID;

	xrt_result_t xret = client_gl_compositor_context_begin(xc, CLIENT_GL_CONTEXT_REASON_SYNCHRONIZE);
	if (xret == XRT_SUCCESS) {
		sync_handle = handle_fencing_or_finish(c);
		client_gl_compositor_context_end(xc, CLIENT_GL_CONTEXT_REASON_SYNCHRONIZE);
	}

	COMP_TRACE_IDENT(layer_commit);

	return xrt_comp_layer_commit(&c->xcn->base, sync_handle);
}

static xrt_result_t
client_gl_compositor_get_swapchain_create_properties(struct xrt_compositor *xc,
                                                     const struct xrt_swapchain_create_info *info,
                                                     struct xrt_swapchain_create_properties *xsccp)
{
	struct client_gl_compositor *c = client_gl_compositor(xc);

	int64_t vk_format = gl_format_to_vk(info->format);
	if (vk_format == 0) {
		U_LOG_E("Invalid format!");
		return XRT_ERROR_SWAPCHAIN_FORMAT_UNSUPPORTED;
	}

	struct xrt_swapchain_create_info vkinfo = *info;
	vkinfo.format = vk_format;

	return xrt_comp_get_swapchain_create_properties(&c->xcn->base, &vkinfo, xsccp);
}

static xrt_result_t
client_gl_swapchain_create(struct xrt_compositor *xc,
                           const struct xrt_swapchain_create_info *info,
                           struct xrt_swapchain **out_xsc)
{
	struct client_gl_compositor *c = client_gl_compositor(xc);
	struct xrt_swapchain_create_properties xsccp = {0};
	xrt_result_t xret = XRT_SUCCESS;

	// Do before getting the context, not using ourselves.
	xret = xrt_comp_get_swapchain_create_properties(xc, info, &xsccp);
	if (xret != XRT_SUCCESS) {
		U_LOG_E("Failed to get create properties: %u", xret);
		return xret;
	}

	// Check before setting the context.
	int64_t vk_format = gl_format_to_vk(info->format);
	if (vk_format == 0) {
		U_LOG_E("Invalid format!");
		return XRT_ERROR_SWAPCHAIN_FORMAT_UNSUPPORTED;
	}

	xret = client_gl_compositor_context_begin(xc, CLIENT_GL_CONTEXT_REASON_OTHER);
	if (xret != XRT_SUCCESS) {
		return xret;
	}

	if (info->array_size > 1) {
		const char *version_str = (const char *)glGetString(GL_VERSION);
		if (strstr(version_str, "OpenGL ES 2.") == version_str) {
			U_LOG_E("Only one array layer is supported with OpenGL ES 2");
			client_gl_compositor_context_end(xc, CLIENT_GL_CONTEXT_REASON_OTHER);
			return XRT_ERROR_SWAPCHAIN_FLAG_VALID_BUT_UNSUPPORTED;
		}
	}

	struct xrt_swapchain_create_info xinfo = *info;
	struct xrt_swapchain_create_info vkinfo = *info;

	// Update the create info.
	xinfo.bits |= xsccp.extra_bits;
	vkinfo.format = vk_format;
	vkinfo.bits |= xsccp.extra_bits;

	struct xrt_swapchain_native *xscn = NULL; // Has to be NULL.
	xret = xrt_comp_native_create_swapchain(c->xcn, &vkinfo, &xscn);

	if (xret != XRT_SUCCESS) {
		client_gl_compositor_context_end(xc, CLIENT_GL_CONTEXT_REASON_OTHER);
		return xret;
	}
	assert(xscn != NULL);

	// Save texture binding
	GLint prev_texture = 0;
	GLuint binding_enum = 0;
	GLuint tex_target = 0;
	ogl_texture_target_for_swapchain_info(&xinfo, &tex_target, &binding_enum);

	glGetIntegerv(binding_enum, &prev_texture);

	struct xrt_swapchain *xsc = &xscn->base;

	struct client_gl_swapchain *sc = NULL;
	if (NULL == c->create_swapchain(xc, &xinfo, xscn, &sc)) {
		// Drop our reference, does NULL checking.
		xrt_swapchain_reference(&xsc, NULL);
		client_gl_compositor_context_end(xc, CLIENT_GL_CONTEXT_REASON_OTHER);
		return XRT_ERROR_OPENGL;
	}

	if (sc == NULL) {
		U_LOG_E("Could not create OpenGL swapchain.");
		client_gl_compositor_context_end(xc, CLIENT_GL_CONTEXT_REASON_OTHER);
		return XRT_ERROR_OPENGL;
	}

	if (NULL == sc->base.base.acquire_image) {
		sc->base.base.acquire_image = client_gl_swapchain_acquire_image;
	}
	if (NULL == sc->base.base.wait_image) {
		sc->base.base.wait_image = client_gl_swapchain_wait_image;
	}
	if (NULL == sc->base.base.barrier_image) {
		sc->base.base.barrier_image = client_gl_swapchain_barrier_image;
	}
	if (NULL == sc->base.base.release_image) {
		sc->base.base.release_image = client_gl_swapchain_release_image;
	}
	// Fetch the number of images from the native swapchain.
	sc->base.base.image_count = xsc->image_count;
	sc->xscn = xscn;

	glBindTexture(tex_target, prev_texture);

	client_gl_compositor_context_end(xc, CLIENT_GL_CONTEXT_REASON_OTHER);

	*out_xsc = &sc->base.base;
	return XRT_SUCCESS;
}

static xrt_result_t
client_gl_compositor_passthrough_create(struct xrt_compositor *xc, const struct xrt_passthrough_create_info *info)
{
	struct client_gl_compositor *c = client_gl_compositor(xc);

	// Pipe down call into native compositor.
	return xrt_comp_create_passthrough(&c->xcn->base, info);
}

static xrt_result_t
client_gl_compositor_passthrough_layer_create(struct xrt_compositor *xc,
                                              const struct xrt_passthrough_layer_create_info *info)
{
	struct client_gl_compositor *c = client_gl_compositor(xc);

	// Pipe down call into native compositor.
	return xrt_comp_create_passthrough_layer(&c->xcn->base, info);
}

static xrt_result_t
client_gl_compositor_passthrough_destroy(struct xrt_compositor *xc)
{
	struct client_gl_compositor *c = client_gl_compositor(xc);

	// Pipe down call into native compositor.
	return xrt_comp_destroy_passthrough(&c->xcn->base);
}

static void
client_gl_compositor_destroy(struct xrt_compositor *xc)
{
	assert(!"Destroy should be implemented by the winsys code that uses the GL code.");
}


/*
 *
 * 'Exported' functions.
 *
 */

void
client_gl_compositor_fini(struct client_gl_compositor *c)
{
	os_mutex_destroy(&c->context_mutex);
}

bool
client_gl_compositor_init(struct client_gl_compositor *c,
                          struct xrt_compositor_native *xcn,
                          client_gl_context_begin_locked_func_t context_begin_locked,
                          client_gl_context_end_locked_func_t context_end_locked,
                          client_gl_swapchain_create_func_t create_swapchain,
                          client_gl_insert_fence_func_t insert_fence)
{
	assert(context_begin_locked != NULL);
	assert(context_end_locked != NULL);

	c->base.base.get_swapchain_create_properties = client_gl_compositor_get_swapchain_create_properties;
	c->base.base.create_swapchain = client_gl_swapchain_create;
	c->base.base.create_passthrough = client_gl_compositor_passthrough_create;
	c->base.base.create_passthrough_layer = client_gl_compositor_passthrough_layer_create;
	c->base.base.destroy_passthrough = client_gl_compositor_passthrough_destroy;
	c->base.base.begin_session = client_gl_compositor_begin_session;
	c->base.base.end_session = client_gl_compositor_end_session;
	c->base.base.wait_frame = client_gl_compositor_wait_frame;
	c->base.base.begin_frame = client_gl_compositor_begin_frame;
	c->base.base.discard_frame = client_gl_compositor_discard_frame;
	c->base.base.layer_begin = client_gl_compositor_layer_begin;
	c->base.base.layer_projection = client_gl_compositor_layer_projection;
	c->base.base.layer_projection_depth = client_gl_compositor_layer_projection_depth;
	c->base.base.layer_quad = client_gl_compositor_layer_quad;
	c->base.base.layer_cube = client_gl_compositor_layer_cube;
	c->base.base.layer_cylinder = client_gl_compositor_layer_cylinder;
	c->base.base.layer_equirect1 = client_gl_compositor_layer_equirect1;
	c->base.base.layer_equirect2 = client_gl_compositor_layer_equirect2;
	c->base.base.layer_passthrough = client_gl_compositor_layer_passthrough;
	c->base.base.layer_commit = client_gl_compositor_layer_commit;
	c->base.base.destroy = client_gl_compositor_destroy;
	c->context_begin_locked = context_begin_locked;
	c->context_end_locked = context_end_locked;
	c->create_swapchain = create_swapchain;
	c->insert_fence = insert_fence;
	c->xcn = xcn;

	// Passthrough our formats from the native compositor to the client.
	uint32_t count = 0;

	// Make sure that we can fit all formats in the destination.
	static_assert(ARRAY_SIZE(xcn->base.info.formats) == ARRAY_SIZE(c->base.base.info.formats), "mismatch");

	for (uint32_t i = 0; i < xcn->base.info.format_count; i++) {
		int64_t f = vk_format_to_gl(xcn->base.info.formats[i]);
		if (f == 0) {
			continue;
		}

		c->base.base.info.formats[count++] = f;
	}
	c->base.base.info.format_count = count;

	// Get max texture size.
	GLint max_texture_size = 0;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
	if (max_texture_size > 0) {
		c->base.base.info.max_texture_size = (uint32_t)max_texture_size;
	}

	os_mutex_init(&c->context_mutex);

	return true;
}

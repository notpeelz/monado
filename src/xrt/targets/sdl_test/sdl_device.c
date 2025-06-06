// Copyright 2020-2023, Collabora, Ltd.
// SPDX-License-Identifier: BSL-1.0
/*!
 * @file
 * @brief  Shared default implementation of the device with compositor.
 * @author Jakob Bornecrantz <jakob@collabora.com>
 */

#include "sdl_internal.h"

#include "math/m_mathinclude.h"

#include "util/u_device.h"
#include "util/u_distortion_mesh.h"

#include <stdio.h>

static xrt_result_t
sdl_hmd_get_tracked_pose(struct xrt_device *xdev,
                         enum xrt_input_name name,
                         int64_t at_timestamp_ns,
                         struct xrt_space_relation *out_relation)
{
	struct sdl_program *sp = from_xdev(xdev);

	if (name != XRT_INPUT_GENERIC_HEAD_POSE) {
		U_LOG_XDEV_UNSUPPORTED_INPUT(&sp->xdev_base, u_log_get_global_level(), name);
		return XRT_ERROR_INPUT_UNSUPPORTED;
	}

	struct xrt_space_relation relation = XRT_SPACE_RELATION_ZERO;

	relation.pose = sp->state.head.pose;
	relation.relation_flags =                       //
	    XRT_SPACE_RELATION_POSITION_TRACKED_BIT |   //
	    XRT_SPACE_RELATION_POSITION_VALID_BIT |     //
	    XRT_SPACE_RELATION_ORIENTATION_VALID_BIT |  //
	    XRT_SPACE_RELATION_ORIENTATION_TRACKED_BIT; //

	*out_relation = relation;

	return XRT_SUCCESS;
}

static void
sdl_hmd_destroy(struct xrt_device *xdev)
{
	struct sdl_program *sp = from_xdev(xdev);

	if (xdev->hmd->distortion.mesh.vertices) {
		free(xdev->hmd->distortion.mesh.vertices);
		xdev->hmd->distortion.mesh.vertices = NULL;
	}

	if (xdev->hmd->distortion.mesh.indices) {
		free(xdev->hmd->distortion.mesh.indices);
		xdev->hmd->distortion.mesh.indices = NULL;
	}

	(void)sp; // We are apart of sdl_program, do not free.
}


/*
 *
 * 'Exported' functions.
 *
 */

void
sdl_device_init(struct sdl_program *sp)
{
	struct xrt_device *xdev = &sp->xdev_base;

	// Setup pointers.
	xdev->inputs = sp->inputs;
	xdev->input_count = ARRAY_SIZE(sp->inputs);
	xdev->tracking_origin = &sp->origin;
	xdev->hmd = &sp->hmd;

	// Name and type.
	xdev->name = XRT_DEVICE_GENERIC_HMD;
	xdev->device_type = XRT_DEVICE_TYPE_HMD;
	xdev->hmd->view_count = 2;

	// Print name.
	snprintf(xdev->str, XRT_DEVICE_NAME_LEN, "SDL HMD");
	snprintf(xdev->serial, XRT_DEVICE_NAME_LEN, "SDL HMD");

	// Input info.
	xdev->inputs[0].name = XRT_INPUT_GENERIC_HEAD_POSE;
	xdev->inputs[0].active = true;

	// Function pointers.
	xdev->update_inputs = u_device_noop_update_inputs;
	xdev->get_tracked_pose = sdl_hmd_get_tracked_pose;
	xdev->get_view_poses = u_device_get_view_poses;
	xdev->destroy = sdl_hmd_destroy;

	// Minimum needed stuff.
	struct u_device_simple_info info;
	info.display.w_pixels = 1280;
	info.display.h_pixels = 720;
	info.display.w_meters = 0.13f;
	info.display.h_meters = 0.07f;
	info.lens_horizontal_separation_meters = 0.13f / 2.0f;
	info.lens_vertical_position_meters = 0.07f / 2.0f;
	info.fov[0] = 85.0f * ((float)(M_PI) / 180.0f);
	info.fov[1] = 85.0f * ((float)(M_PI) / 180.0f);

	if (!u_device_setup_split_side_by_side(xdev, &info)) {
		U_LOG_E("Failed to setup basic device info");
		return;
	}

	// Refresh rate.
	xdev->hmd->screens[0].nominal_frame_interval_ns = time_s_to_ns(1.0f / 60.0f);

	// Blend mode(s), setup after u_device_setup_split_side_by_side.
	xdev->hmd->blend_modes[0] = XRT_BLEND_MODE_OPAQUE;
	xdev->hmd->blend_mode_count = 1;

	// Distortion information, fills in xdev->compute_distortion().
	u_distortion_mesh_set_none(xdev);

	// Tracking origin.
	xdev->tracking_origin->initial_offset = (struct xrt_pose)XRT_POSE_IDENTITY;
	xdev->tracking_origin->type = XRT_TRACKING_TYPE_OTHER;
	snprintf(xdev->tracking_origin->name, XRT_TRACKING_NAME_LEN, "SDL Tracking");
}

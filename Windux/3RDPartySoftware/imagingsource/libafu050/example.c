#include <stdio.h>
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/interfaces/xoverlay.h>

#include "afu050.h"

int new_frame_cb(afu050_handle_t handle, 
		 unsigned char *jpegbuf,
		 size_t jpegsize,
		 void *data)
{
	GstElement *appsrc = GST_ELEMENT(data);
	GstBuffer *buf;

	/* printf ("new frame: %d!\n", jpegsize); */

	buf = gst_buffer_new();
	GST_BUFFER_DATA(buf) = jpegbuf;
	GST_BUFFER_SIZE(buf) = jpegsize;
	GST_BUFFER_MALLOCDATA(buf) = jpegbuf;

	if (gst_app_src_push_buffer(GST_APP_SRC(appsrc), buf) != GST_FLOW_OK){
		gst_buffer_unref (buf);
	}

	return 0;
}

int main(int argc, char **argv)
{
	afu050_handle_t handle;
	GstElement *pipeline;
	GstElement *appsrc;
	GstCaps *caps;
	GError *error = NULL;

 	gst_init(&argc, &argv);

 	pipeline = gst_parse_launch ("appsrc name=source block=false ! queue ! jpegdec ! queue ! videoscale ! "\
				     "cogcolorspace ! video/x-raw-rgb,width=640,height=480 ! "\
				     "ximagesink sync=false force-aspect-ratio=true", &error);
	if (error){
		fprintf( stderr, "Error building pipeline: %s\n", error->message);
		return -1;
	}
	appsrc = gst_bin_get_by_name(GST_BIN(pipeline), "source");
	caps = gst_caps_from_string("image/jpeg");
	g_object_set (appsrc, "block", FALSE, NULL);
	gst_app_src_set_caps(GST_APP_SRC(appsrc), caps);
	gst_element_set_state( pipeline, GST_STATE_PLAYING);

	/*
	  1: Acquire a handle to the camera
	*/
	handle = afu050_open();
	if (!handle){
		fprintf (stderr, "failed to open camera\n");
		return -1;
	}
	printf ("afu050_open: OK\n");

	/*
	  2: Set desired video format
	*/
	afu050_set_video_format(handle, AFU050_FMT_2592X1944);

	/*
	  3: Enable all auto functions
	*/
	afu050_set_exposure_auto (handle, 1);
	afu050_set_gain_auto (handle, 1);
	afu050_set_white_balance_auto (handle, 1);

	/*
	  4: Start capturing image data
	*/
	if (afu050_capture_start(handle, new_frame_cb, appsrc) < 0){
		fprintf (stderr, "failed to start capture\n");
		return -1;
	}
	printf ("afu050_capture_start: OK\n");


	// Let the camera run for 10 seconds, our callback function gets
	// called during that time
	sleep(5);

	afu050_auto_focus(handle);

	sleep(5);

	/*
	  5: Stop image capture process
	*/
	afu050_capture_stop(handle);
	printf ("afu050_capture_stop\n");

	/*
	  6: Free resources allocated for the camera
	*/
	afu050_close(handle);
	return 0;
}

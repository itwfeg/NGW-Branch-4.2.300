#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#define __USE_GNU
#include <string.h>
#include <libusb-1.0/libusb.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>

#include "afu050.h"
#include "tisuvc.h"

#define CMD_MAIN 0x3022
#define CMD_ACK  0x3023
#define CMD_PARA0 0x3024
#define CMD_PARA1 0x3025
#define CMD_PARA2 0x3026
#define CMD_PARA3 0x3027
#define CMD_PARA4 0x3028

#define UVC_SET_CUR 0x1
#define UVC_GET_CUR 0x81

#define AFU050_VENDOR_ID   0x199e
#define AFU050_PRODUCT_ID  0x8209
#define USB_ENDPOINT_IN	  (LIBUSB_ENDPOINT_IN  | 2)

#define LEN_IN_BUFFER 1024*32
#define TRANSFER_COUNT 32
#define JPEGBUF_SIZE (1024*1024*5)

struct afu050_handle
{
	struct libusb_device_handle *devh;
	libusb_context *ctx;
	
	unsigned char *jpegbuf;
	volatile int jpegptr;
	size_t jpgsize;

	afu050_new_frame_callback_t new_frame_cb;
	void *new_frame_user_data;
	int capture_quit;
	pthread_t capture_thread;
	int capture_running;
	
};

static int set_control(afu050_handle_t handle, int unit, int ctrl, int len, unsigned char *value )
{
   int ret;
   
   ret = libusb_control_transfer( handle->devh, 
				  LIBUSB_ENDPOINT_OUT | 
				  LIBUSB_REQUEST_TYPE_CLASS | 
				  LIBUSB_RECIPIENT_DEVICE, 
				  UVC_SET_CUR, 
				  ctrl << 8, 
				  unit << 8, 
				  value, len, 10000 );

   return (ret == len ? 0 : -1);
}


static int get_control(afu050_handle_t handle, int unit, int ctrl, int len, unsigned char *value )
{
   int ret;
   int request = UVC_GET_CUR;
   
   ret = libusb_control_transfer( handle->devh, 
				  LIBUSB_ENDPOINT_IN | 
				  LIBUSB_REQUEST_TYPE_CLASS | 
				  LIBUSB_RECIPIENT_DEVICE, 
				  request, 
				  ctrl << 8, 
				  unit << 8, 
				  value, len, 10000 );

   return (ret == len ? 0 : -1);
}

#define DEBUG_IIC 0
#if DEBUG_IIC
#define IICPRINTF(...) {printf(__VA_ARGS__);}
#else
#define IICPRINTF(...)
#endif

static int euvc_iic_new( afu050_handle_t handle, int wlen, unsigned long wdata, int rlen, unsigned long *rdata  )
{
   int ret;
   unsigned char tmp[10] = {0,0,0,0, 0,0,0,0, 0,0};
   int i;

   tmp[0] = wlen;
   tmp[1] = rlen;

   for ( i = 0; i < wlen; i++){
	   tmp[3+i] = (wdata >> (((wlen-1)-i)*8));
   }

   tmp[2] = 0x3c;

   IICPRINTF ("write: ");
   for ( i = 0; i < wlen+3; i++){
	   IICPRINTF( "%02x ", tmp[i]);
   }
   IICPRINTF ("\n");
	   
   
   ret = libusb_control_transfer( handle->devh, 
				  LIBUSB_ENDPOINT_OUT | 
				  LIBUSB_REQUEST_TYPE_CLASS | 
				  LIBUSB_RECIPIENT_INTERFACE, 
				  UVC_SET_CUR, 
				  0x9 << 8, 
				  0x4 << 8,
				  (unsigned char*)tmp, 10, 10000 );

   IICPRINTF ("ret %x\n", ret);

   if (rlen){
	   tmp [0] = 0;
	   ret = libusb_control_transfer( handle->devh, 
				   LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE, 
				   UVC_GET_CUR, 
				   0x09 << 8, 
				   0x4 << 8, 
				   (unsigned char*)tmp, rlen, 10000 );
	   *rdata = 0;
	   IICPRINTF ("got: %d\n", tmp[0]);
	   for (i = 0; i < rlen; i++){
		   *rdata |= tmp[i] << (((rlen-1)-i)*8);
	   }
	   IICPRINTF ("ret2 %x\n", ret);
   }

   return ret;
}

static int write_i2c(afu050_handle_t handle, int addr, int val)
{
	uint32_t wdat = (addr << 8) | (val & 0xff);
	int ret;
	//printf ("wdat: %x\n", wdat);
	if ((ret = euvc_iic_new (handle, 3, wdat, 0, NULL))<0){
		IICPRINTF ("IIC Write  Error\n");
	}
	return ret;
}

#if 0
static void Sensor_read_word(afu050_handle_t handle, unsigned short idx, unsigned char *pValue)
{
	unsigned long value;
	if (!euvc_iic_new( handle, 2, idx, 1, &value)){
		IICPRINTF ("Fail: Sensor_read_word\n");
	}
	*pValue = value & 0xff;
}
#endif

static int Sensor_write_word(afu050_handle_t handle, unsigned short idx, unsigned char value)
{
	return (write_i2c(handle, idx, value));
}


static int set_video_format (afu050_handle_t handle, uint8_t format_index ,uint8_t frame_index, uint32_t frame_interval)
{
	int ret;
	uint8_t buf[34];

	memset (buf, 0x0, sizeof(buf));

	buf[2] = format_index;
	buf[3] = frame_index;
	buf[4] = frame_interval & 0xff;
	buf[5] = (frame_interval >> 8) & 0xff;
	buf[6] = (frame_interval >> 16) & 0xff;
	buf[7] = (frame_interval >> 24) & 0xff;
   
	ret = libusb_control_transfer( handle->devh, 
				       LIBUSB_ENDPOINT_OUT | 
				       LIBUSB_REQUEST_TYPE_CLASS | 
				       LIBUSB_RECIPIENT_INTERFACE, 
				       UVC_SET_CUR, 
				       0x2 << 8, // VS_COMMIT
				       1, 
				       buf, sizeof(buf), 10000 );
	frame_interval = buf[4] | ((uint32_t)buf[5]<<8) | ((uint32_t)buf[6]<<16) | ((uint32_t)buf[7]<<24);

	return ret;
}

int afu050_set_video_format(afu050_handle_t handle, afu050_video_format_t format)
{
	if ((format < AFU050_FMT_2592X1944) || 
	    (format > AFU050_FMT_1280X960)){
		return -EINVAL;
	}
	
	return set_video_format (handle, format, 1, 0);
}

static int jpeg_done(afu050_handle_t handle, unsigned char *ptr, size_t size)
{
	return handle->new_frame_cb (handle, 
				     ptr, 
				     size, 
				     handle->new_frame_user_data);
}

static void cb_in(struct libusb_transfer *transfer)
{
	int processed = 0;
	unsigned char *ptr = transfer->buffer;
	afu050_handle_t handle = (afu050_handle_t)transfer->user_data;

	while (processed < transfer->actual_length){
		if (!handle->jpegbuf){
			unsigned char * soi = 
				(unsigned char*)memmem(ptr + processed, 
						       ( transfer->actual_length - 
							 processed),
						       "\xff\xd8", 2);
			
			if (soi){	
				handle->jpegbuf = (unsigned char*)
					malloc(JPEGBUF_SIZE);
				processed = soi - transfer->buffer;
				memcpy (handle->jpegbuf, soi, 2);
				processed += 2;
				handle->jpgsize += 2;
				handle->jpegptr += 2;
				
			}
			
		} 
		
		if (handle->jpegbuf){
			unsigned char *eoi = memmem(ptr + processed,
						    transfer->actual_length -
						    processed,
						    "\xff\xd9", 2);
			int cplen = transfer->actual_length - processed;
			if (eoi){
				cplen = (((eoi) - ptr)-processed)+2;
			}
			if ((handle->jpgsize+cplen) > JPEGBUF_SIZE){
				//JPEG too big, dropping
				handle->jpegptr = 0;
				handle->jpgsize = 0;
				free (handle->jpegbuf);
				handle->jpegbuf = 0;
				break;
			}
			memcpy (handle->jpegbuf + handle->jpegptr, 
				ptr + processed, cplen);
			processed += cplen;
			handle->jpegptr += cplen;
			handle->jpgsize += cplen;
				
			if (eoi){
				int do_free = 1;
				handle->jpegptr = 0;
				do_free = jpeg_done(handle, handle->jpegbuf, 
						    handle->jpgsize);
				if (do_free)
					free (handle->jpegbuf);
				handle->jpegbuf = 0;
				handle->jpgsize = 0;
			}	
		}else{
			processed += transfer->actual_length;
		}
	}
	//submit the next transfer
	libusb_submit_transfer(transfer);
}


afu050_handle_t afu050_open(void)
{
	int ret;
	afu050_handle_t handle = NULL;

	handle = calloc(1, sizeof( struct afu050_handle ));
	if (!handle)
		return NULL;

	//init libUSB
	ret = libusb_init(&handle->ctx);
	if (ret < 0) {
		fprintf(stderr, "Failed to initialise libusb\n");
		return NULL;
	}
	
	handle->devh = libusb_open_device_with_vid_pid (handle->ctx, 
							AFU050_VENDOR_ID, 
							AFU050_PRODUCT_ID);
	
	if (!handle->devh){
		fprintf (stderr, "Failed to open camera. "\
			 "Please check device permissions!\n");
		libusb_exit (handle->ctx);
		free(handle);
		return NULL;
	}
	
	/* claim the interface */
	if ( (libusb_claim_interface(handle->devh, 0) < 0) || 
	     (libusb_claim_interface(handle->devh, 1) < 0)){
		fprintf (stderr, "Failed to open camera. "\
			 "Please check device permissions!\n");
		libusb_close (handle->devh);
		libusb_exit (handle->ctx);
		free(handle);
		return NULL;
	}
		
	return handle;
}

void afu050_close(afu050_handle_t handle)
{
	afu050_capture_stop(handle);
	libusb_close (handle->devh);
	libusb_exit (handle->ctx);
	if (handle->jpegbuf)
		free(handle->jpegbuf);
	free(handle);
}
	
static void *capture_thread(void *_handle)
{
	afu050_handle_t handle = (afu050_handle_t)_handle;
	int ret;

	while (!handle->capture_quit){
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 1000;
		ret =  libusb_handle_events_timeout_completed(handle->ctx, &tv, NULL);
		if (ret < 0)
			break;
	}
 	
	return NULL;
}


int afu050_capture_start (afu050_handle_t handle, 
			 afu050_new_frame_callback_t cb, 
			 void *user_data)
{
	int ret;
	int cnt;
	
	if (cb == NULL)
		return -EINVAL;

	if (handle->capture_running)
		return -EBUSY;
	
	handle->new_frame_cb = cb;
	handle->new_frame_user_data = user_data;
	
	if ((ret = pthread_create (&handle->capture_thread, NULL, 
				   capture_thread, (void*)handle))<0){
		return ret;
	}
	
	for (cnt = 0; cnt < TRANSFER_COUNT; cnt++){
		uint8_t *buf = (uint8_t*)malloc( LEN_IN_BUFFER );
		struct libusb_transfer *transfer_in = NULL;
		
		transfer_in  = libusb_alloc_transfer(0);
		libusb_fill_bulk_transfer( transfer_in, handle->devh, 
					   USB_ENDPOINT_IN,
					   buf,  LEN_IN_BUFFER,  
					   cb_in, handle, 0); 
		//submit the transfer, all following transfers are initiated from the CB
		ret = libusb_submit_transfer(transfer_in);
		if (ret < 0 )
			break;
	}
		
	return ret;
}

int afu050_capture_stop(afu050_handle_t handle)
{
	if (!handle->capture_running)
		return 0;
	
	handle->capture_quit = 1;
	pthread_join(handle->capture_thread, NULL);
	
	handle->capture_running = 0;
	return 0;
}

int afu050_set_exposure_auto(afu050_handle_t handle, int auto_value)
{
	unsigned char val = auto_value ? 1:0;
	
	return set_control(handle, 
			   VC_ID_EXTENSION_UNIT, 
			   XU_AUTO_EXPOSURE, 
			   1, 
			   &val);
}

int afu050_get_exposure_auto(afu050_handle_t handle, int *auto_value)
{
	unsigned char val;
	int ret;
	
	ret = get_control(handle,
			  VC_ID_EXTENSION_UNIT, 
			  XU_AUTO_EXPOSURE, 
			  1, 
			  &val);
	
	*auto_value = val ? 1 : 0;
	return ret;
}


int afu050_set_exposure_time(afu050_handle_t handle, unsigned int exposure_us)
{
	exposure_us /= 100;
	
	return set_control(handle, 
			   VC_ID_INPUT_TERMINAL, 
			   CT_EXPOSURE_TIME_ABSOLUTE_CONTROL, 
			   4, 
			   (unsigned char*)&exposure_us);
}

int afu050_get_exposure_time(afu050_handle_t handle, int *exposure_us)
{
	unsigned int val;
	int ret;
	
	ret = get_control(handle,
			  VC_ID_INPUT_TERMINAL, 
			  CT_EXPOSURE_TIME_ABSOLUTE_CONTROL, 
			  4, 
			  (unsigned char*)&val);
	
	*exposure_us = val * 100;
	return ret;
}


int afu050_set_gain_auto(afu050_handle_t handle, int auto_value)
{
	unsigned char val = auto_value ? 1:0;
	
	return set_control(handle, 
			   VC_ID_EXTENSION_UNIT, 
			   XU_AUTO_GAIN, 
			   1, 
			   &val);
}

int afu050_get_gain_auto(afu050_handle_t handle, int *auto_value)
{
	unsigned char val;
	int ret;
	
	ret = get_control(handle,
			  VC_ID_EXTENSION_UNIT, 
			  XU_AUTO_GAIN, 
			  1, 
			  &val);
	
	*auto_value = val ? 1 : 0;
	return ret;
}


int afu050_set_gain(afu050_handle_t handle, unsigned short gain)
{
	return set_control(handle, 
			   VC_ID_PROCESSING_UNIT, 
			   PU_GAIN_CONTROL,
			   2, 
			   (unsigned char*)&gain);
}

int afu050_get_gain(afu050_handle_t handle, unsigned short *gain)
{
	return get_control(handle, 
			   VC_ID_PROCESSING_UNIT, 
			   PU_GAIN_CONTROL,
			   2, 
			   (unsigned char*)gain);
}

int afu050_set_white_balance_auto(afu050_handle_t handle, int auto_value)
{
	unsigned char val = auto_value ? 1 : 0;
	
	return set_control(handle, 
			   VC_ID_EXTENSION_UNIT, 
			   XU_AUTO_WHITE_BALANCE,
			   1,
			   &val);
}

int afu050_get_white_balance_auto(afu050_handle_t handle, int *auto_value)
{
	unsigned char val;
	int ret;
	
	ret = get_control(handle,
			  VC_ID_EXTENSION_UNIT, 
			  XU_AUTO_WHITE_BALANCE, 
			  1, 
			  &val);
	
	*auto_value = val ? 1 : 0;
	return ret;
}

int afu050_set_white_balance_components(afu050_handle_t handle, 
				       unsigned short red, 
				       unsigned short green, 
				       unsigned short blue)
{
	int ret;
	ret = set_control(handle, 
			  VC_ID_EXTENSION_UNIT, 
			  XU_GAIN_R_CONTROL,
			  2, 
			  (unsigned char*)&red);
	if (ret < 0)
		return ret;	
	ret = set_control(handle, 
			  VC_ID_EXTENSION_UNIT, 
			  XU_GAIN_G_CONTROL,
			  2, 
			  (unsigned char*)&green);
	if (ret < 0)
		return ret;	
	ret = set_control(handle, 
			  VC_ID_EXTENSION_UNIT, 
			  XU_GAIN_B_CONTROL,
			  2, 
			  (unsigned char*)&blue);
	return ret;
}

int afu050_get_white_balance_components(afu050_handle_t handle, 
				       unsigned short *red, 
				       unsigned short *green, 
				       unsigned short *blue)
{
	int ret;
	ret = get_control(handle, 
			  VC_ID_EXTENSION_UNIT, 
			  XU_GAIN_R_CONTROL,
			  2, 
			  (unsigned char*)red);
	if (ret < 0)
		return ret;	
	ret = get_control(handle, 
			  VC_ID_EXTENSION_UNIT, 
			  XU_GAIN_G_CONTROL,
			  2, 
			  (unsigned char*)green);
	if (ret < 0)
		return ret;	
	ret = get_control(handle, 
			  VC_ID_EXTENSION_UNIT, 
			  XU_GAIN_B_CONTROL,
			  2, 
			  (unsigned char*)blue);
	return ret;
}


int afu050_auto_focus (afu050_handle_t handle)
{
	unsigned char val = 1;

	return set_control(handle, 
			   VC_ID_EXTENSION_UNIT, 
			   XU_FOCUS_ONE_PUSH,
			   1,
			   &val);	
}

int afu050_set_focus_zone (afu050_handle_t handle, int zoneX, int zoneY)
{
	int ret;
	ret = Sensor_write_word (handle, 0x3024, zoneX - 8);
	ret |= Sensor_write_word (handle, 0x3025, zoneY - 6);
	ret |= Sensor_write_word (handle, CMD_ACK, 0x01);
	ret |= Sensor_write_word (handle, CMD_MAIN, 0x81);	

	return ret;
}	

//
// File : Husb_drv.c
//
// Generic USB driver written for Hobart devices.
//
#include<linux/init.h>
//#include<linux/config.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/slab.h>
#include<linux/fs.h>
#include<linux/errno.h>
#include<linux/types.h>
#include<linux/proc_fs.h>
#include<linux/fcntl.h>
#include<linux/usb.h>
#include<linux/mod_devicetable.h>
#include<asm/system.h>
#include<asm/uaccess.h>
#include<linux/smp_lock.h>
//#include<linux/devfs_fs_kernel.h>
#include"Hobart_usb.h"


//MODULE_LICENSE("Proprietary");
MODULE_LICENSE("GPL");

//
//error codes
//
#define HOBART_USB_SUCCESS                      0
#define HOBART_USB_FAILED                       -1
#define HOBART_NOT_MY_DEV                       -1


//
//module specific
//

#define HOBART_MOD_NAME                         "Hobart_usb"

#define HOBART_USB_MINOR_BASE                   192

//
//generic defs
//
#define HOBART_BLK_BUF_SIZE                     100
#define HOBART_NAME_SIZE                        100

//
//usb device specific
//
#define HOBART_VID                              0x128C
#define HOB_WRAPPER_RAW_PID                     0x8500
#define HOB_WEIGHER_RAW_PID                     0x8400
#define HOB_PRINTER_RAW_PID                     0x8200
#define HOB_WRAPPER_PID                         0x0500
#define HOB_WEIGHER_PID                         0x0400
#define HOB_PRINTER_PID                         0x0200
//#define HOB_WEIGHER_DEV_PID                   0x8600
#define HOBART_SCALE_INTERFACE                  1
#define HOBART_WEIGHER_INTERFACE                1
#define HOBART_PRINTER_INTERFACE                1

//
//Driver status bits
//
#define HOBART_RUNNING                          0x01
#define HOBART_RESET                            0x02
#define HOBART_STOPPED                          0x03




//########################################
//module typedefs
//########################################
//
//Hobart usb typedefs
//
typedef struct inode                            HOBART_INODE;
typedef struct inode *                          HOBART_PINODE;
typedef struct file                             HOBART_FILE;
typedef struct file *                           HOBART_PFILE;
typedef struct file_operations                  HOBART_FILE_OP;
typedef struct file_operations *                HOBART_PFILE_OP;
typedef struct usb_device                       HOBART_USB_DEV;
typedef struct usb_device *                     HOBART_PUSB_DEV;
typedef struct usb_driver                       HOBART_USB_DRV;
typedef struct usb_driver *                     HOBART_PUSB_DRV;
typedef struct usb_interface                    HOBART_USB_IFACE;
typedef struct usb_interface *                  HOBART_PUSB_IFACE;
typedef struct usb_device_id                    HOBART_USB_DEV_ID;
typedef struct usb_device_id *                  HOBART_PUSB_DEV_ID;
/***
  typedef struct urb_t                            HOBART_URB;
  typedef struct urb_t *                          HOBART_PURB;
 ****/
typedef struct urb                              HOBART_URB;
typedef struct urb *                            HOBART_PURB;
typedef loff_t                                  HOBART_LOFF;
typedef loff_t *                                HOBART_PLOFF;
typedef struct kref                             HOBART_KREF;
typedef struct kref *                           HOBART_PKREF;
typedef struct usb_endpoint_descriptor          HOBART_EP_DESC;
typedef struct usb_endpoint_descriptor *        HOBART_PEP_DESC;
typedef struct usb_host_interface               HOBART_HOST_IFACE;
typedef struct usb_host_interface *             HOBART_PHOST_IFACE;
typedef struct device                           HOBART_DEV;
typedef struct device *                         HOBART_PDEV;

/****enable
  typedef devfs_handle_t                          HOBART_DEVFS;
 ******/
/*****************************************************/
//we can use the PAL_typedef.h if need be but char is
//probably good enough here. depends on device need
//hence defered for now.
typedef char            TCHAR;
/*****************************************************/

//########################################
//Function proto type
//########################################
//
//Interface routines for the driver
//
static int32_t Hobart_init(void);
static void Hobart_exit(void);
int32_t Hobart_open(HOBART_PINODE i_node, HOBART_PFILE f_ptr);
int32_t Hobart_release(HOBART_PINODE i_node, HOBART_PFILE f_ptr);
ssize_t Hobart_read(HOBART_PFILE f_ptr, TCHAR *buf, size_t cnt,
        HOBART_PLOFF f_pos);
ssize_t Hobart_write(HOBART_PFILE f_ptr, const TCHAR *buf, size_t cnt,
        HOBART_PLOFF f_pos);
int32_t Hobart_ioctl(HOBART_PINODE i_node, HOBART_PFILE f_ptr, uint32_t
        code, unsigned long s);
int Hobart_probe(HOBART_PUSB_IFACE usb_intf, const struct usb_device_id *id_table);
void Hobart_disconnect(HOBART_PUSB_IFACE intf);

/*********enable if needed
  int Hobart_suspend(HOBART_PUSB_IFACE usb_intf, pm_message_t msg);
  int Hobart_resetResume(HOBART_PUSB_IFACE usb_intf);
  int Hobart_prereset(HOBART_PUSB_IFACE usb_intf);
  int Hobart_postreset(HOBART_PUSB_IFACE usb_intf);
 ***********/


//########################################
//Module specific
//########################################

//
//Hobart file access interface
//
HOBART_FILE_OP Hobart_fops =
{
read:           Hobart_read,
                write:          Hobart_write,
                open:           Hobart_open,
                release:        Hobart_release,
                ioctl:          Hobart_ioctl
};

//
//Registering init and exit
//routiens
module_init(Hobart_init);
module_exit(Hobart_exit);

//
//module major/minor nums
//
int32_t Hobart_major = 70;//USB_MAJOR;          //major num for the driver
int32_t Hobart_minor = HOBART_USB_MINOR_BASE;   //minor num for the driver

//
//driver private data struct
//
typedef struct Hobart_drv_contxt
{
    //
    //house keeping
    //
    int32_t         open_cnt;               //count of handles open
    int32_t         minor;                  //driver minor number
    //
    //device related
    //
    HOBART_PUSB_DEV dev;                    //our usb_dev struct
    HOBART_PUSB_IFACE iface;
    /*******enable
      HOBART_DEVFS    devfs;                  //Devfs used by App
     **********/
    //
    //usb transfer related
    //
    HOBART_PURB     out_urb;                //urb used for output
    HOBART_PURB     in_urb;                 //urb used for input
    size_t          osize;                  //bulk output size
    size_t          isize;                  //bulk input size
    __u8            out_ep_addr;            //Bulk output endpoint
    __u8            in_ep_addr;             //Bulk input endpoint
    char *          in_buf;//[HOBART_BLK_BUF_SIZE];
    //input buffer
    char *          out_buf;//[HOBART_BLK_BUF_SIZE];
    //input buffer
    int32_t         status;                 //current drv status
    HOBART_KREF     kref_cnt;
}HOBART_DRV_CONTEXT, *HOBART_PDRV_CONTEXT;

//
//usb class driver info needed in order to get a minor number from
//the usb core & to have the device registered with devfs and the
//driver core
//
static struct usb_class_driver Hobart_class =
{
    //      name:  "Hobart/"HOBART_MOD_NAME"%d",
name:  HOBART_MOD_NAME"%d",
       fops: &Hobart_fops,


       minor_base: HOBART_USB_MINOR_BASE,
};


//########################################
//Helper Function proto type
//########################################
static int32_t Hobart_getDrvContext(HOBART_PINODE inode,
        HOBART_PFILE f_ptr);
//static void Hobart_freeResources(HOBART_PDRV_CONTEXT drv_contxt);
static void Hobart_freeResources(HOBART_PKREF kref);

//########################################
//USB specific
//########################################

//
//Hobart table of supported devices
//
static HOBART_USB_DEV_ID Hobart_dev_tab[] =
{
    //      {USB_DEVICE(HOBART_VID, HOB_WRAPPER_RAW_PID)},
    //      {USB_DEVICE(HOBART_VID, HOB_WEIGHER_RAW_PID)},
    {USB_DEVICE(HOBART_VID, HOB_PRINTER_RAW_PID)},
    {USB_DEVICE(HOBART_VID, HOB_WRAPPER_PID)},
    {USB_DEVICE(HOBART_VID, HOB_WEIGHER_PID)},
    {USB_DEVICE(HOBART_VID, HOB_PRINTER_PID)},

    { }                                     //terminating entry
};
MODULE_DEVICE_TABLE(usb, Hobart_dev_tab);

//
//Hobart usb_driver struct
//
static HOBART_USB_DRV Hobart_udrv =
{
name:           HOBART_MOD_NAME,
                probe:          Hobart_probe,           //device plugin handler
                disconnect:     Hobart_disconnect,      //device removal handler
                /*********enable if needed
                  suspend:        Hobart_suspend,
                  resume:         Hobart_resetResume,
                  reset_resume:   Hobart_resetResume,
                  pre_reset:      Hobart_prereset,
                  post_reset:     Hobart_postreset,
                 ************/
                id_table:       Hobart_dev_tab
                    //      fops:           &Hobart_fops,
                    //      minor:          Hobart_minor,
};


//########################################
//Implementation
//########################################
//////////////////////////////////////////////////////////////////
//Driver load/initialization routine.
//
//@param: None
//
//@return: 0: upon success
//
int32_t Hobart_init(void)
{
    int32_t ret=HOBART_USB_SUCCESS;

    printk(KERN_ALERT "Hobart_usb: %s called\n", __FUNCTION__);

    //
    //register the major num of the driver
    //
    /*******************shoudl delete
      ret = register_chrdev(Hobart_major, HOBART_MOD_NAME, &Hobart_fops);
      if (0 > ret)
      {
      printk(KERN_ALERT"Hobart_usb: %s-Unable to "
      "register major number %d\n", __FUNCTION__,
      Hobart_major);
      return -1;
      }
     *************************/
    //
    //register with the usb subsystem
    //
    ret = usb_register(&Hobart_udrv);
    if (0 > ret)
    {
        printk(KERN_ALERT"Hobart_usb: %s-Unable to register with"
                "usb sub-systemd\n", __FUNCTION__);
        return -1;
    }

    printk(KERN_ALERT"Hobart_usb: %s-Initialization complete\n",
            __FUNCTION__);
    return ret;

    //HOBART_INIT_FAIL:
    //      Hobart_exit();
    //      return ret;
}

//////////////////////////////////////////////////////////////////
//Driver unload/exit routine.
//
//@param: None
//
//@return: None
//
void Hobart_exit(void)
{
    printk(KERN_ALERT"Hobart_usb: %s-called\n",__FUNCTION__);

    //
    //-deregister the major number
    //-deregister from USB subsystem
    //
    /****************should delete
      unregister_chrdev(Hobart_major, HOBART_MOD_NAME);
     *******************/
    usb_deregister(&Hobart_udrv);
    printk(KERN_ALERT"HObart_usb: %s-Unloading driver\n",__FUNCTION__);
}

//////////////////////////////////////////////////////////////////
//Routine is called when user calls fopen or the likes.
//
//@param: i_node -
//@param: f_ptr - driver file structure
//
//@return: 0: upon success
//
int32_t Hobart_open(HOBART_PINODE i_node, HOBART_PFILE f_ptr)
{
    int32_t ret=HOBART_USB_SUCCESS;

    printk(KERN_ALERT"Hobart_usb: %s-called\n", __FUNCTION__);

    //
    //-save the private data in file struct
    //-update internal open count
    //
    ret = Hobart_getDrvContext(i_node, f_ptr);

    if (0 > ret)
    {
        printk(KERN_ALERT"Hobart_usb: %s-Device not present\n",
                __FUNCTION__);
        return ret;
    }
    if(NULL == f_ptr->private_data)
        printk(KERN_ALERT"%sGOT nothing\n", __FUNCTION__);
    kref_get(&((HOBART_PDRV_CONTEXT)f_ptr->private_data)->kref_cnt);

    printk(KERN_ALERT"Hobart_usb: %s-Done\n", __FUNCTION__);
    return ret;
}

//////////////////////////////////////////////////////////////////
//Routine is called when user calls fclose or the like.
//
//@param: i_node -
//@param: f_ptr - driver file structure
//
//@return: 0: upon success
//
int32_t Hobart_release(HOBART_PINODE i_node, HOBART_PFILE f_ptr)
{
    int32_t ret=HOBART_USB_SUCCESS;
    HOBART_PDRV_CONTEXT drv_contxt=f_ptr->private_data;

    printk(KERN_ALERT"Hobart_usb: %s-called\n",__FUNCTION__);
    //
    //decrement internal handle count and free
    //resources if all handles have been closed
    //
    if (NULL != drv_contxt)
    {
        printk(KERN_ALERT"Hobart_usb: %s- No device present\n",
                __FUNCTION__);
        return -ENODEV;
    }
    if (HOBART_RESET & drv_contxt->status)
    {
        printk(KERN_ALERT "Hobart_usb: %s called as result of "
                "reset\n", __FUNCTION__);
    }
    kref_put(&drv_contxt->kref_cnt, Hobart_freeResources);

    /**************************************
      if (0 >= --drv_contxt->open_cnt)
      {
      usb_unlink_urb(drv_contxt->out_urb);
      drv_contxt->open_cnt = 0;
      }
     **********enable
     MOD_DEC_USE_COUNT;
     ************
     if (NULL == drv_contxt->dev)
     {
    ///?????????????
     ***********enable
     Hobart_disconnect(drv_contxt->dev, drv_contxt);
     ***********
     }
     ***********************************/

    printk(KERN_ALERT"Hobart_usb: %s-Done\n",__FUNCTION__);
    return ret;
}

//////////////////////////////////////////////////////////////////
//Routine called when user calls fread or the like.
//
//@param: f_ptr - driver file structure
//@param: buf - User buffer with data
//@param: cnt - cont of bytes to transmit
//@param: f_pos - current write location of the file
//
//@return: 0: upon success
//
ssize_t Hobart_read(HOBART_PFILE f_ptr, TCHAR *buf, size_t cnt,
        HOBART_PLOFF f_pos)
{
    int32_t ret= -EFAULT;                   //char count read or error
    uint32_t bytes_recvd=0;
    //actual bytes received
    size_t bytes_to_get=0;
    //the max ammount that can be
    //read or the max asked for
    uint8_t cp_bytes=0;
    //bytes copied to user buffer
    //after reception
    HOBART_PDRV_CONTEXT drv_contxt=f_ptr->private_data;

    printk(KERN_ALERT "Hobart_usb: %s called\n", __FUNCTION__);

    if (0 >= cnt)
    {
        printk(KERN_ALERT"Hobart_usb: %s-Invalid "
                "argument passed\n", __FUNCTION__);
        return -EINVAL;
    }
    if (NULL == drv_contxt)
    {
        printk(KERN_ALERT"Hobart_usb: %s-No device\n",
                __FUNCTION__);
        return -ENODEV;
    }

    if (NULL == drv_contxt->dev)
    {
        printk(KERN_ALERT"Hobart_usb: %s-Invalid "
                "internal driver data\n", __FUNCTION__);
        return -ENODEV;
    }
    if (NULL == drv_contxt->in_buf)
    {
        printk(KERN_ALERT"Hobart_usb: %s-Unable to perform "
                "read on device-Invalid bulk buffer\n",
                __FUNCTION__);
        return -EINVAL;
    }


    //
    //usb_bulk_msg used to send data instead of
    //raw URB
    //
    bytes_to_get = min(drv_contxt->isize, cnt);
    ret = usb_bulk_msg(drv_contxt->dev,
            usb_rcvbulkpipe(drv_contxt->dev,
                drv_contxt->in_ep_addr),
            drv_contxt->in_buf,
            bytes_to_get,
            &bytes_recvd, HZ*10);
    printk(KERN_ALERT"Hobart_usb: %s-size to read %d-addr=%x\n",__FUNCTION__, drv_contxt->isize, drv_contxt->in_ep_addr);
    /******** Temp remove
      ret = usb_interrupt_msg(drv_contxt->dev,
      usb_rcvintpipe(drv_contxt->dev,
      drv_contxt->in_ep_addr),
      drv_contxt->in_buf,
      bytes_to_get,
      &bytes_recvd, HZ*100);
     ********/
    printk(KERN_ALERT"Hobart_usb: %s-read %d bytes\n",__FUNCTION__, bytes_recvd);
    if (0 > ret)
    {
        printk(KERN_ALERT"Hobart_usb: %s Failed to read "
                "bytes-%d\n", __FUNCTION__, ret);
        return ret;
    }
    else
    {
        /****use may specify smaller size than the required we may need to hold the
          data for later completion
         *********/
        if (bytes_to_get > bytes_recvd)
        {
            printk(KERN_ALERT"Hobart_usb: %s-"
                    "Too few bytes(%d) received. "
                    "Possible loss of data\n",
                    __FUNCTION__, bytes_recvd);
        }
        if (0 != (cp_bytes=copy_to_user(buf, drv_contxt->in_buf, bytes_recvd)))
        {
            printk(KERN_ALERT"Hobart_usb: %s-%d bytes "
                    "failed to be copied to user.\n",
                    __FUNCTION__, cp_bytes);
            return -EFAULT;
        }
        ret = bytes_recvd;
    }

    printk(KERN_ALERT"Hobart_usb: %s-Done\n",__FUNCTION__);
    return ret;
}
//////////////////////////////////////////////////////////////////
//Called when Bulk transmit completes.
//
//@param: active_urb- URB whoes completion caused us to be called
//
//@return:
//
void HOBART_blkCallback(HOBART_PURB active_urb)
{
    if ((0 != active_urb->status) &&
            ((active_urb->status == -ENOENT) ||
             (active_urb->status == -ECONNRESET) ||
             (active_urb->status == -ESHUTDOWN)))
    {
        printk(KERN_ALERT "Hobart_usb: %s-Unable to write data "
                "Error %d occured.\n", __FUNCTION__,
                active_urb->status);
    }

    /*********free urb if appropriate***/
    usb_buffer_free(active_urb->dev, active_urb->transfer_buffer_length,
            active_urb->transfer_buffer,
            active_urb->transfer_dma);
    /*****************************/
    printk(KERN_ALERT "Hobart_usb: %s-URB sent-status=%d\n", __FUNCTION__,
            active_urb->status);
}

//////////////////////////////////////////////////////////////////
//Routine called when user calls fwrite or the like.  Only supports
//writes of buffer size less than or equal to the pipe buffer size.
//
//
//@param: f_ptr - driver file structure
//@param: buf - User buffer with data
//@param: cnt - cont of bytes to transmit
//@param: f_pos - current write location of the file
//
//@return: 0: upon success
//
ssize_t Hobart_write(HOBART_PFILE f_ptr, const TCHAR *buf, size_t cnt,
        HOBART_PLOFF f_pos)
{
    int32_t ret= 0;                 //char count written or err
    uint8_t cp_bytes=0;
    HOBART_PDRV_CONTEXT drv_contxt=f_ptr->private_data;
    uint32_t bytes_to_snd;


    printk(KERN_ALERT "Hobart_usb: %s called\n", __FUNCTION__);

    if (0 >= cnt)
    {
        printk(KERN_ALERT"Hobart_usb: %s-Invalid "
                "argument passed\n", __FUNCTION__);
        return -EINVAL;
    }
    if (NULL == drv_contxt)
    {
        printk(KERN_ALERT"Hobart_usb: %s-Invalid "
                "internal driver data\n",__FUNCTION__);
        return -EINVAL;
    }
    if (NULL == drv_contxt->dev)
    {
        printk(KERN_ALERT"Hobart_usb: %s-No device\n",
                __FUNCTION__);
        return -ENODEV;
    }

    //################################################################
    //we may do this at a certral location once, if it works out
    //this we avoid memory segmentation with too many allocs and deletes
    //################################################################

    //
    //-allcoate the urb
    //
    //
    //****************osize is right now 0 must be initialized
    printk(KERN_ALERT"Hobart_usb: %s cnt is %d-context=%x-dev=%x\n", __FUNCTION__, cnt,
            (int)drv_contxt,(int)drv_contxt->dev);
    bytes_to_snd = (cnt > drv_contxt->osize) ?
        drv_contxt->osize : cnt;
    bytes_to_snd = cnt;
    printk(KERN_ALERT"Hobart_usb: %s Sending %d bytes cnt was %d---%d\n", __FUNCTION__, bytes_to_snd, cnt, drv_contxt->osize);
    //***************************
    drv_contxt->out_urb = usb_alloc_urb(0, GFP_KERNEL);
    if (NULL == drv_contxt->out_urb)
    {
        printk(KERN_ALERT"Hobart_usb: %s-Invalid URB\n", __FUNCTION__);
        return -ENOMEM;
    }
    drv_contxt->out_buf = usb_buffer_alloc(drv_contxt->dev, bytes_to_snd,
            GFP_KERNEL,
            &(drv_contxt->out_urb->transfer_dma));
    if (NULL == drv_contxt->out_buf)
    {
        printk(KERN_ALERT"Hobart_usb: %s-Invalid buf\n", __FUNCTION__);
        ret = -ENOMEM;
        goto HOBART_WRITE_ERROR;
    }

    if (0 != (cp_bytes=copy_from_user(drv_contxt->out_buf,
                    buf, bytes_to_snd)))
    {
        printk(KERN_ALERT "Hobart_usb: %s failed "
                "to copy %d bytes\n", __FUNCTION__, cp_bytes);
        ret = -EFAULT;
        goto HOBART_WRITE_ERROR;
    }

    usb_fill_int_urb(drv_contxt->out_urb,
            drv_contxt->dev,
            usb_sndintpipe(drv_contxt->dev,
                drv_contxt->out_ep_addr),
            drv_contxt->out_buf,
            bytes_to_snd, HOBART_blkCallback,
            drv_contxt,10);
    drv_contxt->out_urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

    ret = usb_submit_urb(drv_contxt->out_urb, GFP_KERNEL);

    if (0 > ret)
    {
        printk(KERN_ALERT "Hobart_usb: %s "
                "Unable to sumbit Urb-%d\n",
                __FUNCTION__, ret);
        goto HOBART_WRITE_ERROR;
    }
    ret = bytes_to_snd;
    usb_free_urb(drv_contxt->out_urb);


    printk(KERN_ALERT"Hobart_usb: %s-Done\n",__FUNCTION__);
    return ret;

HOBART_WRITE_ERROR:
    usb_buffer_free(drv_contxt->dev, bytes_to_snd, drv_contxt->out_buf,
            drv_contxt->out_urb->transfer_dma);
    usb_free_urb(drv_contxt->out_urb);
    //      kfree(drv_contxt->out_buf);
    return ret;
}

//////////////////////////////////////////////////////////////////
//Routine called when user calls fwrite or the like.
//
//@param: f_ptr - driver file structure
//@param: buf - User buffer with data
//@param: cnt - cont of bytes to transmit
//@param: f_pos - current write location of the file
//
//@return: 0: upon success
//
int32_t Hobart_ioctl(HOBART_PINODE i_node, HOBART_PFILE f_ptr, uint32_t
        code, unsigned long s)
{
    int32_t ret;
    HOBART_PDRV_CONTEXT drv_contxt= (HOBART_PDRV_CONTEXT)
        f_ptr->private_data;
    HOBART_PUSB_DEV udev;

    printk(KERN_ALERT"Hobart_usb: %s-called\n",__FUNCTION__);
    //
    //retrive device data to get to usb_dev struct
    //we'll need this for reset.
    //
    if ((NULL == drv_contxt) || (NULL == drv_contxt->dev))
    {
        printk(KERN_ALERT "Hobart_usb: %s Unable to retrive driver"
                " data\n", __FUNCTION__);
        return -ENODEV;
    }
    udev = drv_contxt->dev;
    switch(code)
    {
        case(HOBART_DEV_RESET):
            //
            //-update the status and reset usb device
            //
            printk(KERN_ALERT "Hobart_usb: %s Reset requested."
                    " \n", __FUNCTION__);
            drv_contxt->status = HOBART_RESET;
            ret = usb_reset_device(udev);
            break;
        default:
            printk(KERN_ALERT "Hobart_usb: %s Invalid IOCTL."
                    " \n", __FUNCTION__);
            ret = -EINVAL;
            break;
    }

    printk(KERN_ALERT"Hobart_usb: %s-Done\n",__FUNCTION__);
    return ret;
}
//////////////////////////////////////////////////////////////////
//Retrieves hobart driver data from usb_interface structure.
//Hobart driver data is stored into the sub_interface structure
//at device plugin.
//
//@param: HOBART_PINODE-inode structure passed to driver open/
//                      disconnect APIs
//@param: f_ptr - driver file structure
//
//@return: HOBART_SUCCESS: upon success else approrpriate error.
//
int32_t Hobart_getDrvContext(HOBART_PINODE inode,
        HOBART_PFILE f_ptr)
{
    HOBART_PUSB_IFACE intf;
    HOBART_PDRV_CONTEXT drv_contxt;
    int32_t sub_minor;

    printk(KERN_ALERT"Hobart_usb: %s-Called\n", __FUNCTION__);
    //
    //we dont check for valid pointers to optamize the
    //code a bit.  Caller should send us valid data and
    //handle any errors
    //
    //retrieve device specific struct
    //from the interface struct
    //
    sub_minor = iminor(inode);

    intf = usb_find_interface(&Hobart_udrv, sub_minor);
    if (NULL == intf)
    {
        printk(KERN_ALERT"Hobart_usb: %s-Unable to "
                "find driver for minor %d\n", __FUNCTION__, sub_minor);
        return -ENODEV;
    }
    printk(KERN_ALERT"Hobart_usb: %s-intf=%x\n", __FUNCTION__, (int) intf);

    //      f_ptr->private_data = usb_get_intfdata(intf);
    drv_contxt = usb_get_intfdata(intf);
    f_ptr->private_data = drv_contxt;
    if (NULL == f_ptr->private_data)
    {
        printk(KERN_ALERT"Hobart_usb: %s-No device data found %d\n",
                __FUNCTION__, sub_minor);
        return -ENODEV;
    }
    printk(KERN_ALERT"Hobart_usb: %s-private dat=%x\n", __FUNCTION__, (int)f_ptr->private_data);
    printk(KERN_ALERT"Hobart_usb: %s-Done\n", __FUNCTION__);
    return HOBART_USB_SUCCESS;

}
//////////////////////////////////////////////////////////////////
//Allocates resources for the driver
//
//@param: None
//
//@return: HOBART_PDRV_CONTEXT: pointer to driver_context struct.
//
HOBART_PDRV_CONTEXT Hobart_allocDrvResources(void)
{
    HOBART_PDRV_CONTEXT context;

    printk(KERN_ALERT "Hobart_usb: %s called\n", __FUNCTION__);

    context = (HOBART_PDRV_CONTEXT)
        kmalloc(sizeof(HOBART_DRV_CONTEXT), GFP_KERNEL);
    if (NULL == context)
    {
        printk(KERN_ALERT "Hobart_usb: %s-Unable to allocate "
                "memory\n", __FUNCTION__);
        return NULL;
    }
    /*********enable
      context->out_urb = usb_alloc_urb();
      if (NULL == context->out_urb)
      {
      printk(KERN_ALERT "Hobart_usb: "
      "Hobart_allocDrvResources "
      "unable to allocate URB\n");
      return NULL;
      }
     ***********/

    printk(KERN_ALERT"Hobart_usb: %s-drv_contxt=%x Done\n",
            __FUNCTION__, (int)context);
    return context;
}
#define Hobart_getDev(d)        container_of(d, HOBART_DRV_CONTEXT, kref_cnt)
//////////////////////////////////////////////////////////////////
//Allocates resources for the driver
//
//@param: HOBART_PDRV_CONTEXT - pointer to driver_context struct.
//
//@return: None
//
//void Hobart_freeResources(HOBART_PDRV_CONTEXT drv_contxt)
void Hobart_freeResources(HOBART_PKREF kref)
{
    HOBART_PDRV_CONTEXT drv_contxt;
    printk(KERN_ALERT "Hobart_usb: %s called\n", __FUNCTION__);

    /****enable
      minor_table[drv_contxt->minor] = NULL;
     *****/

    //
    //no need to check for drv+contxt being valid
    //as we have kref that's valid.
    //
    drv_contxt = Hobart_getDev(kref);

    usb_put_dev(drv_contxt->dev);
    if (NULL != drv_contxt->in_buf)
        kfree(drv_contxt->in_buf);
    if (NULL != drv_contxt->out_buf)
        kfree(drv_contxt->out_buf);
    if (NULL != drv_contxt->in_urb)
        usb_free_urb(drv_contxt->in_urb);
    if (NULL != drv_contxt->out_urb)
        usb_free_urb(drv_contxt->out_urb);

    printk(KERN_ALERT"Hobart_usb: %s-Done\n",__FUNCTION__);
}
//////////////////////////////////////////////////////////////////
//Retrieves Endpoint info for the printer.
//
//@param: iface_desc- interface descriptor.  No error checking done
//                      for optimization.
//@param: drv_contxt - driver data.  No error checking done for
//                      for optimization.
//
//@return: HOBART_SUCCESS: upon success else error code.
//
int Hobart_getBlkIface(HOBART_PHOST_IFACE iface_desc,
        HOBART_PDRV_CONTEXT drv_contxt)
{
    int i;
    HOBART_PEP_DESC ep;
    uint32_t ret=HOBART_USB_SUCCESS;
    uint8_t buf_size;

    printk(KERN_ALERT "Hobart_usb: %s called\n", __FUNCTION__);
    for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i)
    {
        ep = &iface_desc->endpoint[i].desc;
        printk(KERN_ALERT "Hobart_usb: %s "
                "ep=%x ", __FUNCTION__,
                ep->bEndpointAddress);
        //              if (usb_pipebulk(ep->bEndpointAddress))
        if (( (ep->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)==
                    USB_ENDPOINT_XFER_BULK))
        {
            printk(KERN_ALERT "Hobart_usb: %s is Bulk ",
                    __FUNCTION__);
            if (!(drv_contxt->in_ep_addr) &&
                    (usb_pipein(ep->bEndpointAddress)))
            {
                //
                //we found a bulk in endpoint
                //
                buf_size = ep->wMaxPacketSize;
                drv_contxt->isize = buf_size;
                drv_contxt->in_ep_addr = ep->bEndpointAddress;
                printk(KERN_ALERT "Hobart_usb: %s Type IN\n",
                        __FUNCTION__);
                drv_contxt->in_buf =
                    kmalloc(buf_size, GFP_KERNEL);
                if (!drv_contxt->in_buf)
                {
                    printk(KERN_ALERT "Hobart_usb: %s-"
                            "Could not allocate in_buf\n",
                            __FUNCTION__);
                    ret = -ENOMEM;
                    return ret;
                }
            }//if bulk in endpoint
            if (!(drv_contxt->out_ep_addr) &&
                    (usb_pipeout(ep->bEndpointAddress)))
            {
                //
                //we found a bulk out endpoint
                //
                drv_contxt->out_ep_addr = ep->bEndpointAddress;
                printk(KERN_ALERT "Hobart_usb: %s Type OUT\n",
                        __FUNCTION__);
                buf_size = ep->wMaxPacketSize;
                drv_contxt->osize = buf_size;
                drv_contxt->out_buf =
                    kmalloc(buf_size, GFP_KERNEL);
                if (!drv_contxt->out_buf)
                {
                    printk(KERN_ALERT "Hobart_usb: %s- "
                            "Could not allocate out_buf\n",
                            __FUNCTION__);
                    ret = -ENOMEM;
                    return ret;
                }
                printk(KERN_ALERT HOBART_MOD_NAME":%s-%d",
                        __FUNCTION__, drv_contxt->osize);
            }//if bulk out endpoint
        }//if interrupt pipe
    }//for
    if (!(drv_contxt->in_ep_addr && drv_contxt->out_ep_addr))
    {
        printk(KERN_ALERT "Hobart_usb: %s-"
                "Could not find both bulk-in and bulk-out "
                "endpoints\n",__FUNCTION__);
    }
    printk(KERN_ALERT "Hobart_usb: %s Done\n", __FUNCTION__);
    return ret;
}
//////////////////////////////////////////////////////////////////
//Retrieves Endpoint info for the weigher.
//
//@param: iface_desc- interface descriptor.  No error checking done
//                      for optimization.
//@param: drv_contxt - driver data.  No error checking done for
//                      for optimization.
//
//@return: HOBART_SUCCESS: upon success else error code.
//
int Hobart_getIntrIface(HOBART_PHOST_IFACE iface_desc,
        HOBART_PDRV_CONTEXT drv_contxt)
{
    int i;
    HOBART_PEP_DESC ep;
    uint32_t ret=HOBART_USB_SUCCESS;
    uint8_t buf_size;

    printk(KERN_ALERT "Hobart_usb: %s called\n", __FUNCTION__);

    for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i)
    {
        ep = &iface_desc->endpoint[i].desc;
        printk(KERN_ALERT "Hobart_usb: %s "
                "ep=%x ", __FUNCTION__,
                ep->bEndpointAddress);
        //              if (usb_pipeint(ep->bEndpointAddress))
        if (( (ep->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)==
                    USB_ENDPOINT_XFER_INT))
        {
            printk(KERN_ALERT "Hobart_usb: %s "
                    "is Intr\n", __FUNCTION__);
            if (!(drv_contxt->in_ep_addr) &&
                    (usb_pipein(ep->bEndpointAddress)))
            {
                //
                //we found a bulk in endpoint
                //
                buf_size = ep->wMaxPacketSize;
                drv_contxt->isize = buf_size;
                drv_contxt->in_ep_addr = ep->bEndpointAddress;
                printk(KERN_ALERT "Hobart_usb: %s Type IN\n",
                        __FUNCTION__);
                drv_contxt->in_buf =
                    kmalloc(buf_size, GFP_KERNEL);
                if (!drv_contxt->in_buf)
                {
                    printk(KERN_ALERT "Hobart_usb: %s-"
                            "Could not allocate in_buf\n",
                            __FUNCTION__);
                    ret = -ENOMEM;
                    return ret;
                }
            }//if bulk in endpoint
            if (!(drv_contxt->out_ep_addr) &&
                    (usb_pipeout(ep->bEndpointAddress)))
            {
                //
                //we found a bulk out endpoint
                //
                drv_contxt->out_ep_addr = ep->bEndpointAddress;
                printk(KERN_ALERT "Hobart_usb: %s Type OUT\n",
                        __FUNCTION__);
                buf_size = ep->wMaxPacketSize;
                drv_contxt->osize = buf_size;
                drv_contxt->out_buf =
                    kmalloc(buf_size, GFP_KERNEL);
                if (!drv_contxt->out_buf)
                {
                    printk(KERN_ALERT "Hobart_usb: %s- "
                            "Could not allocate out_buf\n",
                            __FUNCTION__);
                    ret = -ENOMEM;
                    return ret;
                }
                printk(KERN_ALERT HOBART_MOD_NAME":%s-%d",
                        __FUNCTION__, drv_contxt->osize);
            }//if bulk out endpoint
        }//if interrupt pipe
    }//for
    if (!(drv_contxt->in_ep_addr && drv_contxt->out_ep_addr))
    {
        printk(KERN_ALERT "Hobart_usb: %s-"
                "Could not find both bulk-in and bulk-out "
                "endpoints\n",__FUNCTION__);
    }
    printk(KERN_ALERT "Hobart_usb: %s Done\n", __FUNCTION__);
    return ret;
}
//////////////////////////////////////////////////////////////////
//Rotuine handles device arrival.
//
//@param: usb_dev- usb_device structure which holds usb handlers etc
//@param: interface - usb interface.
//@param: id_tab - usb dev id table containing ids of supported devs
//
//@return: HOBART_SUCCESS: upon success else error code.
//
int Hobart_probe(HOBART_PUSB_IFACE usb_iface,
        const struct usb_device_id *id_table)
{
    HOBART_PDRV_CONTEXT drv_contxt=NULL;
    uint8_t valid_dev=true;
    HOBART_PHOST_IFACE iface_desc;
    uint32_t ret=HOBART_USB_SUCCESS;

    printk(KERN_ALERT "Hobart_usb: %s called\n", __FUNCTION__);

    if (true == valid_dev)
    {
        //
        //- allocate driver specific resoruces & init it
        //- update instance counter
        //
        drv_contxt = Hobart_allocDrvResources();
        if (NULL == drv_contxt)
        {
            printk(KERN_ALERT "Hobart_usb: %s- "
                    "Printer device\n",__FUNCTION__);
            ret = -ENOMEM;
            goto HOBART_PROBE_ERR;
        }
        memset(drv_contxt, 0x00, sizeof(HOBART_DRV_CONTEXT));
        kref_init(&drv_contxt->kref_cnt);

        drv_contxt->dev = usb_get_dev(interface_to_usbdev(usb_iface));
        drv_contxt->iface = usb_iface;
        printk(KERN_ALERT "Hobart_usb: %s-%s devnum=%d\n\n", __FUNCTION__, drv_contxt->dev->devpath, drv_contxt->dev->devnum);
        //
        //set up the endpoint information
        //use only the first bulk-in and bulk-out endpoints
        //
        //usb_set_interface(usb_iface, 0, 1);
        iface_desc = usb_iface->cur_altsetting;
        printk(KERN_ALERT "Hobart_usb: ep cnt=%d-length=%d\n", iface_desc->desc.bNumEndpoints, iface_desc->desc.bLength);
        //              if (HOB_PRINTER_RAW_PID == pid)
        //Hobart_getBlkIface(iface_desc, drv_contxt);
        //              else
        Hobart_getIntrIface(iface_desc, drv_contxt);
        //
        //-save driver internal structure for use with
        //other callbacks
        //-register the dev since it's ready
        //
        usb_set_intfdata(usb_iface, (void *)drv_contxt);

        ret = usb_register_dev(usb_iface, &Hobart_class);
        if(0 != ret)
        {
            printk(KERN_ALERT "Hobart_usb: %s-Unable to register driver\n",
                    __FUNCTION__);

            goto HOBART_PROBE_ERR;
        }

        drv_contxt->status = HOBART_RUNNING;
    }//if our device

    printk(KERN_ALERT "Hobart_usb: %s-Done\n", __FUNCTION__);
    return ret;
HOBART_PROBE_ERR:
    kref_put(&drv_contxt->kref_cnt, Hobart_freeResources);
    return ret;
}
//////////////////////////////////////////////////////////////////
//Rotuine handles device removal.
//
//@param: iface-usb_interface
//
//@return: 0: upon success
//
void Hobart_disconnect(HOBART_PUSB_IFACE iface)
{
    HOBART_PDRV_CONTEXT drv_contxt=NULL;

    printk(KERN_ALERT "Hobart_usb: %s called\n", __FUNCTION__);

    //
    //-block all opens on the device
    //-retrieve driver data
    //-unregister the device
    //
    /**********************
      we would need to make sure that the logic here waits for any Async reads
      writes...at the moment all are Sync reads
     **********************/

    lock_kernel();

    drv_contxt = usb_get_intfdata(iface);
    usb_set_intfdata(iface, NULL);

    if (NULL == drv_contxt)
    {
        printk(KERN_ALERT"Hobart_usb: Hobart_disconnect-Invalid "
                "internal driver data\n");
        return;
    }
    drv_contxt->status = HOBART_STOPPED;
    if (NULL == drv_contxt->dev)
    {
        printk(KERN_ALERT"Hobart_usb: %s-No device\n",
                __FUNCTION__);
        return;
    }
    usb_deregister_dev(iface, &Hobart_class);
    unlock_kernel();

    kref_put(&drv_contxt->kref_cnt, Hobart_freeResources);
    /**********enable
      MOD_DEC_USE_COUNT;
     ************/
    if (0 < drv_contxt->open_cnt)
    {
        //
        //-set remove pending flag
        //-wake up everyone who needs to wake up
        //-wait until we are ready to release dev
        //-deregister from devfs
        //-free resources
        //-decrement our instance count
        //
        /**************enable
          drv_contxt->remove_pending = true;

          wake_up(&drv_contxt->wait);

          sleep_on(drv_contxt->remove_ok);

          devfs_unregister(drv_contxt->devfs);
        //maintain a flag to keep track if an urb has been submitted
        //and not completed yet.. call usb_unlink_urb(drv_context->out_urb);
        //to cancel if so.  Better in this routine is usb_kill_urb(...);
         **************/
        //              Hobart_freeResources(drv_contxt);
    }
    /**************enable
      else
      {
      drv_contxt->dev = NULL;
      up(&drv_contxt->sem);
      }

     **************/
    printk(KERN_ALERT"Hobart_usb: %s-Done\n", __FUNCTION__);
}















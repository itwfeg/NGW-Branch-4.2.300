#include"Hobart_usb.h"

#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>
#include<sys/ioctl.h>
#include<inttypes.h>

/****
#include<semaphore.h>
#include<time.h>
#include<pthread.h>
#include<xlocale.h>
#include<locale.h>
#include<limits.h>
 ****/

//#include<string>
//#include<fstream>


void ReadFile(int fd)
{
    char buf[10000];
    //  memset(buf, 0);
    int ret = read(fd, buf, 5000);
    if (0 > ret)
    {
        switch(errno)
        {
            case(EAGAIN):
                //O_NONBLOCK-overlapped
                printf("Data not available immidately\n");
                break;
            case(EBADF):
                printf("BAD file descriptor\n");
                break;
            case(EFAULT):
                printf("Invalid buf\n");
                break;
            case(EINTR):
                printf("Read got interrupted by a signal\n");
                break;
            case(EINVAL):
                printf("Device unavailable for reads\n");
                break;
            case(EIO):
                printf("I/O error\n");
                break;
            case(EISDIR):
                printf("fd is a dir\n");
                break;
            case(ENODEV):
                printf("No device is plugged in\n");
                break;
            default:
                printf("Read failed with error %d\n", ret);
                break;
        }//switch
    }//if read fails
    else
        printf("%d bytes in buffer %s\n", ret, buf);
}

int WriteFile(int fd)
{
    printf("Write entered\n");
    struct something
    {
        uint32_t src_addr;
        uint32_t dest_addr;
        uint16_t len;
        uint8_t s;
        uint8_t q;
    };
    struct something ss={src_addr:0x00000009, dest_addr: 0x00000008, 0x0010, s:1, q:0};
    struct something pp={src_addr:0x09000000, dest_addr: 0x08000000, 0x1000, s:1, q:0};
    //char buf[]={0x09, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x10, 0x00, 0x01, 0x00};
    char buf[]={0x00, 0x00, 0x00, 0x00, 0x46, 0x46, 0x00, 0x00, 0x00, 0x02, 0x02, 0x00};
    printf("sending %s\n", buf);

    int ret = write(fd, buf, 12);

    if (0 > ret)
    {
        perror("WriteFile:");
        /*
        switch(ret)
        {
            case(EINVAL):
                printf("Invalid value\n");
                break;
            case(ENOMEM):
                printf("Out of memory\n");
                break;
            default:
                printf("Write failed %d\n", ret);
                break;
        }
        perror("WriteFile:");
        */
    }
    else
        printf("sent\n");

    return ret;

}

int main(int argc, char *argv[])
{
    // char path[]="/sys/bus/usb/drivers/Hobart_usb";
    // char path[]="/dev/bus/usb/005/003";
    //char path[]="/dev/Hobart_usb0\0";
    // char path[]="/sys/devices/pci0000:00/0000:00:1d.0/usb2/2-1/2-1.4/2-1.4:1.0/usb/Hobart_usb0";

    int cnt=0;
    // char path[]="/dev/raw_printer0\0";
    // char path[]="/dev/Hobart/printer0\0";
    // char path[]="/dev/raw_weigher0\0";
    // char path[]="/dev/Hobart/weigher0\0";
    // char path[]="/dev/min_cyp1\0";

    char path[]="/dev/Hobart/raw_printer0";
    // char path[]="/dev/Hobart/raw_weigher4";
    // char path[]="/dev/bus/usb/002/016";

    int fd = open(path, O_RDWR|O_NOCTTY|O_NONBLOCK|O_NDELAY);
    if (-1 == fd)
    {
        printf("Path=%s err = %m- %d\n", path, errno);
    }
    else
    {

        /***8
          if (-1 == ioctl(fd, HOBART_DEV_RESET))
          {
          printf("Unable to send ioctl--%m\n");
          }
          else
          printf("Ioctl sent\n");
          sleep(1);
         *****/

        if (WriteFile(fd) >= 0)
            sleep(60);
        return 1;
        while (1)
        {
            WriteFile(fd);
            ReadFile(fd);
            sleep(1);
            printf("XXXXX\n");
            //printf("ok done with read # %d\n", ++cnt);
        }
        /*****
         *****/
    }
    return 1;
}

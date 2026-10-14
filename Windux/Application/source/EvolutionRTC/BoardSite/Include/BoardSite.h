/**
 * @file BoardSite.h
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Aaron Swift
 * @brief
 * Created on: Feb 3, 2020 
 * @{
 */
#ifndef BOARDSITE_H
#define BOARDSITE_H



typedef enum
{
    _ARK_NO_TEST,
    _ARK_IO,                    //
    _ARK_CONFIGURATION_IO,      //tests display, memory type, memory capacity IO
    _ARK_CAN_0,
    _ARK_CAN_1,
    _ARK_AUDIO,
    _ARK_SERIAL_FLASH,
    _ARK_SD_SLOT_0,
    _ARK_SD_SLOT_1,
    _ARK_RTC,
    _ARK_USB,    
    _ARK_WIFI,
    _ARK_BLUE,
    _ARK_ETHERNET,
    _ARK_LVDS0,
    _ARK_LVDS1,    
    _ARK_MEMORY,
    _KPC_MODEL,
    _KPC_IO,
    _KPC_RS485_SETUP,
    _KPC_RS485_TRANSFER,
    _KPC_VALUEMAX,
    _KPC_SERIAL_FLASH,
    _KPC_CAN0,
    _KPC_CUTTER,
    _KPC_DOTWEAR,
    _KPC_PRINT_HEAD,
    _KPC_STEPPER,
    _KPC_MEDIA,
    _KPC_WGH_ADC,
    _KPC_WGH_SWITCH,
    _KPC_FAN,
    _KPC_LABEL_TAKEN,
    _ARK_RESET_BUTTON,
}BoardSiteTests;

typedef enum
{
    _IO_IN,
    _IO_OUT
}IO_Direction;

typedef enum
{
    _IO_LOW,
    _IO_HIGH
}IO_State;

typedef enum
{
    _VideoRed,
    _VideoBlue,
    _VideoGreen,
    _BacklightOn,
    _Backlight5V,
    _Backlight12V,
    _BacklightOff

}LVDS_Test;

#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h> 
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "euble_interface.h"
#include "sim_com_debug.h"
#include "sim_com_serial.h"

#ifdef __cplusplus
extern "C" {
#endif

_INT32 ComnIoctlRegOption(_UINT32 add, _UINT32 val, _UINT32 mask);
_INT32 ComnIoctlGpioInit(_UCHAR8 gpio, _UCHAR8 dir, _UCHAR8 val);
_INT32 ComnIoctlGpioSetValue(_UCHAR8 gpio, _UCHAR8 val);
_INT32 ComnIoctlGpioGetValue(_UCHAR8 gpio);



int Char2Int(unsigned char charVal)
{
    if(('0' <= charVal) && (charVal <= '9'))
    {
        return (charVal - '0');
    }
    else if(('a' <= charVal) && (charVal <= 'f'))
    {
        return (charVal - 'a' + 10);
    }
    else if(('A' <= charVal) && (charVal <= 'F'))
    {
        return (charVal - 'A' + 10);
    }
    else
        return 0;
}
int Int2Char(unsigned char val)
{
    if((0 <= val) && (val <= 9)){
        return (val + 0x30);
    }else if((0xa <= val) && (val <= 0xf)){
        return (val - 0xa + 'a');
    }
}

_INT32 SimComSerialInit(_VOID)
{
    _INT32 fd_dev = -1;

#if SIMDATA_IO_UART
    ComnIoctlRegOption(REG_GPIO1_MODE, 0x50044454, 0x3<<10); 
    ComnIoctlGpioInit(SIM_COM_SIM_DET_IOB, GPIO_DIR_IN, 1);        //MDI_RN_P3 GPIO22
    ComnIoctlRegOption(REG_GPIO2_MODE, 0x05540554, 0x3<<4);    //small card,
    ComnIoctlGpioInit(SIM_COM_SIM_DET_IOS, GPIO_DIR_IN, 0);
#endif

    euble_init(NULL);
    fd_dev = euble_open("BLUETOOTH");
    return fd_dev;
}

_INT32 SimComSerialUninit(_INT32 fd)
{
    return euble_close(fd);
}


/*
 *write data to serial
 *fd      :serial fd
 *p_data  :pointer of store
 *length  :data length
 */
_INT32 SimComSerialWrite(_INT32 fd, _UCHAR8* p_data, _UINT32 length)
{
    _UCHAR8 dataBuf[1024] = {};
    _UINT32 oldPtr = 0, newPtr = 0;

    //dataBuf[0] = '0';
    //dataBuf[1] = 'x';
    //dataBuf[2] = Int2Char(length >> 4);
    //dataBuf[3] = Int2Char(length & 0xf);
    //newPtr += 2;
    while(oldPtr < length){
        dataBuf[newPtr] = Int2Char(*(p_data + oldPtr) >> 4);
        dataBuf[newPtr + 1] = Int2Char(*(p_data + oldPtr) & 0xf);
        newPtr += 2;
        oldPtr++;
    }
    return euble_write(fd, dataBuf, newPtr);
}

/*
 *read data from serial
 *fd      :serial fd
 *p_data  :pointer of store
 */
_INT32 SimComSerialRead(_INT32 fd, _UCHAR8* p_data)
{
    _INT32 length = -1;
    _CHAR8 dataBuf = 0;
    _INT32 newPtr=0,oldPtr=0;
	
    memset(p_data, 0, 1024);
	length = euble_read(fd, p_data, 1024, 0);
    while(oldPtr < length){
        dataBuf = Char2Int(*(p_data + oldPtr))*16 + Char2Int(*(p_data + oldPtr + 1));
        *(p_data + newPtr) = dataBuf;
        newPtr++;
        oldPtr += 2;
    }
    #if 0
    {
        _CHAR8 printBuff[1024];
        _INT32 uiPrintLen;
        _INT32 i=0;

        uiPrintLen = sprintf(printBuff, "SerialRd:(%d):", newPtr);
        for(i=0; i<newPtr; i++)
        {
            uiPrintLen += sprintf(printBuff, "0x%02x ",*(p_data+i));
        }
        sprintf(printBuff+uiPrintLen,"\n");
        SIMCOM_DBG("%s",printBuff);
    }
    #endif
	return newPtr;
}

/* Reset Sim Card	*/
_INT32 SimComCardReset(_INT32 fd)
{
    //return EOS_OK;
   return euble_reset(fd);
}

_INT32 SimComCardDetect(_UINT32 *pRetVal)
{
	_UINT32 sim_detect_val = 0;
    if(ComnIoctlGpioGetValue(SIM_COM_SIM_DET_IOB) == 0)
	{
		sim_detect_val = 1;
	}
    *pRetVal = sim_detect_val;
	return EOS_OK;
}


/*********************Local Function***********************************************/
_INT32 ComnIoctlRegOption(_UINT32 add, _UINT32 val, _UINT32 mask)
{
	int fd = -1;
	_UINT32 reg_val[2] = {0, 0};
	
	fd = open(REG_DEV_NAME , O_RDWR|O_NOCTTY|O_NDELAY);
	if(fd < 0)
	{
		SIMCOM_INFO("Cann't Open %s\n", REG_DEV_NAME);
		return EOS_ERROR;
	}
	//set Reset IO Output
	reg_val[0] = add;
	read(fd, reg_val, 8);
	reg_val[1] &= ~mask;	
	reg_val[1] |= (val & mask);		
	write(fd, reg_val, 8);

	close(fd);
	return EOS_OK;
}

/*
 *init the gpio function.
 *gpio:number of the pin£¬dir:in=0/out=1£¬val:voltage
 */
_INT32 ComnIoctlGpioInit(_UCHAR8 gpio, _UCHAR8 dir, _UCHAR8 val)
{
	_INT32 fd = -1;
	_UINT32 reg_val[2] = {0, 0};
	_UINT32 gpio_ctrl_reg_add = 0;
	_UINT32 gpio_data_reg_add = 0;
	
	if(gpio < 32)
	{
		gpio_ctrl_reg_add = REG_GPIO_CTRL0;
		gpio_data_reg_add = REG_GPIO_DATA0;
	}
	else if(gpio >= 32)
	{
		gpio_ctrl_reg_add = REG_GPIO_CTRL1;
		gpio_data_reg_add = REG_GPIO_DATA1;
		gpio -= 32;
	}
	
	fd = open(REG_DEV_NAME , O_RDWR|O_NOCTTY|O_NDELAY);
	if(fd < 0)
	{
		SIMCOM_INFO("Cann't Open %s\n", REG_DEV_NAME);
		return EOS_ERROR;
	}
	//set the io port,out or in
	reg_val[0] = gpio_ctrl_reg_add;
	read(fd, reg_val, 8);
	
	if(dir == GPIO_DIR_OUT)
	{
		reg_val[1] |= 1 << gpio;
	}
	else if(dir == GPIO_DIR_IN)
	{
		reg_val[1] &= ~(1 << gpio);
	}
	write(fd, reg_val, 8);
	
    //set the val
    reg_val[0] = gpio_data_reg_add;
	read(fd, reg_val, 8);
    if(val > 0)
	{
	    reg_val[1] |= 1 << gpio;
	}
	else
	{
		reg_val[1] &= ~(1 << gpio);
	}
    write(fd, reg_val, 8);
	close(fd);
	
	return EOS_OK;
}

_INT32 ComnIoctlGpioSetValue(_UCHAR8 gpio, _UCHAR8 val)
{
	_INT32 fd = -1;
	_UINT32 reg_val[2] = {0, 0};
	_UINT32 gpio_data_reg_add = 0;
    
	if((gpio > 63) || (val > 1))
	{
		SIMCOM_INFO("paramer is wrong!\n");
		return EOS_ERROR;
	}
	
	if(gpio < 32)
	{
		gpio_data_reg_add = REG_GPIO_DATA0;
	}
	else if(gpio >= 32)
	{
		gpio_data_reg_add = REG_GPIO_DATA1;
		gpio -= 32;
	}
    
	fd = open(REG_DEV_NAME , O_RDWR|O_NOCTTY|O_NDELAY);
	if(fd < 0)
	{
		SIMCOM_INFO("Cann't Open %s\n", REG_DEV_NAME);
		return EOS_ERROR;
	}
	//output 0/1
    reg_val[0] = gpio_data_reg_add;
    read(fd, reg_val, 8);
    if(val == 0)
	{
        reg_val[1] &= ~(1 << gpio);
    }
	else if(val == 1)
	{
        reg_val[1] |= 1 << gpio;
    }
	write(fd, reg_val, 8);
	
	close(fd);
	return EOS_OK;
}

_INT32 ComnIoctlGpioGetValue(_UCHAR8 gpio)
{
    _INT32 fd = -1;
    _UINT32 reg_val[2] = {0, 0};
    _UINT32 gpio_data_reg_add = 0;
    
    if(gpio > 63)
    {
        SIMCOM_INFO("paramer is wrong!\n");
        return EOS_ERROR;
    }
    
    if(gpio < 32)
    {
        gpio_data_reg_add = REG_GPIO_DATA0;
    }
    else if(gpio >= 32)
    {
        gpio_data_reg_add = REG_GPIO_DATA1;
        gpio -= 32;
    }
    
    fd = open(REG_DEV_NAME , O_RDWR|O_NOCTTY|O_NDELAY);
    if(fd < 0)
    {
        SIMCOM_INFO("Cann't Open %s\n", REG_DEV_NAME);
        return EOS_ERROR;
    }
    //output 0/1
    reg_val[0] = gpio_data_reg_add;
    read(fd, reg_val, 8);
    close(fd);
    
    return ((reg_val[1] >> gpio) & 1);
}

#ifdef __cplusplus
}
#endif



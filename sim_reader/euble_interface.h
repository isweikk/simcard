#ifndef __EOU_EUBLE_INTERFACE_H
#define __EOU_EUBLE_INTERFACE_H
//Created by Jeff.Wong@EOUTECH on 20160308
#include <string.h>


//0:x86 1:Android 2:iOS
#define OS_TYPE 1
/////////////////////////////////////////////////////////////////////////////////
//log
/////////////////////////////////////////////////////////////////////////////////
#if (OS_TYPE == 1)
#define DBG(format, ...)  printf("|%15.15s:%04d| "format"", __FILE__, __LINE__, ##__VA_ARGS__);
#define LOG(format, ...)  printf("|%15.15s:%04d| "format"", __FILE__, __LINE__, ##__VA_ARGS__);
#define LOGI(format, ...) printf("|%15.15s:%04d| "format"", __FILE__, __LINE__, ##__VA_ARGS__);
#define ERR(format, ...)  printf("|%15.15s:%04d| "format"", __FILE__, __LINE__, ##__VA_ARGS__);

#elif (OS_TYPE == 2)
#include <jni.h>
#include <android/log.h>
#define DBG(format, ...)  __android_log_print(ANDROID_LOG_DEBUG, "JNIEUBLEC", format, ## __VA_ARGS__);
#define LOG(format, ...)  __android_log_print(ANDROID_LOG_INFO, "JNIEUBLEC", format, ## __VA_ARGS__);
#define LOGI(format, ...) __android_log_print(ANDROID_LOG_INFO, "JNIEUBLEC", format, ## __VA_ARGS__);
#define ERR(format, ...)  __android_log_print(ANDROID_LOG_ERROR, "JNIEUBLEC", format, ## __VA_ARGS__);
#endif


/////////////////////////////////////////////////////////////////////////////////
//@brief
//  init jnienv
/////////////////////////////////////////////////////////////////////////////////
//void euble_init(JNIEnv* env);
void euble_init(void* env);


/////////////////////////////////////////////////////////////////////////////////
///@brief
//	Given a name for a bluetooth device, returns a file descriptor for use in subsequent system calls
///@return 
//	return the bluetooth device file descriptor, or -1 if an error occurred. 
/////////////////////////////////////////////////////////////////////////////////
int euble_open(const char* device_name);



/////////////////////////////////////////////////////////////////////////////////
///@brief
//	closes a file descriptor, so that it no longer refers to any file and may be reused.
///@return 
//	returns zero on success.  On error, -1 is returned. 
/////////////////////////////////////////////////////////////////////////////////
int euble_close(int fd);



/////////////////////////////////////////////////////////////////////////////////
///@brief
//	reset the bluetooth device status
///@return 
//	returns zero on success.  On error, -1 is returned. 
/////////////////////////////////////////////////////////////////////////////////
int euble_reset(int fd);



/////////////////////////////////////////////////////////////////////////////////
//@brief
//	writes up to count bytes from the buffer pointed buf to the file referred to by the file descriptor fd.
///@return 
//	On success, the number of bytes written is returned (zero indicates nothing was written).  
//	On error, -1 is returned
/////////////////////////////////////////////////////////////////////////////////
int euble_write(int fd, const void* mybuf, size_t count);


/////////////////////////////////////////////////////////////////////////////////
//@brief
//	 attempts to read up to count bytes from file descriptor fd into the buffer starting at buf.
//@param
//	fd      - file descriptor
//	buf     - read buffer
//	count   - count bytes
//	blockms - block millisecond, 0 means no block 
//@return 
//	On success,  the  number of bytes read is returned (zero indicates end of file)
//	On error, -1 is returned
/////////////////////////////////////////////////////////////////////////////////
int euble_read(int fd, void* buf, size_t count, int blockms);




#endif

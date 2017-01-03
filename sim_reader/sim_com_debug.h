
#ifndef SIM_COM_DEBUG_H
#define SIM_COM_DEBUG_H

//wkk20161228
#define __OS_ANDROID__  0

/* 2009.11.23 pengjian added */

#define DEBUG_MODULE_SIMCOM       "ESPC"

#define DEBUG_UNIT_SIMCOM_ERROR   "ERROR"
#define DEBUG_UNIT_SIMCOM_WARNING "WARN"
#define DEBUG_UNIT_SIMCOM_INFO    "INFO"

#define DEBUG_LEVEL_SIMCOM_ERROR      //UT_LOG_LEVEL_ERROR
#define DEBUG_LEVEL_SIMCOM_WARNING    //UT_LOG_LEVEL_WARN
#define DEBUG_LEVEL_SIMCOM_INFO       //UT_LOG_LEVEL_INFO

#define SIMCOM_DEBUG_ERROR            0x00000001
#define SIMCOM_DEBUG_WARNING          0x00000002
#define SIMCOM_DEBUG_INFO             0x00000004
#define SIMCOM_DEBUG_STATE            0x00000008
#define SIMCOM_DEBUG_SEND             0x00000010
#define SIMCOM_DEBUG_RECV             0x00000020
#define SIMCOM_DEBUG_ENTER            0x00000040
#define SIMCOM_DEBUG_LEAVE            0x00000080
#define SIMCOM_DEBUG_TRACE            0x00000100
#define SIMCOM_DEBUG_STAT             0x00000200
#define SIMCOM_DEBUG_ASSERT           0x00000400

#define SIMCOM_DEBUG_NONE             0
#define SIMCOM_DEBUG_ALL              (~0)
#define SIMCOM_DEBUG_DEFMASK          SIMCOM_DEBUG_ALL

#define SIMCOM_DEBUG_MASK             SIMCOM_DEBUG_DEFMASK
#define SIMCOM_EOS_TRACE_INCLUDE      EOS_TRUE

#if 0
#define SIMCOM_LOG(lname, level, logout) \
            UT_LogSetEnv(EN_ASYS_MODULE_SPC, ASYS_MODULE_SPC, __FILE__, __LINE__, \
                lname, level, SIMCOM_EOS_TRACE_INCLUDE); logout
#endif
#if 0//(SIMCOM_DEBUG_MASK & SIMCOM_DEBUG_ERROR) 
#define SIMCOM_ERROR SIMCOM_LOG(DEBUG_UNIT_SIMCOM_ERROR, DEBUG_LEVEL_SIMCOM_ERROR, UT_LogErr)
#else
#define SIMCOM_ERROR
#endif

#if 0//(SIMCOM_DEBUG_MASK & SIMCOM_DEBUG_WARNING) 
#define SIMCOM_WARN SIMCOM_LOG(DEBUG_UNIT_SIMCOM_WARNING, DEBUG_LEVEL_SIMCOM_WARNING, UT_LogErr)
#else
#define SIMCOM_WARN
#endif

#if __OS_ANDROID__
#include <android/log.h>
#define LOG_TAG  "C_TAG"

#define SIMCOM_INFO(format,...)   __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,format,## __VA_ARGS__)
#define SIMCOM_DBG(format,...)    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,format,## __VA_ARGS__)

#else
#if 1//(SIMCOM_DEBUG_MASK & SIMCOM_DEBUG_INFO) 
#define SIMCOM_INFO(format,...) printf("|%15.15s:%04d| "format"", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define SIMCOM_INFO(format,...) 
#endif

#if 1
#define SIMCOM_DBG(format,...) printf("|%15.15s:%04d| "format"", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define SIMCOM_DBG(format,...) 
#endif
#endif


#if (SIMCOM_DEBUG_MASK & SIMCOM_DEBUG_ASSERT) 
#define SIMCOM_ASSERT(cond) \
do{ \
    if (!(cond)) \
        printf("%s %d: assert faiSIMCOM!\n", __FILE__, __LINE__); \
}while(0)
#else
#define SIMCOM_ASSERT(cond)
#endif

#define SIMCOM_CONDCHK(cond, errcode, prompt) \
do{ \
    if (!(cond)) \
    { \
        if (NULL != (prompt)) \
            SIMCOM_ERROR((_UINT32)(errcode), (_CHAR8*)(prompt)); \
        return errcode; \
    } \
}while(0)

#define SIMCOM_NPCHK(ptr, err) \
    SIMCOM_CONDCHK(ptr, err, "null pointer passed!\n")
    
#define SIMCOM_MPCHK(ptr, err) \
    SIMCOM_CONDCHK(ptr, err, "fail SIMCOM to allocate memory!\n")

#define SIMCOM_TRACE   printf("(%s)%d\n", __FILE__, __LINE__)
#define SIMCOM_Sprintf sprintf

#define SIMCOM_ERRORNO        -1
#endif /* SIM_COM_DEBUG_H */



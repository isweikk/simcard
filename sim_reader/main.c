/*
 read simdata,test task
 */

#include "eos_typedef.h"
#include "sim_com_debug.h"
#include "sim_com_interface.h"
#include "sim_com_app.h"
#include "sim_com_preread.h"

#if !__OS_ANDROID__

int main(int argc, char *argv[])
{
    SimComInit();
    SimCom_Task();
}

#else

#include "CToJava.h"

#include<string.h>
#include "zlib.h"

JNIEnv* jniEnv;
ST_SIMCOM_APPEVT appEvtSendBuff;
/**

 */
void  Java_com_nordicsemi_nrfUARTv2_JNIUtil_main(JNIEnv *env, jobject obj){

    if(jniEnv == NULL) {
        jniEnv = env;
    }



    SimComInit();
    SimCom_Task();


}

void  Java_com_nordicsemi_nrfUARTv2_JNIUtil_simComEvtApp2Drv(JNIEnv *env, jobject obj,jbyte chn,jbyte evtIndex,jint length,jbyteArray pdata){

    if(jniEnv == NULL) {
        jniEnv = env;
    }

unsigned char *pBuffer=(*env)->GetByteArrayElements(env,pdata,NULL);
    if(pBuffer==NULL){
        SIMCOM_INFO("GetByteArrayElements FAIL");
        return;
    }

    appEvtSendBuff.chn=chn;
    appEvtSendBuff.evtIndex=evtIndex;
    appEvtSendBuff.len=length;
    appEvtSendBuff.pData=pBuffer;
    SimComAppEvtHdl(&appEvtSendBuff);
    SIMCOM_INFO("chn length3 %d \n", chn);
}

#endif
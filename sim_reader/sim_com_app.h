
#ifndef    SIM_COM_APP_H
#define    SIM_COM_APP_H

#include "eos_typedef.h"
#include "sim_com_interface.h"

typedef        _VOID (*TIMER_TYPE)(_UINT32 ulParam1, _UINT32 ulParam2);
typedef _UINT32 EOS_TIMER_ID;

//#define SIMCOM_MALLOC(size)               ut_mem_alloc((size), __FILE__, __LINE__)
//#define SIMCOM_REMALLOC(addr, size)       ut_mem_realloc((addr), (size), __FILE__, __LINE__)
//#define SIMCOM_FREE(addr)                 ut_mem_free(addr, __FILE__, __LINE__)

typedef struct {
    unsigned int channelid;
	unsigned int size;
    unsigned char buff[1024];
}__attribute__((packed)) ioctl_simpool_read_st;

typedef struct {
    unsigned int channelid;
	unsigned int size;
    unsigned char buff[512];
}__attribute__((packed)) ioctl_simpool_write_st;

typedef struct {
    unsigned short simdetect[16];
}__attribute__((packed)) ioctl_simpool_sim_det_st;

typedef struct {
    unsigned int channelid;
	unsigned int simrst;
}__attribute__((packed)) ioctl_simpool_reset_st;

typedef struct {
    unsigned int channelid;
	unsigned int etu;
}__attribute__((packed)) ioctl_simpool_etu_st;

typedef struct {
    unsigned int channelid;
	unsigned int waitrsp;
}__attribute__((packed)) ioctl_simpool_setrsp_st;

typedef struct {
    unsigned int channelid;
    unsigned int RcvTotalCnt;
    unsigned int RcvOkCnt;
    unsigned int TransTotalCnt;
    unsigned int uiRxErrCnt;
}__attribute__((packed)) ioctl_simpool_pkt_cnt_st;

typedef enum
{
	SIMPOOL_ST_IDLE = 0,
	SIMPOOL_ST_RST,
	SIMPOOL_ST_WAIT_INS,
	SIMPOOL_ST_WAIT_RSP,
	SIMPOOL_ST_NEXTCMD_WAIT_RSP,
	SIMPOOL_ST_PRERD,
	SIMPOOL_ST_PREFIX_WAIT_INS,
	SIMPOOL_ST_PREFIX_WAIT_RSP,
	
	SIMPOOL_ST_MAX
}SIMPOOL_STATUS;

typedef enum
{
	SPPRERD_SUBST_A4INSWAIT = 0,
	SPPRERD_SUBST_A4RSPWAIT,
	SPPRERD_SUBST_C0RSPWAIT,
	SPPRERD_SUBST_B0RSPWAIT,
	SPPRERD_SUBST_B2RSPWAIT,
	
	SPPRERD_SUBST_MAX
}SIMPOOL_PRERD_SUBSTATUS;

#define SIMPOOL_RSPREAD_MAX_SIZE    512

#define SIMPOOL_GETPPS_TIME			2000//500

#define SP_MAX_PRERD_ROOT_SISE		(32*1024)
#define SP_MAX_PRERD_FILE_SISE		(22*1024)

#if 0    
#define SimComSemP(sem, timeout) \
do{ \
    eos_sem_p(sem, timeout); \
}while(0)

#define SimComSemV(sem) \
do{ \
    eos_sem_v(sem); \
}while(0)
#else
#define SimComSemP(sem, timeout)
#define SimComSemV(sem)
#endif

typedef enum
{
	EN_TIMER_CALLID_DETSIMIN = 101,
	EN_TIMER_CALLID_RST,
	EN_TIMER_CALLID_RSTRSP,
	EN_TIMER_CALLID_PPSRSP
}EN_TIMER_CALLID;

typedef struct simcomchn
{    
    _UINT32 uiChannelid;
	_BOOL	bGetRstRspStatus;
	_BOOL	bGetRstRspDone;
	_UINT32 RstRspBuffLen;
	_UCHAR8 RstRspBuff[128];
	
	EOS_TIMER_ID    GetRstRspTimerId;
	EOS_TIMER_ID    GetPPSRspTimerId;
	EOS_TIMER_ID    SetRstTimerId;
	EOS_TIMER_ID    SetSimRstTimerId;

	_UCHAR8 ICCIDBuf[64];
	_UCHAR8 IMSIBuf[64];
	_UCHAR8 IMEIBuf[64];

    _UCHAR8  RspBuff[SIMPOOL_RSPREAD_MAX_SIZE];
	_UINT32  uiReadOffset;
	_UINT32  uiWriteOffset;

	_UINT32	 LastReadTick;
	_UINT32	 WaitRspTick;
	SIMPOOL_STATUS SimPoolStatus;
	_USHORT16 CmdIdx;
	_USHORT16 expRspLen;	

	SIMPOOL_PRERD_SUBSTATUS SPPreRDStatus;
	_UINT32 uiPreRdIdx;
	_UINT32 uiPrefixSelectIdx;
	
	_BOOL	bNeedPreRead;
	_UCHAR8 PreRdCmdSt[512];
	_UCHAR8* PreRdFidSt;
	_UCHAR8* PreRdRootSt;
	_UINT32 uiPreRdExpLen;
	_UINT32 uiB2CurrentIdx;
	_UINT32 uiB2TotalNum;
	_UINT32 uiB2Type;
	_UINT32 uiB2RecordSize;

	_UINT32 uiB0CurrentIdx;
	_UINT32 uiB0TotalNum;
	_UINT32 uiB0LastRdSize;
	
	
	_UCHAR8 ucCmdData[1024];
    _BOOL   bHasBakCmd;

	_BOOL   bNormalRead;
	_BOOL   bSupportUSIM;

	_BOOL	bSimExisted;
	_BOOL	bSimOk;
	_BOOL	bHighBaudrate;
	_UINT32 uiSameCmd;
	_UINT32 uiDiffCmd;
	_UINT32 uiLessCmd;
	_UINT32 uiTimeoutRsp;
	_UINT32	uiPreReadCnt;		
	_UINT32	uiTotalSendCnt;	
	_UINT32	uiTotalRcvCnt;	
	_UCHAR8 ucRstRsp[32];

	_UCHAR8  AIDBuff[32];
	_UCHAR8  AIDLen;
	
	_UCHAR8 RspForNextCmd[128];
	_UINT32 uiRspForNextCmdLen;
	
}SIMCOMCHNST,*PSIMCOMCHNST;
/*
typedef struct simcomlistnode{
	_NODE		stnode;
	_UINT32    	channelid;
    _UINT32    	op_type;
	_UINT32		Value;
	_CHAR8*		pcStr;
	_CHAR8		acValue[800];
}SIMCOMLISTNODE,*PSIMCOMLISTNODE;
*/

enum
{
	SIMPOOL_MSG_GET_RSTDATA = 0,
	SIMPOOL_MSG_RSTDATA,
	SIMPOOL_MSG_SET_RST,
	SIMPOOL_MSG_SIM_RST,	
	SIMPOOL_MSG_NULL,
	SIMPOOL_MSG_MAX
};

typedef struct stSimPoolMsg
{
	_UCHAR8 chn;	
	_UCHAR8 type;
	_UCHAR8 value;
    _UCHAR8 len;
	_UCHAR8 data[128];
}ST_SIMPOOL_MSG;

typedef struct wrlescccmd
{    
    _USHORT16 cmdIndex;
	_UCHAR8 cmdLen;
	_UCHAR8 paramLen;
	_USHORT16 expRspLen;
	_USHORT16 PrefixNum;
	_UCHAR8 PrefixFid[4][2];	
	_UCHAR8 cmdData[0];

}__attribute__((__packed__)) WRLESCCCMDST, *PWRLESCCCMDST;


//typedef  struct wrlescccmd WRLESCCCMDST;
//typedef  struct wrlescccmd *PWRLESCCCMDST;

#define SP_ESCCRSP_HDRLEN    10

typedef struct wrlesccrsp
{    
    _USHORT16 rspIndex;
	_USHORT16 rsplen;
	_UCHAR8 nextlen;
	_UCHAR8 nextcmd[5];
	_UCHAR8 rspdata[0];

}__attribute__((__packed__)) WRLESCCRSPST, *PWRLESCCRSPST;

//typedef  struct wrlesccrsp WRLESCCRSPST;
//typedef  struct wrlesccrsp *PWRLESCCRSPST;


enum
{
	SP_MSG_PRERD1 = 0,
	SP_MSG_PRERD2,
	SP_MSG_NULL,
	
	SP_MSG_MAX
};

struct stSPMsg
{
	_UCHAR8 chn;	
	_UCHAR8 type;
	_UCHAR8 cnt;
	_UCHAR8 paramlen;
    _UINT32 len;
	_UCHAR8 rspdata[0];
}__attribute__((__packed__));

typedef  struct stSPMsg WRLESCCPRERDMSGST;
typedef  struct stSPMsg *PWRLESCCPRERDMSGST;


typedef struct simcomrootst
{
    _VOID            (*SimComInitFunPtr)(_VOID);
	_VOID			  (*SimComTask)();
    /*    max channel num    */
    _UINT32        uiMaxChannelNum;
    
    _INT32         iHardwareVer;
    _INT32         iSIMVer;

	_INT32 		   fdSerialHdl;
	PSIMCOMCHNST   pSimComChnSt;

	EOS_TIMER_ID 	SimComTimerID;

	/* for sim detection	*/
	EOS_TIMER_ID	SIMDetectTimerId;
    _UINT32         cardStatus;
	_UINT32 	    CardDetect1;
	_UINT32 	    CardDetect2;
	_UINT32 	    CardDetect3;
	
}SIMCOMROOTST,*PSIMCOMROOTST;

_INT32 SimCom_Init(_VOID);
_VOID SimComAppEvtHdl(ST_SIMCOM_APPEVT * evtMsg);
_VOID SimComAppEvtSend(ST_SIMCOM_APPEVT * evtMsg);

_VOID SimCom_Task(_VOID);
_VOID SimComWriteData(_UINT32 uiChannelId, _UCHAR8* pData, _UINT32 uiLen);

#endif


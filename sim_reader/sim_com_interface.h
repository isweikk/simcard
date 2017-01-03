
#ifndef SIM_COM_INTERFACE_H
#define SIM_COM_INTERFACE_H

#include "eos_typedef.h"

enum
{
    EN_APPEVT_NONE = 0,
    EN_APPEVT_CMD_SETRST,
    EN_APPEVT_CMD_SIMCLR,
    EN_APPEVT_RSTRSP,
    EN_APPEVT_PRDATA,
    EN_APPEVT_SIMDATA,
    EN_APPEVT_SIMINFO
};//EN_APP_SUBEVT;

typedef struct stSimComAppEvt
{
	_UCHAR8  chn;	
	_UCHAR8  evtIndex;    //EN_APP_EVT
    _UINT32  len;
	_UCHAR8 *pData;
}ST_SIMCOM_APPEVT;


_INT32 SimComInit(_VOID);
_INT32 SimComEvtApp2Drv(ST_SIMCOM_APPEVT * evtMsg);
_INT32 SimComEvtDrv2App(ST_SIMCOM_APPEVT * evtMsg);


#endif

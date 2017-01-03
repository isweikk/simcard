
#ifndef    SIM_COM_PREREAD_H
#define    SIM_COM_PREREAD_H

#include "eos_typedef.h"

enum
{
	PREREAD_A4 = 0,
	PREREAD_C0,
	PREREAD_B0,
	PREREAD_B2,
	
	PREREAD_AID,

	PREREAD_END,
	PREREAD_MAX
};

typedef struct stPreRdFid
{
	_UCHAR8 type;	
	_UCHAR8 fileid[2];
}ST_PREREAD_FID;

#define PRERDCMDELEMENT_HDLEN 	12

typedef struct stPreRdCmdElement
{
	_UCHAR8  CmdLen;	
	_UCHAR8  CmdData[9];
	_USHORT16 RspLen;
	_UCHAR8  RspData[512];
}__attribute__((__packed__)) ST_PREREAD_CMDELEMENT;

//typedef struct stPreRdCmdElement ST_PREREAD_CMDELEMENT;

#define PRERDFILEID_HDLEN 	38

typedef struct stPreRdFileID
{
	_UCHAR8  FileID[32];
	_USHORT16 Number;
	_UINT32  Len;
	ST_PREREAD_CMDELEMENT CmdElement[0];
}__attribute__((__packed__)) ST_PREREAD_FILEID;

//typedef struct stPreRdFileID ST_PREREAD_FILEID;

#define PRERDROOT_HDLEN 	16

typedef struct stPreRdRoot
{
	_UINT32  Slot;
	_UINT32  uiFileNum;	
	_UINT32  Len;
	_UINT32	 bUSIM; //1:USIM, 0:SIM
	ST_PREREAD_FILEID FileId[0];
}__attribute__((__packed__)) ST_PREREAD_ROOT;

//typedef struct stPreRdRoot ST_PREREAD_ROOT;

_INT32 SPPreRd_Read_SIM(_UINT32 ChannelId, _UINT32 uiBuffLeftLen);


#endif


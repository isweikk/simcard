
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "sim_com_debug.h"
#include "sim_com_app.h"
#include "sim_com_preread.h"
#include "sim_com_interface.h"

extern SIMCOMROOTST stSimComRootSt;

ST_PREREAD_FID PreReadFID_UC15_SIM[] = {/*	UC15	*/
									{PREREAD_A4, {0x3f, 0x00}}, 
									{PREREAD_B0, {0x2f, 0xe2}},
									{PREREAD_B0, {0x2f, 0x05}},
									{PREREAD_C0, {0x7f, 0x20}},
									{PREREAD_B0, {0x6f, 0x74}},
									{PREREAD_B0, {0x6f, 0xb7}},	
									{PREREAD_B0, {0x6f, 0x16}}, 
									{PREREAD_B0, {0x6f, 0xae}},
									{PREREAD_B0, {0x6f, 0x05}},
									{PREREAD_B0, {0x6f, 0x38}}, 
									{PREREAD_B0, {0x6f, 0x07}}, 
									{PREREAD_B0, {0x6f, 0x3e}}, 
									{PREREAD_B0, {0x6f, 0x3f}},
									//{PREREAD_B0, {0x6f, 0x30}},
									{PREREAD_B0, {0x6f, 0x62}},
									{PREREAD_B0, {0x6f, 0x60}},
									{PREREAD_B0, {0x6f, 0x61}},
									//kinson test {PREREAD_B0, {0x6f, 0x20}},								   	
									{PREREAD_B0, {0x6f, 0x7e}},									
									{PREREAD_B0, {0x6f, 0x31}},									
									{PREREAD_B0, {0x6f, 0x74}},
									{PREREAD_B0, {0x6f, 0x7b}},									
									{PREREAD_B0, {0x6f, 0x41}},
									{PREREAD_B0, {0x6f, 0xad}},	
									{PREREAD_B0, {0x6f, 0x78}},	
									{PREREAD_B0, {0x6f, 0x13}},	
									{PREREAD_B0, {0x6f, 0x14}},
									{PREREAD_B0, {0x6f, 0xcb}},
									{PREREAD_B0, {0x6f, 0x19}},	
									{PREREAD_B0, {0x6f, 0x45}},
									{PREREAD_B0, {0x6f, 0x46}},
									{PREREAD_B0, {0x6f, 0x48}},
									{PREREAD_B0, {0x6f, 0x11}},
									{PREREAD_B0, {0x6f, 0x50}}, 	
									{PREREAD_B0, {0x6f, 0x52}},		
									{PREREAD_B0, {0x6f, 0x53}},		

									{PREREAD_A4, {0x7f, 0x10}}, 
									{PREREAD_B2, {0x6f, 0x40}},
									{PREREAD_B2, {0x6f, 0x3a}},
									{PREREAD_B2, {0x6f, 0x3b}},
									{PREREAD_B2, {0x6f, 0x3c}},
									{PREREAD_B2, {0x6f, 0x3d}},
									{PREREAD_B2, {0x6f, 0x4a}},
									{PREREAD_B2, {0x6f, 0x4b}},
									{PREREAD_B2, {0x6f, 0x4c}},
									{PREREAD_B2, {0x6f, 0x42}},
									{PREREAD_B0, {0x6f, 0x43}},
									{PREREAD_B2, {0x6f, 0x44}},
									{PREREAD_B2, {0x6f, 0x49}},
									{PREREAD_END, {0xff, 0xff}}
								    };


static _INT32 spPreRdFillFileSt(_UINT32 ChannelId, ST_PREREAD_CMDELEMENT* pPreRdCmdElmt, _UINT32 SubElmtSize);
static _INT32 spPreRdFillRootSt(_UINT32 ChannelId, ST_PREREAD_FILEID* pPreRdFileId, _UINT32 SubFileSize);
static _INT32 spPreRdSend(_UINT32 ChannelId);
static _CHAR8* spPreRdFindNodeFromC0(_CHAR8* pSrcBuff, _CHAR8 NodeTag, _UINT32 SrcLen);

static _INT32 spPreRdFillFileSt(_UINT32 ChannelId, ST_PREREAD_CMDELEMENT* pPreRdCmdElmt, _UINT32 SubElmtSize)
{
    PSIMCOMROOTST         pSimComRootSt;
    PSIMCOMCHNST          pChnStHandle = NULL;

	_UINT32 CmdElmtSize = SubElmtSize + PRERDCMDELEMENT_HDLEN;
	ST_PREREAD_ROOT* pPreRdRoot = NULL;
	ST_PREREAD_FILEID* pPreRdFileId = NULL;

    pSimComRootSt = (PSIMCOMROOTST)&stSimComRootSt; 
    pChnStHandle = &pSimComRootSt->pSimComChnSt[ChannelId];

	pPreRdRoot = (ST_PREREAD_ROOT*)(pChnStHandle->PreRdRootSt);
	pPreRdFileId = (ST_PREREAD_FILEID*)(pChnStHandle->PreRdFidSt);

	pPreRdFileId->Number++;
	
	/* Fill File Struct */
	if((pPreRdFileId->Len + CmdElmtSize) < (SP_MAX_PRERD_FILE_SISE - PRERDFILEID_HDLEN))
	{
		memcpy(((_UCHAR8*)(pPreRdFileId->CmdElement)) + pPreRdFileId->Len, (_UCHAR8*)(pPreRdCmdElmt), CmdElmtSize);
		pPreRdFileId->Len += CmdElmtSize;
	}
	else
	{
		SIMCOM_INFO("Channel %d memcpy overflow in filling filest, ocuppied %d, need:%d\n", ChannelId, pPreRdFileId->Len, CmdElmtSize);
	}
	
	return EOS_OK;
}

static _CHAR8* spPreRdFindNodeFromC0(_CHAR8* pSrcBuff, _CHAR8 NodeTag, _UINT32 SrcLen)
{
	_CHAR8* pFindTag = pSrcBuff + 3;

	while((*pFindTag) != NodeTag)
	{
		pFindTag = pFindTag + 2 + (*(pFindTag + 1));

		if((*pFindTag == 0x90) || ((pSrcBuff + SrcLen) < pFindTag))
		{
			pFindTag = NULL;
			break;
		}
	}

	return pFindTag;
}

static _INT32 spPreRdFillRootSt(_UINT32 ChannelId, ST_PREREAD_FILEID* pPreRdFileId, _UINT32 SubFileSize)
{
    PSIMCOMROOTST         pSimComRootSt;
    PSIMCOMCHNST          pChnStHandle = NULL;
	_UINT32 FileStSize	= SubFileSize + PRERDFILEID_HDLEN;
	ST_PREREAD_ROOT* pPreRdRoot = NULL;

    pSimComRootSt = (PSIMCOMROOTST)&stSimComRootSt;
    pChnStHandle = &pSimComRootSt->pSimComChnSt[ChannelId];

	pPreRdRoot = (ST_PREREAD_ROOT*)(pChnStHandle->PreRdRootSt);
		
	pPreRdFileId->Len = htonl(pPreRdFileId->Len);
	pPreRdFileId->Number = htons(pPreRdFileId->Number);

	if((pPreRdRoot->Len + FileStSize) < (SP_MAX_PRERD_ROOT_SISE - PRERDROOT_HDLEN))
	{
		memcpy(((_UCHAR8*)(pPreRdRoot->FileId)) + pPreRdRoot->Len, (_UCHAR8*)(pPreRdFileId), FileStSize);
		pPreRdRoot->Len += FileStSize;	
		pPreRdRoot->uiFileNum++;
		
		pChnStHandle->uiPreRdIdx++;
	}
	else
	{
		SIMCOM_INFO("Channel %d memcpy overflow in root filest, ocuppied %d, need:%d\n", ChannelId, pPreRdRoot->Len, FileStSize);		
	}
	return EOS_OK;
}

static _INT32 spPreRdSend(_UINT32 ChannelId)
{
    PSIMCOMROOTST         pSimComRootSt;
    PSIMCOMCHNST          pChnStHandle = NULL;
	ST_PREREAD_ROOT* pPreRdRoot = NULL;
	_UINT32 uiSendLen = 0;
	
	pSimComRootSt = (PSIMCOMROOTST)&stSimComRootSt;  
    pChnStHandle = &pSimComRootSt->pSimComChnSt[ChannelId];
	pPreRdRoot = (ST_PREREAD_ROOT*)(pChnStHandle->PreRdRootSt);
	
	uiSendLen = pPreRdRoot->Len + PRERDROOT_HDLEN;
	
	SIMCOM_INFO("Channel %d Total PreRead Len:%d, FileNum:%d\n", ChannelId, (pPreRdRoot->Len + PRERDROOT_HDLEN), pPreRdRoot->uiFileNum);
    SIMCOM_INFO("Channel %d SimInfo ICCID-%s IMSI-%s\n", ChannelId, pChnStHandle->ICCIDBuf, pChnStHandle->IMSIBuf);
	pPreRdRoot->uiFileNum = htonl(pPreRdRoot->uiFileNum);
	pPreRdRoot->Slot = htonl(pPreRdRoot->Slot);
	pPreRdRoot->Len = htonl(pPreRdRoot->Len);
	pPreRdRoot->bUSIM = htonl(pChnStHandle->bSupportUSIM);

	pChnStHandle->uiPreReadCnt = uiSendLen;
	
	//ESCC_SessNtfySimIn(pChnStHandle->pEsccSessId, pChnStHandle->ICCIDBuf, pChnStHandle->IMSIBuf, (_UCHAR8 *)pPreRdRoot, uiSendLen);
    //send to app
    //weikk 20161221
    ST_SIMCOM_APPEVT appEvtSendBuff;
    appEvtSendBuff.chn = 0;
    appEvtSendBuff.evtIndex = EN_APPEVT_PRDATA;
    appEvtSendBuff.len = uiSendLen;
    appEvtSendBuff.pData = (_UCHAR8 *)pPreRdRoot;
    SimComAppEvtSend(&appEvtSendBuff);

    _UCHAR8 simInfo[128];
    sprintf(simInfo, "ICCID:%s;IMSI:%s", pChnStHandle->ICCIDBuf, pChnStHandle->IMSIBuf);
    SIMCOM_INFO("appEvtSend siminfo:%s\n", simInfo);
    appEvtSendBuff.evtIndex = EN_APPEVT_SIMINFO;
    appEvtSendBuff.len = strlen(simInfo);
    appEvtSendBuff.pData = (_UCHAR8 *)simInfo;
    SimComAppEvtSend(&appEvtSendBuff);
	
	pChnStHandle->SimPoolStatus = SIMPOOL_ST_IDLE;
    pChnStHandle->SPPreRDStatus = SPPRERD_SUBST_A4INSWAIT;
	
	pChnStHandle->bHasBakCmd = EOS_FALSE;

	return EOS_OK;
}

_INT32 SPPreRd_Read_SIM(_UINT32 ChannelId, _UINT32 uiBuffLeftLen)
{
    PSIMCOMROOTST         pSimComRootSt;
    PSIMCOMCHNST          pChnStHandle = NULL;

	ST_PREREAD_ROOT* pPreRdRoot = NULL;
	ST_PREREAD_FILEID* pPreRdFileId = NULL;
	ST_PREREAD_CMDELEMENT* pPreRdCmdElmt = NULL;
	_UINT32 i = 0;
	ST_PREREAD_FID* PreReadFID = NULL;

    pSimComRootSt = (PSIMCOMROOTST)&stSimComRootSt;  
    pChnStHandle = &pSimComRootSt->pSimComChnSt[ChannelId];

	pPreRdRoot = (ST_PREREAD_ROOT*)(pChnStHandle->PreRdRootSt);
	pPreRdFileId = (ST_PREREAD_FILEID*)(pChnStHandle->PreRdFidSt);
	pPreRdCmdElmt = (ST_PREREAD_CMDELEMENT*)(pChnStHandle->PreRdCmdSt);
	PreReadFID = PreReadFID_UC15_SIM;			
    
    SIMCOM_INFO("SPPreRDStatus=%d, uiPreRdExpLen = %d, uiBuffLeftLen = %d\n" , pChnStHandle->SPPreRDStatus,  pChnStHandle->uiPreRdExpLen, uiBuffLeftLen);
    SIMCOM_DBG("buffer - %x.%x\n", pChnStHandle->RspBuff[pChnStHandle->uiWriteOffset],pChnStHandle->RspBuff[pChnStHandle->uiWriteOffset+1]);
    SIMCOM_DBG("test00 --type=%d.\n", PreReadFID[pChnStHandle->uiPreRdIdx].type);
	if(SPPRERD_SUBST_A4INSWAIT == pChnStHandle->SPPreRDStatus)
	{
		/* Receive A4	*/
		if(((1 == uiBuffLeftLen) && (pChnStHandle->RspBuff[pChnStHandle->uiWriteOffset] == 0xA4))
			|| ((uiBuffLeftLen > 1) && (pChnStHandle->RspBuff[pChnStHandle->uiWriteOffset + uiBuffLeftLen - 1] == 0xA4)))
		{
			/* Send FileId*/
            #if 0
			    SimComWriteData(ChannelId, PreReadFID[pChnStHandle->uiPreRdIdx].fileid, sizeof(PreReadFID[pChnStHandle->uiPreRdIdx].fileid));
            #else
                _UCHAR8 WriteDataEx[16] = {0xa0, 0xa4, 0x0, 0x0, 0x2,}; 
                _UINT32 uiSizeTemp = 0;
                
                uiSizeTemp = 5;
    			memcpy(&WriteDataEx[uiSizeTemp], PreReadFID[pChnStHandle->uiPreRdIdx].fileid, sizeof(PreReadFID[pChnStHandle->uiPreRdIdx].fileid));
    			uiSizeTemp += sizeof(PreReadFID[pChnStHandle->uiPreRdIdx].fileid);
    			/* Send FileId*/
    			SimComWriteData(ChannelId, WriteDataEx, uiSizeTemp);
            #endif

			pChnStHandle->uiWriteOffset += uiBuffLeftLen;
			pChnStHandle->uiWriteOffset &= (SIMPOOL_RSPREAD_MAX_SIZE - 1);
			
			pChnStHandle->SPPreRDStatus = SPPRERD_SUBST_A4RSPWAIT;
		}
		else if(2 == uiBuffLeftLen)
		{
			if(pChnStHandle->RspBuff[pChnStHandle->uiWriteOffset] == 0x6E)
			{
				pChnStHandle->uiWriteOffset++;
				pChnStHandle->uiWriteOffset &= (SIMPOOL_RSPREAD_MAX_SIZE - 1);
				
				if(pChnStHandle->RspBuff[pChnStHandle->uiWriteOffset] == 0x00)
				{
					if(0)//(NVM_GOIP_SIMTYPE_WCDMA == uiSimType)
					{
						_UCHAR8 WriteData[5] = {0xa0, 0xa4, 0x0, 0x0, 0x2}; 															

						pChnStHandle->bSupportUSIM = EOS_FALSE;

						SIMCOM_INFO("Channel %d WCDMA not support USIM\n", ChannelId);				
								
						/* Send FileId*/
						SimComWriteData(ChannelId, WriteData, sizeof(WriteData));
						pChnStHandle->SimPoolStatus = SIMPOOL_ST_PRERD;
					
						/* Send A0 A4 00 00 02 */
						pChnStHandle->SPPreRDStatus = SPPRERD_SUBST_A4INSWAIT;

					}
				}
			}
			else if(pChnStHandle->RspBuff[pChnStHandle->uiWriteOffset] == 0x6A)
			{
				pChnStHandle->uiWriteOffset++;
				pChnStHandle->uiWriteOffset &= (SIMPOOL_RSPREAD_MAX_SIZE - 1);
				
				if(pChnStHandle->RspBuff[pChnStHandle->uiWriteOffset] == 0x86)
				{
					if(0)//(NVM_GOIP_SIMTYPE_WCDMA == uiSimType)
					{
						_UCHAR8 WriteData[5] = {0xa0, 0xa4, 0x0, 0x0, 0x2}; 															

						pChnStHandle->bSupportUSIM = EOS_FALSE;

						SIMCOM_INFO("Channel %d WCDMA not support USIM\n", ChannelId);				
								
						/* Send FileId*/
						SimComWriteData(ChannelId, WriteData, sizeof(WriteData));
						pChnStHandle->SimPoolStatus = SIMPOOL_ST_PRERD;
					
						/* Send A0 A4 00 00 02 */
						pChnStHandle->SPPreRDStatus = SPPRERD_SUBST_A4INSWAIT;

					}
				}
			}
		}
	}
	else if(SPPRERD_SUBST_A4RSPWAIT == pChnStHandle->SPPreRDStatus)
	{						
		_UCHAR8 A4CmdData[5] = {0xa0, 0xa4, 0x0, 0x0, 0x02};

		if(2 == uiBuffLeftLen)
		{
			pPreRdCmdElmt->CmdLen = 5;
			memcpy(pPreRdCmdElmt->CmdData, A4CmdData, sizeof(A4CmdData));
			pPreRdCmdElmt->RspLen = htons(2);

			for(i = 0; i < uiBuffLeftLen; i++)
			{
				pPreRdCmdElmt->RspData[i] = pChnStHandle->RspBuff[pChnStHandle->uiWriteOffset++];
				pChnStHandle->uiWriteOffset &= (SIMPOOL_RSPREAD_MAX_SIZE - 1);
			}		

			memcpy(pPreRdFileId->FileID, PreReadFID[pChnStHandle->uiPreRdIdx].fileid, 2);
			pPreRdFileId->Len= 0;
			pPreRdFileId->Number = 0;
			spPreRdFillFileSt(ChannelId, pPreRdCmdElmt, uiBuffLeftLen);			

			/*	if file type isn't A0	*/
			if(PreReadFID[pChnStHandle->uiPreRdIdx].type != PREREAD_A4)
			{
				if(pPreRdCmdElmt->RspData[0] == 0x94)
				{
					spPreRdFillRootSt(ChannelId, pPreRdFileId, pPreRdFileId->Len);
															
					if(PREREAD_END == PreReadFID[pChnStHandle->uiPreRdIdx].type)
					{
						spPreRdSend(ChannelId);		
					}	
					else
					{
                        #if 0
						SimComWriteData(ChannelId, A4CmdData, sizeof(A4CmdData));
													
						pChnStHandle->SPPreRDStatus = SPPRERD_SUBST_A4INSWAIT;	
                        #else
                        _UCHAR8 WriteDataEx[16] = {0xa0, 0xa4, 0x0, 0x0, 0x2,}; 
                        _UINT32 uiSizeTemp = 0;

                        uiSizeTemp = 5;
            			memcpy(&WriteDataEx[uiSizeTemp], PreReadFID[pChnStHandle->uiPreRdIdx].fileid, sizeof(PreReadFID[pChnStHandle->uiPreRdIdx].fileid));
            			uiSizeTemp += sizeof(PreReadFID[pChnStHandle->uiPreRdIdx].fileid);
            			/* Send FileId*/
            			SimComWriteData(ChannelId, WriteDataEx, uiSizeTemp);
        											
        				pChnStHandle->SPPreRDStatus = SPPRERD_SUBST_A4RSPWAIT;	
                        #endif
					}
				}
				else
				{
					_UCHAR8 C0CmdData[5] = {0xA0, 0xC0, 0x00, 0x00, 0x00};

					C0CmdData[4] = ((ST_PREREAD_CMDELEMENT*)(pChnStHandle->PreRdCmdSt))->RspData[1];

					if(0)//(NVM_GOIP_SIMTYPE_GSM == uiSimType)//only for m35
					{
						if(C0CmdData[4]> 0x16)
						{
							C0CmdData[4] = 0x16;
						}
					}
					
					pChnStHandle->uiPreRdExpLen = C0CmdData[4] + 3;
					/* Send C0	*/
					SimComWriteData(ChannelId, C0CmdData, sizeof(C0CmdData));
					
					pChnStHandle->SPPreRDStatus = SPPRERD_SUBST_C0RSPWAIT;
					pPreRdCmdElmt->CmdLen = 5;
					memcpy(pPreRdCmdElmt->CmdData, C0CmdData, sizeof(C0CmdData));
				}
			}
			else
			{
                #if 0
				spPreRdFillRootSt(ChannelId, pPreRdFileId, pPreRdFileId->Len);
		
				SimComWriteData(ChannelId, A4CmdData, sizeof(A4CmdData));
				pChnStHandle->SPPreRDStatus = SPPRERD_SUBST_A4INSWAIT;	
                #else
                _UCHAR8 WriteDataEx[16] = {0xa0, 0xa4, 0x0, 0x0, 0x2,}; 
                _UINT32 uiSizeTemp = 0;

                spPreRdFillRootSt(ChannelId, pPreRdFileId, pPreRdFileId->Len);
                uiSizeTemp = 5;
    			memcpy(&WriteDataEx[uiSizeTemp], PreReadFID[pChnStHandle->uiPreRdIdx].fileid, sizeof(PreReadFID[pChnStHandle->uiPreRdIdx].fileid));
    			uiSizeTemp += sizeof(PreReadFID[pChnStHandle->uiPreRdIdx].fileid);
    			/* Send FileId*/
    			SimComWriteData(ChannelId, WriteDataEx, uiSizeTemp);
											
				pChnStHandle->SPPreRDStatus = SPPRERD_SUBST_A4RSPWAIT;	
                #endif
			}				
		}
	}
	else if(SPPRERD_SUBST_C0RSPWAIT == pChnStHandle->SPPreRDStatus)
	{
		if(pChnStHandle->uiPreRdExpLen == uiBuffLeftLen)
		{
			/* Fill Command Element	*/
			pPreRdCmdElmt->RspLen = htons(pChnStHandle->uiPreRdExpLen);
            #if 1
            _CHAR8 printBuff[1024];
            _INT32 uiPrintLen;
            _INT32 i=0;

            uiPrintLen = sprintf(printBuff, "C0 Data Cpy(%d):", uiBuffLeftLen);
            for(i = 0; i < uiBuffLeftLen; i++)
			{
				pPreRdCmdElmt->RspData[i] = pChnStHandle->RspBuff[pChnStHandle->uiWriteOffset++];						
				pChnStHandle->uiWriteOffset &= (SIMPOOL_RSPREAD_MAX_SIZE - 1);
                uiPrintLen += sprintf(printBuff, "0x%02x ", pPreRdCmdElmt->RspData[i]);
			}
            sprintf(printBuff+uiPrintLen,"\n");
            SIMCOM_INFO("%s",printBuff);
            #endif
			
			/*	if file type isn't C0	*/
			if(PreReadFID[pChnStHandle->uiPreRdIdx].type != PREREAD_C0)
			{
				/* Send B0 or B2	*/
				if(PreReadFID[pChnStHandle->uiPreRdIdx].type == PREREAD_B0)
				{
					_UCHAR8 B0CmdData[5] = {0xA0, 0xB0, 0x00, 0x00, 0x00};

					if(0)//(NVM_GOIP_SIMTYPE_GSM == uiSimType)//only for m35
					{
						if((pPreRdCmdElmt->RspData[4] > 0x78) || (pPreRdCmdElmt->RspData[3] > 0x0))
						{
							B0CmdData[4] = 0x78;
						}
						else
						{
							B0CmdData[4] = pPreRdCmdElmt->RspData[4];
						}
					}
					else
					{
						if(pPreRdCmdElmt->RspData[3] > 0x0)
						{
							B0CmdData[4] = 0;
						}
						else
						{
							B0CmdData[4] = pPreRdCmdElmt->RspData[4];
						}
					}
					
					/* Send B0	*/
					SimComWriteData(ChannelId, B0CmdData, sizeof(B0CmdData));
					if(B0CmdData[4] == 0x0)
					{
						pChnStHandle->uiPreRdExpLen = 3 + 256;				
					}
					else
					{
						pChnStHandle->uiPreRdExpLen = 3 + B0CmdData[4];
					}
					
					spPreRdFillFileSt(ChannelId, pPreRdCmdElmt, uiBuffLeftLen);
					
					pChnStHandle->SPPreRDStatus = SPPRERD_SUBST_B0RSPWAIT;
					pPreRdCmdElmt->CmdLen = 5;
					memcpy(pPreRdCmdElmt->CmdData, B0CmdData, sizeof(B0CmdData));
				
				}
				else if(PreReadFID[pChnStHandle->uiPreRdIdx].type == PREREAD_B2)
				{
					_UINT32 uiTotalSize = 0;
					_UCHAR8 B2CmdData[5] = {0xA0, 0xB2, 0x00, 0x00, 0x00};

					pChnStHandle->uiB2RecordSize = pPreRdCmdElmt->RspData[15];

					if(((0x6f == PreReadFID[pChnStHandle->uiPreRdIdx].fileid[0]) && (0x3b == PreReadFID[pChnStHandle->uiPreRdIdx].fileid[1]))
						|| ((0x6f == PreReadFID[pChnStHandle->uiPreRdIdx].fileid[0]) && (0x3c == PreReadFID[pChnStHandle->uiPreRdIdx].fileid[1]))
						|| ((0x6f == PreReadFID[pChnStHandle->uiPreRdIdx].fileid[0]) && (0x3d == PreReadFID[pChnStHandle->uiPreRdIdx].fileid[1]))
						|| ((0x6f == PreReadFID[pChnStHandle->uiPreRdIdx].fileid[0]) && (0x4a == PreReadFID[pChnStHandle->uiPreRdIdx].fileid[1]))
						|| ((0x6f == PreReadFID[pChnStHandle->uiPreRdIdx].fileid[0]) && (0x4b == PreReadFID[pChnStHandle->uiPreRdIdx].fileid[1]))
						|| ((0x6f == PreReadFID[pChnStHandle->uiPreRdIdx].fileid[0]) && (0x42 == PreReadFID[pChnStHandle->uiPreRdIdx].fileid[1]))
						|| ((0x6f == PreReadFID[pChnStHandle->uiPreRdIdx].fileid[0]) && (0x43 == PreReadFID[pChnStHandle->uiPreRdIdx].fileid[1]))
						|| ((0x6f == PreReadFID[pChnStHandle->uiPreRdIdx].fileid[0]) && (0x44 == PreReadFID[pChnStHandle->uiPreRdIdx].fileid[1])))
					{
						uiTotalSize = (pChnStHandle->uiB2RecordSize) * 1;
						pPreRdCmdElmt->RspData[3] = (_UCHAR8)(uiTotalSize >> 8);
						pPreRdCmdElmt->RspData[4] = (_UCHAR8)(uiTotalSize & 0xff);
					}
					else if((0x6f == PreReadFID[pChnStHandle->uiPreRdIdx].fileid[0]) && (0x3a == PreReadFID[pChnStHandle->uiPreRdIdx].fileid[1]))
					{
						uiTotalSize = (pChnStHandle->uiB2RecordSize) * 6;
						pPreRdCmdElmt->RspData[3] = (_UCHAR8)(uiTotalSize >> 8);
						pPreRdCmdElmt->RspData[4] = (_UCHAR8)(uiTotalSize & 0xff);
					}
					else
					{
						uiTotalSize = ((pPreRdCmdElmt->RspData[3]) << 8) + pPreRdCmdElmt->RspData[4];
					}

					spPreRdFillFileSt(ChannelId, pPreRdCmdElmt, uiBuffLeftLen);

					pChnStHandle->uiB2CurrentIdx++;
					B2CmdData[2] = pChnStHandle->uiB2CurrentIdx;
					pChnStHandle->uiB2Type = pPreRdCmdElmt->RspData[7];
					B2CmdData[3] = pChnStHandle->uiB2Type;

					if(0 != pChnStHandle->uiB2RecordSize)
					{
						pChnStHandle->uiB2TotalNum = uiTotalSize/(pChnStHandle->uiB2RecordSize);
						B2CmdData[4] = pChnStHandle->uiB2RecordSize;
					
						/* Send B2	*/
						SimComWriteData(ChannelId, B2CmdData, sizeof(B2CmdData));

						pChnStHandle->uiPreRdExpLen = 3 + B2CmdData[4];
						
						pChnStHandle->SPPreRDStatus = SPPRERD_SUBST_B2RSPWAIT;
						pPreRdCmdElmt->CmdLen = 5;
						memcpy(pPreRdCmdElmt->CmdData, B2CmdData, sizeof(B2CmdData));
					}
					else
					{
						SIMCOM_INFO("Channel %d B2 Read Error, RecordSize is 0!!!\n", ChannelId);	
						
						pChnStHandle->uiPreRdIdx++;

						if(PREREAD_END == PreReadFID[pChnStHandle->uiPreRdIdx].type)
						{		
							spPreRdSend(ChannelId); 	
						}
						else
						{
                            #if 0
							_UCHAR8 WriteData[5] = {0xa0, 0xa4, 0x0, 0x0, 0x2}; 															
							
							/* Send FileId*/
							SimComWriteData(ChannelId, WriteData, sizeof(WriteData));
														
							/* Send A0 A4 00 00 02 */
							pChnStHandle->SPPreRDStatus = SPPRERD_SUBST_A4INSWAIT;
                            #else
                            _UCHAR8 WriteDataEx[16] = {0xa0, 0xa4, 0x0, 0x0, 0x2,}; 
                            _UINT32 uiSizeTemp = 0;
                            
                            uiSizeTemp = 5;
                			memcpy(&WriteDataEx[uiSizeTemp], PreReadFID[pChnStHandle->uiPreRdIdx].fileid, sizeof(PreReadFID[pChnStHandle->uiPreRdIdx].fileid));
                			uiSizeTemp += sizeof(PreReadFID[pChnStHandle->uiPreRdIdx].fileid);
                			/* Send FileId*/
                			SimComWriteData(ChannelId, WriteDataEx, uiSizeTemp);
            											
            				pChnStHandle->SPPreRDStatus = SPPRERD_SUBST_A4RSPWAIT;	
                            #endif
						}
					}
				}	
			}
			else
			{	
				spPreRdFillFileSt(ChannelId, pPreRdCmdElmt, uiBuffLeftLen);
				spPreRdFillRootSt(ChannelId, pPreRdFileId, pPreRdFileId->Len);
				
				if(PREREAD_END == PreReadFID[pChnStHandle->uiPreRdIdx].type)
				{		
					spPreRdSend(ChannelId); 	
				}
				else
				{
                    #if 0
					_UCHAR8 WriteData[5] = {0xa0, 0xa4, 0x0, 0x0, 0x2}; 															
					
					/* Send FileId*/
					SimComWriteData(ChannelId, WriteData, sizeof(WriteData));
												
					/* Send A0 A4 00 00 02 */
					pChnStHandle->SPPreRDStatus = SPPRERD_SUBST_A4INSWAIT;
                    #else
                    _UCHAR8 WriteDataEx[16] = {0xa0, 0xa4, 0x0, 0x0, 0x2,}; 
                    _UINT32 uiSizeTemp = 0;
                    
                    uiSizeTemp = 5;
        			memcpy(&WriteDataEx[uiSizeTemp], PreReadFID[pChnStHandle->uiPreRdIdx].fileid, sizeof(PreReadFID[pChnStHandle->uiPreRdIdx].fileid));
        			uiSizeTemp += sizeof(PreReadFID[pChnStHandle->uiPreRdIdx].fileid);
        			/* Send FileId*/
        			SimComWriteData(ChannelId, WriteDataEx, uiSizeTemp);
    											
    				pChnStHandle->SPPreRDStatus = SPPRERD_SUBST_A4RSPWAIT;	
                    #endif
				}
			}
		}
	}
	else if(SPPRERD_SUBST_B0RSPWAIT == pChnStHandle->SPPreRDStatus)
	{
        if(pChnStHandle->uiPreRdExpLen == uiBuffLeftLen)
		{
			pPreRdCmdElmt->RspLen = htons(pChnStHandle->uiPreRdExpLen);
			for(i = 0; i < uiBuffLeftLen; i++)
			{
				pPreRdCmdElmt->RspData[i] = pChnStHandle->RspBuff[pChnStHandle->uiWriteOffset++];						
				pChnStHandle->uiWriteOffset &= (SIMPOOL_RSPREAD_MAX_SIZE - 1);
			}		

			/* Get ICCID	*/
			if((0x2f == pPreRdFileId->FileID[0]) && (0xe2 == pPreRdFileId->FileID[1]))
			{
				if(sizeof(pChnStHandle->ICCIDBuf) > ((uiBuffLeftLen - 3)*2))
				{
					memset(pChnStHandle->ICCIDBuf, 0, sizeof(pChnStHandle->ICCIDBuf));
					for(i = 1; i < (uiBuffLeftLen - 2); i++)
					{
						pChnStHandle->ICCIDBuf[(i - 1)*2] = (pPreRdCmdElmt->RspData[i] & 0x0f) + 0x30;
						if((pChnStHandle->ICCIDBuf[(i - 1)*2]) > 0x39)
						{
							pChnStHandle->ICCIDBuf[(i - 1)*2] += 0x07;
						}
						
						pChnStHandle->ICCIDBuf[(i - 1)*2 + 1] = ((pPreRdCmdElmt->RspData[i] & 0xf0) >> 4) + 0x30;
						if((pChnStHandle->ICCIDBuf[(i - 1)*2 + 1]) > 0x39)
						{
							pChnStHandle->ICCIDBuf[(i - 1)*2 + 1] += 0x07;
						}		
					}
					
					SIMCOM_INFO("Channel %d ICCID:%s\n", ChannelId, pChnStHandle->ICCIDBuf);
				}
				else
				{
					SIMCOM_INFO("Channel %d ICCID Buff Overflow\n", ChannelId);					
				}
			}

			/* Get IMSI	*/
			if((0x6f == pPreRdFileId->FileID[0]) && (0x07 == pPreRdFileId->FileID[1]))
			{
				memset(pChnStHandle->IMSIBuf, 0, sizeof(pChnStHandle->IMSIBuf));
				
				pChnStHandle->IMSIBuf[0] = ((pPreRdCmdElmt->RspData[2] & 0xf0) >> 4) + 0x30;
				if(sizeof(pChnStHandle->IMSIBuf) > ((uiBuffLeftLen - 4)*2 - 1))
				{
					for(i = 3; i < (uiBuffLeftLen - 2); i++)
					{
						pChnStHandle->IMSIBuf[(i - 3)*2 + 1] = (pPreRdCmdElmt->RspData[i] & 0x0f) + 0x30;
						pChnStHandle->IMSIBuf[(i - 3)*2 + 2] = ((pPreRdCmdElmt->RspData[i] & 0xf0) >> 4) + 0x30;
					}
					SIMCOM_INFO("Channel %d IMSI:%s\n", ChannelId, pChnStHandle->IMSIBuf);
				}
				else
				{
					SIMCOM_INFO("Channel %d IMSI Buff Overflow\n", ChannelId);										
				}
			}			
			
			spPreRdFillFileSt(ChannelId, pPreRdCmdElmt, uiBuffLeftLen);
			spPreRdFillRootSt(ChannelId, pPreRdFileId, pPreRdFileId->Len);

			if(PREREAD_END == PreReadFID[pChnStHandle->uiPreRdIdx].type)
			{		
				spPreRdSend(ChannelId); 	
			}
			else
			{
                #if 0
				_UCHAR8 WriteData[5] = {0xa0, 0xa4, 0x0, 0x0, 0x2}; 

				/* Send FileId*/
				SimComWriteData(ChannelId, WriteData, sizeof(WriteData));
											
				/* Send A0 A4 00 00 02 */
				pChnStHandle->SPPreRDStatus = SPPRERD_SUBST_A4INSWAIT;	
                #else
                _UCHAR8 WriteDataEx[16] = {0xa0, 0xa4, 0x0, 0x0, 0x2,}; 
                _UINT32 uiSizeTemp = 0;
                
                uiSizeTemp = 5;
    			memcpy(&WriteDataEx[uiSizeTemp], PreReadFID[pChnStHandle->uiPreRdIdx].fileid, sizeof(PreReadFID[pChnStHandle->uiPreRdIdx].fileid));
    			uiSizeTemp += sizeof(PreReadFID[pChnStHandle->uiPreRdIdx].fileid);
    			/* Send FileId*/
    			SimComWriteData(ChannelId, WriteDataEx, uiSizeTemp);
											
				pChnStHandle->SPPreRDStatus = SPPRERD_SUBST_A4RSPWAIT;	
                #endif
                
				pChnStHandle->uiB0CurrentIdx = 0;
				pChnStHandle->uiB2CurrentIdx = 0;
			}
		}
		else if(((pChnStHandle->RspBuff[pChnStHandle->uiWriteOffset] == 0x94) || (pChnStHandle->RspBuff[pChnStHandle->uiWriteOffset] == 0x98)) && (2 == uiBuffLeftLen))
		{
			pPreRdCmdElmt->RspLen = htons(uiBuffLeftLen);
			for(i = 0; i < uiBuffLeftLen; i++)
			{
				pPreRdCmdElmt->RspData[i] = pChnStHandle->RspBuff[pChnStHandle->uiWriteOffset++];						
				pChnStHandle->uiWriteOffset &= (SIMPOOL_RSPREAD_MAX_SIZE - 1);
			}		
			
			spPreRdFillFileSt(ChannelId, pPreRdCmdElmt, uiBuffLeftLen);
			spPreRdFillRootSt(ChannelId, pPreRdFileId, pPreRdFileId->Len);

			if(PREREAD_END == PreReadFID[pChnStHandle->uiPreRdIdx].type)
			{	
				spPreRdSend(ChannelId); 	
			}
			else
			{
				#if 0
				_UCHAR8 WriteData[5] = {0xa0, 0xa4, 0x0, 0x0, 0x2}; 

				/* Send FileId*/
				SimComWriteData(ChannelId, WriteData, sizeof(WriteData));
											
				/* Send A0 A4 00 00 02 */
				pChnStHandle->SPPreRDStatus = SPPRERD_SUBST_A4INSWAIT;	
                #else
                _UCHAR8 WriteDataEx[16] = {0xa0, 0xa4, 0x0, 0x0, 0x2,}; 
                _UINT32 uiSizeTemp = 0;
                
                uiSizeTemp = 5;
    			memcpy(&WriteDataEx[uiSizeTemp], PreReadFID[pChnStHandle->uiPreRdIdx].fileid, sizeof(PreReadFID[pChnStHandle->uiPreRdIdx].fileid));
    			uiSizeTemp += sizeof(PreReadFID[pChnStHandle->uiPreRdIdx].fileid);
    			/* Send FileId*/
    			SimComWriteData(ChannelId, WriteDataEx, uiSizeTemp);
											
				pChnStHandle->SPPreRDStatus = SPPRERD_SUBST_A4RSPWAIT;	
                #endif
				
				pChnStHandle->uiB0CurrentIdx = 0;
				pChnStHandle->uiB2CurrentIdx = 0;
			}
		}
	}
	else if(SPPRERD_SUBST_B2RSPWAIT == pChnStHandle->SPPreRDStatus)
	{
		if(pChnStHandle->uiPreRdExpLen == uiBuffLeftLen)
		{
			pPreRdCmdElmt->RspLen = htons(pChnStHandle->uiPreRdExpLen);
			for(i = 0; i < uiBuffLeftLen; i++)
			{
				pPreRdCmdElmt->RspData[i] = pChnStHandle->RspBuff[pChnStHandle->uiWriteOffset++];						
				pChnStHandle->uiWriteOffset &= (SIMPOOL_RSPREAD_MAX_SIZE - 1);
			}	
			
			spPreRdFillFileSt(ChannelId, pPreRdCmdElmt, uiBuffLeftLen);

#if 0
			/* Get PHONEBOOK	*/
			if((0x6f == pPreRdFileId->FileID[0]) && (0x3a == pPreRdFileId->FileID[1]) && (6 == pChnStHandle->uiB2CurrentIdx))
			{
				_UINT32 uiXLen = 0;

				uiXLen = (uiBuffLeftLen - 3) - 14;
				
				if(1)//if(sizeof(pChnStHandle->IMEIBuf) > ((uiBuffLeftLen - 3)*2))
				{
					memset(pChnStHandle->IMEIBuf, 0, sizeof(pChnStHandle->IMEIBuf));
					SIMCOM_INFO("Channel %d IMEI LEN:0x%x\n", ChannelId, pPreRdCmdElmt->RspData[uiXLen + 1]);

					for(i = 0; i < (pPreRdCmdElmt->RspData[uiXLen + 1] - 1); i++)
					{
						SIMCOM_INFO("Channel %d IMEI(%d):0x%x\n", ChannelId, i, pPreRdCmdElmt->RspData[uiXLen + 3 + i]);

						
						pChnStHandle->IMEIBuf[(i)*2 + 0] = (pPreRdCmdElmt->RspData[uiXLen + 3 + i] & 0x0f) + 0x30;
						pChnStHandle->IMEIBuf[(i)*2 + 1] = ((pPreRdCmdElmt->RspData[uiXLen + 3 + i] & 0xf0) >> 4) + 0x30;
					}
					pChnStHandle->IMEIBuf[15] = 0;
					
					SIMCOM_INFO("Channel %d IMEI:%s\n", ChannelId, pChnStHandle->IMEIBuf);
				}
				else
				{
					SIMCOM_INFO("Channel %d IMEI Buff Overflow\n", ChannelId);					
				}
			}
#endif			

			if(pChnStHandle->uiB2CurrentIdx < pChnStHandle->uiB2TotalNum)
			{
				_UCHAR8 B2CmdData[5] = {0xA0, 0xB2, 0x00, 0x00, 0x00};

				pChnStHandle->uiB2CurrentIdx++;
				B2CmdData[2] = pChnStHandle->uiB2CurrentIdx;
				B2CmdData[3] = pChnStHandle->uiB2Type;

				B2CmdData[4] = pChnStHandle->uiB2RecordSize;

				/* Send B2	*/
				SimComWriteData(ChannelId, B2CmdData, sizeof(B2CmdData));

				pChnStHandle->uiPreRdExpLen = 3 + B2CmdData[4];

				pChnStHandle->SPPreRDStatus = SPPRERD_SUBST_B2RSPWAIT;
				pPreRdCmdElmt->CmdLen = 5;
				
				memcpy(pPreRdCmdElmt->CmdData, B2CmdData, sizeof(B2CmdData));

			}
			else
			{
				spPreRdFillRootSt(ChannelId, pPreRdFileId, pPreRdFileId->Len);

				if(PREREAD_END == PreReadFID[pChnStHandle->uiPreRdIdx].type)
				{		
					spPreRdSend(ChannelId); 	
				}
				else
				{
					#if 0
    				_UCHAR8 WriteData[5] = {0xa0, 0xa4, 0x0, 0x0, 0x2}; 

    				/* Send FileId*/
    				SimComWriteData(ChannelId, WriteData, sizeof(WriteData));
    											
    				/* Send A0 A4 00 00 02 */
    				pChnStHandle->SPPreRDStatus = SPPRERD_SUBST_A4INSWAIT;	
                    #else
                    _UCHAR8 WriteDataEx[16] = {0xa0, 0xa4, 0x0, 0x0, 0x2,}; 
                    _UINT32 uiSizeTemp = 0;
                    
                    uiSizeTemp = 5;
        			memcpy(&WriteDataEx[uiSizeTemp], PreReadFID[pChnStHandle->uiPreRdIdx].fileid, sizeof(PreReadFID[pChnStHandle->uiPreRdIdx].fileid));
        			uiSizeTemp += sizeof(PreReadFID[pChnStHandle->uiPreRdIdx].fileid);
        			/* Send FileId*/
        			SimComWriteData(ChannelId, WriteDataEx, uiSizeTemp);
    											
    				pChnStHandle->SPPreRDStatus = SPPRERD_SUBST_A4RSPWAIT;	
                    #endif
					
					pChnStHandle->uiB0CurrentIdx = 0;
					pChnStHandle->uiB2CurrentIdx = 0;
				}
			}
		}
		else if(((pChnStHandle->RspBuff[pChnStHandle->uiWriteOffset] == 0x94) || (pChnStHandle->RspBuff[pChnStHandle->uiWriteOffset] == 0x98)) && (2 == uiBuffLeftLen))
		{
			pPreRdCmdElmt->RspLen = htons(uiBuffLeftLen);
			for(i = 0; i < uiBuffLeftLen; i++)
			{
				pPreRdCmdElmt->RspData[i] = pChnStHandle->RspBuff[pChnStHandle->uiWriteOffset++];						
				pChnStHandle->uiWriteOffset &= (SIMPOOL_RSPREAD_MAX_SIZE - 1);
			}	
			
			spPreRdFillFileSt(ChannelId, pPreRdCmdElmt, uiBuffLeftLen);
			spPreRdFillRootSt(ChannelId, pPreRdFileId, pPreRdFileId->Len);

			if(PREREAD_END == PreReadFID[pChnStHandle->uiPreRdIdx].type)
			{	
				spPreRdSend(ChannelId); 	
			}
			else
			{
				#if 0
				_UCHAR8 WriteData[5] = {0xa0, 0xa4, 0x0, 0x0, 0x2}; 

				/* Send FileId*/
				SimComWriteData(ChannelId, WriteData, sizeof(WriteData));
											
				/* Send A0 A4 00 00 02 */
				pChnStHandle->SPPreRDStatus = SPPRERD_SUBST_A4INSWAIT;	
                #else
                _UCHAR8 WriteDataEx[16] = {0xa0, 0xa4, 0x0, 0x0, 0x2,}; 
                _UINT32 uiSizeTemp = 0;
                
                uiSizeTemp = 5;
    			memcpy(&WriteDataEx[uiSizeTemp], PreReadFID[pChnStHandle->uiPreRdIdx].fileid, sizeof(PreReadFID[pChnStHandle->uiPreRdIdx].fileid));
    			uiSizeTemp += sizeof(PreReadFID[pChnStHandle->uiPreRdIdx].fileid);
    			/* Send FileId*/
    			SimComWriteData(ChannelId, WriteDataEx, uiSizeTemp);
											
				pChnStHandle->SPPreRDStatus = SPPRERD_SUBST_A4RSPWAIT;	
                #endif
				
				pChnStHandle->uiB0CurrentIdx = 0;
				pChnStHandle->uiB2CurrentIdx = 0;
			}
		}
	}					
	
	return EOS_OK;
}



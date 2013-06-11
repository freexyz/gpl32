/*
* Purpose: message q manager
*
* Author: zhangzha
*
* Date: 2008/05/29
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 0.01
* History :
*          1 2008/5/29 zhangzha creat
*/

#include "msg_manager.h"

#define MSG_Q_DEBUG			0

#define MSG_Q_LOCK()		OSSchedLock()
#define MSG_Q_UNLOCK()		OSSchedUnlock()

/* Prototypes */
INT8U msgQQuery(MSG_Q_ID msgQId, OS_Q_DATA *pdata);
INT32U msgQSizeGet(MSG_Q_ID msgQId);

//========================================================
//Function Name:msgQCreate
//Syntax:		MSG_Q_ID msgQCreate(INT32U maxQSize, INT32U maxMsgs, INT32U maxMsgLength)
//Purpose:		create and initialize a message queue
//Note:
//Parameters:   INT32U maxQSize			/* max queue can be creat */
//				INT32U	maxMsgs			/* max messages that can be queued */
//				INT32U	maxMsgLength	/* max bytes in a message */
//Return:		NULL if faile
//=======================================================
MSG_Q_ID msgQCreate(INT32U maxQSize, INT32U maxMsgs, INT32U maxMsgLength)
{
	MSG_Q_ID	msgQId;
	OS_EVENT*	pEvent;
	void*		pMsgQ;
	INT32U		msg_length = (maxMsgLength + 4 - 1) & (~(4 - 1));
	INT32U		size = sizeof(*msgQId) + 						/* MSG_Q_ID struct size */
					maxQSize * sizeof(void*) + 					/* OS Q size */
					maxMsgs * (msg_length + 4);				/* type + message size */



	msgQId = (MSG_Q_ID)gp_malloc(size);
	if(msgQId == NULL)
	{
	#if MSG_Q_DEBUG == 1
		DBG_PRINT("msg create err:memory allocate fail\r\n");
	#endif
		return NULL;
	}

	gp_memset((INT8S *)msgQId, 0, size);	/* clear out msg q structure */

	pMsgQ = (void*)(msgQId + 1);
	pEvent = OSQCreate(pMsgQ, maxQSize);
	if(pEvent == NULL)						/* creat q faile */
	{
		gp_free(msgQId);
	#if MSG_Q_DEBUG == 1
		DBG_PRINT("msg create err:queue create fail\r\n");
	#endif
		return NULL;
	}

	msgQId->pEvent = pEvent;
	msgQId->pMsgQ = pMsgQ;
	msgQId->maxMsgs = maxMsgs;
	msgQId->maxMsgLength = msg_length;
	msgQId->pMsgPool = (INT8U*)((INT32U*)pMsgQ + maxQSize);

	return ((MSG_Q_ID) msgQId);
}

//========================================================
//Function Name:msgQDelete
//Syntax:		void msgQDelete (MSG_Q_ID msgQId)
//Purpose:		delete a message queue
//Note:
//Parameters:   MSG_Q_ID msgQId		/* message queue to delete */
//Return:
//=======================================================
void msgQDelete (MSG_Q_ID msgQId)
{
	INT8U err;

	MSG_Q_LOCK();

	OSQDel(msgQId->pEvent, OS_DEL_ALWAYS, &err);
	gp_free((void *)msgQId);

	MSG_Q_UNLOCK();
}

//========================================================
//Function Name:msgQSend
//Syntax:		INT32S msgQSend(MSG_Q_ID msgQId, INT32U msg_id, void *para, INT32U nParaByte, INT32U priority)
//Purpose:		send a message to a message queue
//Note:
//Parameters:   MSG_Q_ID msgQId			/* message queue on which to send */
//				INT32U msg_id			/* message id */
//				void *para				/* message to send */
//				INT32U nParaByte		/* byte number of para buffer */
//				INT32U priority			/* MSG_PRI_NORMAL or MSG_PRI_URGENT */
//Return:		-1 if faile
//				0 success
//=======================================================
INT32S msgQSend(MSG_Q_ID msgQId, INT32U msg_id, void *para, INT32U nParaByte, INT32U priority)
{
	INT8U	ret;
	INT32U	i;
	INT8U	*pMsg = msgQId->pMsgPool;
	INT32U	maxMsgLength = msgQId->maxMsgLength;
	INT32U	maxMsgs = msgQId->maxMsgs;

	if(nParaByte > maxMsgLength)		/* too many parameter */
	{
	#if MSG_Q_DEBUG == 1
		DBG_PRINT("msg send err:message size is too large\r\n");
	#endif
		return -1;
	}

	MSG_Q_LOCK();

	/* find a free message */
	for(i = 0; i < maxMsgs; i++)
	{
		if(*(INT32U*)pMsg == 0)
			break;

		pMsg += (maxMsgLength + 4);
	}
	if(i == maxMsgs)					/* not enough message */
	{
		MSG_Q_UNLOCK();
	#if MSG_Q_DEBUG == 1
		DBG_PRINT("msg send err:allocate message block faile\r\n");
	#endif
		return -1;
	}

	*(INT32U*)pMsg = msg_id;
	if(nParaByte)
	{
		gp_memcpy((INT8S*)(pMsg + 4), (INT8S*)para, nParaByte);
	}

	if(priority == MSG_PRI_NORMAL)
		ret = OSQPost(msgQId->pEvent, (void *)pMsg);
	else
		ret = OSQPostFront(msgQId->pEvent, (void *)pMsg);

#if MSG_Q_DEBUG == 1
	if(ret)
	{
		DBG_PRINT("msg send err:post message faile\r\n");
	}
#endif

	MSG_Q_UNLOCK();
	return (INT32S)ret;
}

//========================================================
//Function Name:msgQReceive
//Syntax:		INT32S msgQReceive(MSG_Q_ID msgQId, INT32U *msg_id, void *para, INT32U maxParaNByte)
//Purpose:		receive a message from a message queue
//Note:
//Parameters:   MSG_Q_ID msgQId			/* message queue on which to send */
//				INT32U *msg_id			/* message id */
//				void *para				/* message and type received */
//				INT32U maxNByte			/* message size */
//Return:		-1: if faile
//				0: success
//=======================================================
INT32S msgQReceive(MSG_Q_ID msgQId, INT32U *msg_id, void *para, INT32U maxParaNByte)
{
	INT8U err;
	void *pMsg;

	pMsg = OSQPend(msgQId->pEvent, 0, &err);
	if (err!=0 || !pMsg) {
	#if MSG_Q_DEBUG == 1
		DBG_PRINT("msg receive err:pend message fail\r\n");
	#endif
		return -1;
	}

	MSG_Q_LOCK();

	if(maxParaNByte > msgQId->maxMsgLength)
	{
		MSG_Q_UNLOCK();
	#if MSG_Q_DEBUG == 1
		DBG_PRINT("msg receive err:message size is too large\r\n");
	#endif
		return -1;
	}
	*msg_id = *(INT32U*)pMsg;
	if(maxParaNByte && para)
	{
		gp_memcpy((INT8S*)para, (INT8S*)pMsg + 4, maxParaNByte);
	}
	*(INT32U*)pMsg = 0;				/* free message */

	MSG_Q_UNLOCK();
	return 0;
}

//========================================================
//Function Name:msgQAccept
//Syntax:		INT32S msgQAccept(MSG_Q_ID msgQId, INT32U *msg_id, void *para, INT32U maxParaNByte)
//Purpose:		Check whether a message is available from a message queue
//Note:
//Parameters:   MSG_Q_ID msgQId			/* message queue on which to send */
//				INT32U *msg_id			/* message id */
//				void *para				/* message and type received */
//				INT32U maxNByte			/* message size */
//Return:		-1: queue is empty or fail
//				0: success
//=======================================================
INT32S msgQAccept(MSG_Q_ID msgQId, INT32U *msg_id, void *para, INT32U maxParaNByte)
{
	INT8U err;
	void *pMsg;

	pMsg = OSQAccept(msgQId->pEvent, &err);
	if (err!=OS_NO_ERR || !pMsg) {
		return -1;
	}

	MSG_Q_LOCK();

	if (maxParaNByte > msgQId->maxMsgLength) {
	  	MSG_Q_UNLOCK();
	  #if MSG_Q_DEBUG == 1
		DBG_PRINT("msg receive err:message size is too large\r\n");
	  #endif

		return -1;
	}
	*msg_id = *((INT32U *) pMsg);
	if (maxParaNByte && para) {
		gp_memcpy((INT8S *) para, (INT8S *) pMsg + 4, maxParaNByte);
	}
	*(INT32U *) pMsg = 0;				/* free message */

	MSG_Q_UNLOCK();

	return 0;
}

//========================================================
//Function Name:msgQFlush
//Syntax:		void msgQFlush(MSG_Q_ID msgQId)
//Purpose:		flush message queue
//Note:
//Parameters:   MSG_Q_ID msgQId
//Return:
//=======================================================
void msgQFlush(MSG_Q_ID msgQId)
{
	MSG_Q_LOCK();
	OSQFlush(msgQId->pEvent);
	gp_memset( (INT8S *)msgQId->pMsgPool, 0, msgQId->maxMsgs * (msgQId->maxMsgLength + 4) );
	MSG_Q_UNLOCK();
}

INT8U msgQQuery(MSG_Q_ID msgQId, OS_Q_DATA *pdata)
{
    INT8U ret;

	OSSchedLock();
    ret = OSQQuery(msgQId->pEvent, pdata);
	OSSchedUnlock();
    return ret;
}

INT32U msgQSizeGet(MSG_Q_ID msgQId)
{
    INT32U ret;
    OS_Q_DATA query_data;

    msgQQuery(msgQId, &query_data);
    ret = (INT32U) query_data.OSNMsgs;
    return ret;
}


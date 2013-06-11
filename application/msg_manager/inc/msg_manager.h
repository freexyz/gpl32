#ifndef __TURNKEY_MSG_MANAGER_H__
#define __TURNKEY_MSG_MANAGER_H__
#include "application.h"

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
extern MSG_Q_ID msgQCreate(INT32U maxQSize, INT32U maxMsgs, INT32U maxMsgLength);

//========================================================
//Function Name:msgQDelete
//Syntax:		void msgQDelete (MSG_Q_ID msgQId)
//Purpose:		delete a message queue
//Note:			
//Parameters:   MSG_Q_ID msgQId		/* message queue to delete */
//Return:		
//=======================================================
extern void msgQDelete (MSG_Q_ID msgQId);

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
extern INT32S msgQSend(MSG_Q_ID msgQId, INT32U msg_id, void *para, INT32U nParaByte, INT32U priority);

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
extern INT32S msgQReceive(MSG_Q_ID msgQId, INT32U *msg_id, void *para, INT32U maxParaNByte);

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
extern INT32S msgQAccept(MSG_Q_ID msgQId, INT32U *msg_id, void *para, INT32U maxParaNByte);

//========================================================
//Function Name:msgQFlush
//Syntax:		void msgQFlush(MSG_Q_ID msgQId)
//Purpose:		flush message queue
//Note:			
//Parameters:   MSG_Q_ID msgQId
//Return:		
//=======================================================
extern void msgQFlush(MSG_Q_ID msgQId);

//========================================================
//Function Name:msgQQuery
//Syntax:		INT8U msgQQuery(MSG_Q_ID msgQId, OS_Q_DATA *pdata)
//Purpose:		get current Q message information
//Note:			
//Parameters:   MSG_Q_ID msgQId, OS_Q_DATA *pdata
//Return:		
//=======================================================
extern INT8U msgQQuery(MSG_Q_ID msgQId, OS_Q_DATA *pdata);

//========================================================
//Function Name:msgQSizeGet
//Syntax:		INT32U msgQSizeGet(MSG_Q_ID msgQId)
//Purpose:		get current Q message number
//Note:			
//Parameters:   MSG_Q_ID msgQId
//Return:		
//=======================================================
extern INT32U msgQSizeGet(MSG_Q_ID msgQId);

#endif /* __TURNKEY_MSG_MANAGER_H__ */
#ifndef __OS_H__
#define __OS_H__


#include "project.h"
#include "os_cfg.h"


//#if _OPERATING_SYSTEM == _OS_UCOS2
	typedef INT32U 		OS_STK;     // Each stack entry is 32-bit wide
	
	/*
	*********************************************************************************************************
	*                                             MISCELLANEOUS
	*********************************************************************************************************
	*/
	
	#ifdef   OS_GLOBALS
	#define  OS_EXT
	#else
	#define  OS_EXT  extern
	#endif
	
	#ifndef  FALSE
	#define  FALSE                        0u
	#endif
	
	#ifndef  TRUE
	#define  TRUE                         1u
	#endif
	
	#define  OS_ASCII_NUL          (INT8U)0
	
	#define  OS_PRIO_SELF              0xFFu                /* Indicate SELF priority                      */
	
	#if OS_TASK_STAT_EN > 0
	#define  OS_N_SYS_TASKS               2u                /* Number of system tasks                      */
	#else
	#define  OS_N_SYS_TASKS               1u
	#endif
	
	#define  OS_STAT_PRIO       (OS_LOWEST_PRIO - 1)        /* Statistic task priority                     */
	#define  OS_IDLE_PRIO       (OS_LOWEST_PRIO)            /* IDLE      task priority                     */
	
	#if OS_LOWEST_PRIO <= 63
	#define  OS_EVENT_TBL_SIZE ((OS_LOWEST_PRIO) / 8 + 1)   /* Size of event table                         */
	#define  OS_RDY_TBL_SIZE   ((OS_LOWEST_PRIO) / 8 + 1)   /* Size of ready table                         */
	#else
	#define  OS_EVENT_TBL_SIZE ((OS_LOWEST_PRIO) / 16 + 1)  /* Size of event table                         */
	#define  OS_RDY_TBL_SIZE   ((OS_LOWEST_PRIO) / 16 + 1)  /* Size of ready table                         */
	#endif
	
	#define  OS_TASK_IDLE_ID          65535u                /* I.D. numbers for Idle and Stat tasks        */
	#define  OS_TASK_STAT_ID          65534u
	
	#define  OS_EVENT_EN       (((OS_Q_EN > 0) && (OS_MAX_QS > 0)) || (OS_MBOX_EN > 0) || (OS_SEM_EN > 0) || (OS_MUTEX_EN > 0))
	
	/*
	*********************************************************************************************************
	*                              TASK STATUS (Bit definition for OSTCBStat)
	*********************************************************************************************************
	*/
	#define  OS_STAT_RDY               0x00u    /* Ready to run                                            */
	#define  OS_STAT_SEM               0x01u    /* Pending on semaphore                                    */
	#define  OS_STAT_MBOX              0x02u    /* Pending on mailbox                                      */
	#define  OS_STAT_Q                 0x04u    /* Pending on queue                                        */
	#define  OS_STAT_SUSPEND           0x08u    /* Task is suspended                                       */
	#define  OS_STAT_MUTEX             0x10u    /* Pending on mutual exclusion semaphore                   */
	#define  OS_STAT_FLAG              0x20u    /* Pending on event flag group                             */
	
	#define  OS_STAT_PEND_ANY         (OS_STAT_SEM | OS_STAT_MBOX | OS_STAT_Q | OS_STAT_MUTEX | OS_STAT_FLAG)
	
	/*
	*********************************************************************************************************
	*                                        OS_EVENT types
	*********************************************************************************************************
	*/
	#define  OS_EVENT_TYPE_UNUSED         0u
	#define  OS_EVENT_TYPE_MBOX           1u
	#define  OS_EVENT_TYPE_Q              2u
	#define  OS_EVENT_TYPE_SEM            3u
	#define  OS_EVENT_TYPE_MUTEX          4u
	#define  OS_EVENT_TYPE_FLAG           5u
	
	/*
	*********************************************************************************************************
	*                                         EVENT FLAGS
	*********************************************************************************************************
	*/
	#define  OS_FLAG_WAIT_CLR_ALL         0u    /* Wait for ALL    the bits specified to be CLR (i.e. 0)   */
	#define  OS_FLAG_WAIT_CLR_AND         0u
	
	#define  OS_FLAG_WAIT_CLR_ANY         1u    /* Wait for ANY of the bits specified to be CLR (i.e. 0)   */
	#define  OS_FLAG_WAIT_CLR_OR          1u
	
	#define  OS_FLAG_WAIT_SET_ALL         2u    /* Wait for ALL    the bits specified to be SET (i.e. 1)   */
	#define  OS_FLAG_WAIT_SET_AND         2u
	
	#define  OS_FLAG_WAIT_SET_ANY         3u    /* Wait for ANY of the bits specified to be SET (i.e. 1)   */
	#define  OS_FLAG_WAIT_SET_OR          3u
	
	
	#define  OS_FLAG_CONSUME           0x80u    /* Consume the flags if condition(s) satisfied             */
	
	
	#define  OS_FLAG_CLR                  0u
	#define  OS_FLAG_SET                  1u
	
	/*
	*********************************************************************************************************
	*                                   Values for OSTickStepState
	*
	* Note(s): This feature is used by uC/OS-View.
	*********************************************************************************************************
	*/
	
	#if OS_TICK_STEP_EN > 0
	#define  OS_TICK_STEP_DIS             0u    /* Stepping is disabled, tick runs as mormal               */
	#define  OS_TICK_STEP_WAIT            1u    /* Waiting for uC/OS-View to set OSTickStepState to _ONCE  */
	#define  OS_TICK_STEP_ONCE            2u    /* Process tick once and wait for next cmd from uC/OS-View */
	#endif
	
	/*
	*********************************************************************************************************
	*       Possible values for 'opt' argument of OSSemDel(), OSMboxDel(), OSQDel() and OSMutexDel()
	*********************************************************************************************************
	*/
	#define  OS_DEL_NO_PEND               0u
	#define  OS_DEL_ALWAYS                1u
	
	/*
	*********************************************************************************************************
	*                                     OS???PostOpt() OPTIONS
	*
	* These #defines are used to establish the options for OSMboxPostOpt() and OSQPostOpt().
	*********************************************************************************************************
	*/
	#define  OS_POST_OPT_NONE          0x00u    /* NO option selected                                      */
	#define  OS_POST_OPT_BROADCAST     0x01u    /* Broadcast message to ALL tasks waiting                  */
	#define  OS_POST_OPT_FRONT         0x02u    /* Post to highest priority task waiting                   */
	
	/*
	*********************************************************************************************************
	*                                 TASK OPTIONS (see OSTaskCreateExt())
	*********************************************************************************************************
	*/
	#define  OS_TASK_OPT_NONE        0x0000u    /* NO option selected                                      */
	#define  OS_TASK_OPT_STK_CHK     0x0001u    /* Enable stack checking for the task                      */
	#define  OS_TASK_OPT_STK_CLR     0x0002u    /* Clear the stack when the task is create                 */
	#define  OS_TASK_OPT_SAVE_FP     0x0004u    /* Save the contents of any floating-point registers       */
	
	/*
	*********************************************************************************************************
	*                                             ERROR CODES
	*********************************************************************************************************
	*/
	#define OS_NO_ERR                     0u
	
	#define OS_ERR_EVENT_TYPE             1u
	#define OS_ERR_PEND_ISR               2u
	#define OS_ERR_POST_NULL_PTR          3u
	#define OS_ERR_PEVENT_NULL            4u
	#define OS_ERR_POST_ISR               5u
	#define OS_ERR_QUERY_ISR              6u
	#define OS_ERR_INVALID_OPT            7u
	#define OS_ERR_TASK_WAITING           8u
	#define OS_ERR_PDATA_NULL             9u
	
	#define OS_TIMEOUT                   10u
	#define OS_TASK_NOT_EXIST            11u
	#define OS_ERR_EVENT_NAME_TOO_LONG   12u
	#define OS_ERR_FLAG_NAME_TOO_LONG    13u
	#define OS_ERR_TASK_NAME_TOO_LONG    14u
	#define OS_ERR_PNAME_NULL            15u
	#define OS_ERR_TASK_CREATE_ISR       16u
	
	#define OS_MBOX_FULL                 20u
	
	#define OS_Q_FULL                    30u
	#define OS_Q_EMPTY                   31u
	
	#define OS_PRIO_EXIST                40u
	#define OS_PRIO_ERR                  41u
	#define OS_PRIO_INVALID              42u
	
	#define OS_SEM_OVF                   50u
	
	#define OS_TASK_DEL_ERR              60u
	#define OS_TASK_DEL_IDLE             61u
	#define OS_TASK_DEL_REQ              62u
	#define OS_TASK_DEL_ISR              63u
	
	#define OS_NO_MORE_TCB               70u
	
	#define OS_TIME_NOT_DLY              80u
	#define OS_TIME_INVALID_MINUTES      81u
	#define OS_TIME_INVALID_SECONDS      82u
	#define OS_TIME_INVALID_MILLI        83u
	#define OS_TIME_ZERO_DLY             84u
	
	#define OS_TASK_SUSPEND_PRIO         90u
	#define OS_TASK_SUSPEND_IDLE         91u
	
	#define OS_TASK_RESUME_PRIO         100u
	#define OS_TASK_NOT_SUSPENDED       101u
	
	#define OS_MEM_INVALID_PART         110u
	#define OS_MEM_INVALID_BLKS         111u
	#define OS_MEM_INVALID_SIZE         112u
	#define OS_MEM_NO_FREE_BLKS         113u
	#define OS_MEM_FULL                 114u
	#define OS_MEM_INVALID_PBLK         115u
	#define OS_MEM_INVALID_PMEM         116u
	#define OS_MEM_INVALID_PDATA        117u
	#define OS_MEM_INVALID_ADDR         118u
	#define OS_MEM_NAME_TOO_LONG        119u
	
	#define OS_ERR_NOT_MUTEX_OWNER      120u
	
	#define OS_TASK_OPT_ERR             130u
	
	#define OS_ERR_DEL_ISR              140u
	#define OS_ERR_CREATE_ISR           141u
	
	#define OS_FLAG_INVALID_PGRP        150u
	#define OS_FLAG_ERR_WAIT_TYPE       151u
	#define OS_FLAG_ERR_NOT_RDY         152u
	#define OS_FLAG_INVALID_OPT         153u
	#define OS_FLAG_GRP_DEPLETED        154u
	
	#define OS_ERR_PIP_LOWER            160u
	
	/*
	*********************************************************************************************************
	*                                          EVENT CONTROL BLOCK
	*********************************************************************************************************
	*/
	
	#if OS_EVENT_EN && (OS_MAX_EVENTS > 0)
	typedef struct os_event {
	    INT8U    OSEventType;                    /* Type of event control block (see OS_EVENT_TYPE_xxxx)    */
	    void    *OSEventPtr;                     /* Pointer to message or queue structure                   */
	    INT16U   OSEventCnt;                     /* Semaphore Count (not used if other EVENT type)          */
	#if OS_LOWEST_PRIO <= 63
	    INT8U    OSEventGrp;                     /* Group corresponding to tasks waiting for event to occur */
	    INT8U    OSEventTbl[OS_EVENT_TBL_SIZE];  /* List of tasks waiting for event to occur                */
	#else
	    INT16U   OSEventGrp;                     /* Group corresponding to tasks waiting for event to occur */
	    INT16U   OSEventTbl[OS_EVENT_TBL_SIZE];  /* List of tasks waiting for event to occur                */
	#endif
	
	#if OS_EVENT_NAME_SIZE > 1
	    INT8U    OSEventName[OS_EVENT_NAME_SIZE];
	#endif
	} OS_EVENT;
	#endif
	
	
	/*
	*********************************************************************************************************
	*                                       EVENT FLAGS CONTROL BLOCK
	*********************************************************************************************************
	*/
	
	#if (OS_VERSION >= 251) && (OS_FLAG_EN > 0) && (OS_MAX_FLAGS > 0)
	
	#if OS_FLAGS_NBITS == 8                     /* Determine the size of OS_FLAGS (8, 16 or 32 bits)       */
	typedef  INT8U    OS_FLAGS;
	#endif
	
	#if OS_FLAGS_NBITS == 16
	typedef  INT16U   OS_FLAGS;
	#endif
	
	#if OS_FLAGS_NBITS == 32
	typedef  INT32U   OS_FLAGS;
	#endif
	
	
	typedef struct os_flag_grp {                /* Event Flag Group                                        */
	    INT8U         OSFlagType;               /* Should be set to OS_EVENT_TYPE_FLAG                     */
	    void         *OSFlagWaitList;           /* Pointer to first NODE of task waiting on event flag     */
	    OS_FLAGS      OSFlagFlags;              /* 8, 16 or 32 bit flags                                   */
	#if OS_FLAG_NAME_SIZE > 1
	    INT8U         OSFlagName[OS_FLAG_NAME_SIZE];
	#endif
	} OS_FLAG_GRP;
	
	
	
	typedef struct os_flag_node {               /* Event Flag Wait List Node                               */
	    void         *OSFlagNodeNext;           /* Pointer to next     NODE in wait list                   */
	    void         *OSFlagNodePrev;           /* Pointer to previous NODE in wait list                   */
	    void         *OSFlagNodeTCB;            /* Pointer to TCB of waiting task                          */
	    void         *OSFlagNodeFlagGrp;        /* Pointer to Event Flag Group                             */
	    OS_FLAGS      OSFlagNodeFlags;          /* Event flag to wait on                                   */
	    INT8U         OSFlagNodeWaitType;       /* Type of wait:                                           */
	                                            /*      OS_FLAG_WAIT_AND                                   */
	                                            /*      OS_FLAG_WAIT_ALL                                   */
	                                            /*      OS_FLAG_WAIT_OR                                    */
	                                            /*      OS_FLAG_WAIT_ANY                                   */
	} OS_FLAG_NODE;
	#endif
	
	/*
	*********************************************************************************************************
	*                                          MESSAGE MAILBOX DATA
	*********************************************************************************************************
	*/
	
	#if OS_MBOX_EN > 0
	typedef struct os_mbox_data {
	    void   *OSMsg;                         /* Pointer to message in mailbox                            */
	#if OS_LOWEST_PRIO <= 63
	    INT8U   OSEventTbl[OS_EVENT_TBL_SIZE]; /* List of tasks waiting for event to occur                 */
	    INT8U   OSEventGrp;                    /* Group corresponding to tasks waiting for event to occur  */
	#else
	    INT16U  OSEventTbl[OS_EVENT_TBL_SIZE]; /* List of tasks waiting for event to occur                 */
	    INT16U  OSEventGrp;                    /* Group corresponding to tasks waiting for event to occur  */
	#endif
	} OS_MBOX_DATA;
	#endif
	
	/*
	*********************************************************************************************************
	*                                     MEMORY PARTITION DATA STRUCTURES
	*********************************************************************************************************
	*/
	
	#if (OS_MEM_EN > 0) && (OS_MAX_MEM_PART > 0)
	typedef struct os_mem {                   /* MEMORY CONTROL BLOCK                                      */
	    void   *OSMemAddr;                    /* Pointer to beginning of memory partition                  */
	    void   *OSMemFreeList;                /* Pointer to list of free memory blocks                     */
	    INT32U  OSMemBlkSize;                 /* Size (in bytes) of each block of memory                   */
	    INT32U  OSMemNBlks;                   /* Total number of blocks in this partition                  */
	    INT32U  OSMemNFree;                   /* Number of memory blocks remaining in this partition       */
	#if OS_MEM_NAME_SIZE > 1
	    INT8U   OSMemName[OS_MEM_NAME_SIZE];  /* Memory partition name                                     */
	#endif
	} OS_MEM;
	
	
	typedef struct os_mem_data {
	    void   *OSAddr;                    /* Pointer to the beginning address of the memory partition     */
	    void   *OSFreeList;                /* Pointer to the beginning of the free list of memory blocks   */
	    INT32U  OSBlkSize;                 /* Size (in bytes) of each memory block                         */
	    INT32U  OSNBlks;                   /* Total number of blocks in the partition                      */
	    INT32U  OSNFree;                   /* Number of memory blocks free                                 */
	    INT32U  OSNUsed;                   /* Number of memory blocks used                                 */
	} OS_MEM_DATA;
	#endif
	
	/*
	*********************************************************************************************************
	*                                    MUTUAL EXCLUSION SEMAPHORE DATA
	*********************************************************************************************************
	*/
	
	#if OS_MUTEX_EN > 0
	typedef struct os_mutex_data {
	#if OS_LOWEST_PRIO <= 63
	    INT8U   OSEventTbl[OS_EVENT_TBL_SIZE];  /* List of tasks waiting for event to occur                */
	    INT8U   OSEventGrp;                     /* Group corresponding to tasks waiting for event to occur */
	#else
	    INT16U  OSEventTbl[OS_EVENT_TBL_SIZE];  /* List of tasks waiting for event to occur                */
	    INT16U  OSEventGrp;                     /* Group corresponding to tasks waiting for event to occur */
	#endif
	    INT8U   OSValue;                        /* Mutex value (0 = used, 1 = available)                   */
	    INT8U   OSOwnerPrio;                    /* Mutex owner's task priority or 0xFF if no owner         */
	    INT8U   OSMutexPIP;                     /* Priority Inheritance Priority or 0xFF if no owner       */
	} OS_MUTEX_DATA;
	#endif
	
	/*
	*********************************************************************************************************
	*                                          MESSAGE QUEUE DATA
	*********************************************************************************************************
	*/
	
	#if OS_Q_EN > 0
	typedef struct os_q {                   /* QUEUE CONTROL BLOCK                                         */
	    struct os_q   *OSQPtr;              /* Link to next queue control block in list of free blocks     */
	    void         **OSQStart;            /* Pointer to start of queue data                              */
	    void         **OSQEnd;              /* Pointer to end   of queue data                              */
	    void         **OSQIn;               /* Pointer to where next message will be inserted  in   the Q  */
	    void         **OSQOut;              /* Pointer to where next message will be extracted from the Q  */
	    INT16U         OSQSize;             /* Size of queue (maximum number of entries)                   */
	    INT16U         OSQEntries;          /* Current number of entries in the queue                      */
	} OS_Q;
	
	
	typedef struct os_q_data {
	    void          *OSMsg;               /* Pointer to next message to be extracted from queue          */
	    INT16U         OSNMsgs;             /* Number of messages in message queue                         */
	    INT16U         OSQSize;             /* Size of message queue                                       */
	#if OS_LOWEST_PRIO <= 63
	    INT8U          OSEventTbl[OS_EVENT_TBL_SIZE];  /* List of tasks waiting for event to occur         */
	    INT8U          OSEventGrp;          /* Group corresponding to tasks waiting for event to occur     */
	#else
	    INT16U         OSEventTbl[OS_EVENT_TBL_SIZE];  /* List of tasks waiting for event to occur         */
	    INT16U         OSEventGrp;          /* Group corresponding to tasks waiting for event to occur     */
	#endif
	} OS_Q_DATA;
	#endif
	
	/*
	*********************************************************************************************************
	*                                           SEMAPHORE DATA
	*********************************************************************************************************
	*/
	
	#if OS_SEM_EN > 0
	typedef struct os_sem_data {
	    INT16U  OSCnt;                          /* Semaphore count                                         */
	#if OS_LOWEST_PRIO <= 63
	    INT8U   OSEventTbl[OS_EVENT_TBL_SIZE];  /* List of tasks waiting for event to occur                */
	    INT8U   OSEventGrp;                     /* Group corresponding to tasks waiting for event to occur */
	#else
	    INT16U  OSEventTbl[OS_EVENT_TBL_SIZE];  /* List of tasks waiting for event to occur                */
	    INT16U  OSEventGrp;                     /* Group corresponding to tasks waiting for event to occur */
	#endif
	} OS_SEM_DATA;
	#endif
	
	/*
	*********************************************************************************************************
	*                                            TASK STACK DATA
	*********************************************************************************************************
	*/
	
	#if OS_TASK_CREATE_EXT_EN > 0
	typedef struct os_stk_data {
	    INT32U  OSFree;                    /* Number of free bytes on the stack                            */
	    INT32U  OSUsed;                    /* Number of bytes used on the stack                            */
	} OS_STK_DATA;
	#endif
	
	/*
	*********************************************************************************************************
	*                                          TASK CONTROL BLOCK
	*********************************************************************************************************
	*/
	
	typedef struct os_tcb {
	    OS_STK          *OSTCBStkPtr;      /* Pointer to current top of stack                              */
	
	#if OS_TASK_CREATE_EXT_EN > 0
	    void            *OSTCBExtPtr;      /* Pointer to user definable data for TCB extension             */
	    OS_STK          *OSTCBStkBottom;   /* Pointer to bottom of stack                                   */
	    INT32U           OSTCBStkSize;     /* Size of task stack (in number of stack elements)             */
	    INT16U           OSTCBOpt;         /* Task options as passed by OSTaskCreateExt()                  */
	    INT16U           OSTCBId;          /* Task ID (0..65535)                                           */
	#endif
	
	    struct os_tcb   *OSTCBNext;        /* Pointer to next     TCB in the TCB list                      */
	    struct os_tcb   *OSTCBPrev;        /* Pointer to previous TCB in the TCB list                      */
	
	#if OS_EVENT_EN
	    OS_EVENT        *OSTCBEventPtr;    /* Pointer to event control block                               */
	#endif
	
	#if ((OS_Q_EN > 0) && (OS_MAX_QS > 0)) || (OS_MBOX_EN > 0)
	    void            *OSTCBMsg;         /* Message received from OSMboxPost() or OSQPost()              */
	#endif
	
	#if (OS_VERSION >= 251) && (OS_FLAG_EN > 0) && (OS_MAX_FLAGS > 0)
	#if OS_TASK_DEL_EN > 0
	    OS_FLAG_NODE    *OSTCBFlagNode;    /* Pointer to event flag node                                   */
	#endif
	    OS_FLAGS         OSTCBFlagsRdy;    /* Event flags that made task ready to run                      */
	#endif
	
	    INT16U           OSTCBDly;         /* Nbr ticks to delay task or, timeout waiting for event        */
	    INT8U            OSTCBStat;        /* Task status                                                  */
	    BOOLEAN          OSTCBPendTO;      /* Flag indicating PEND timed out (TRUE == timed out)           */
	    INT8U            OSTCBPrio;        /* Task priority (0 == highest)                                 */
	
	    INT8U            OSTCBX;           /* Bit position in group  corresponding to task priority        */
	    INT8U            OSTCBY;           /* Index into ready table corresponding to task priority        */
	#if OS_LOWEST_PRIO <= 63
	    INT8U            OSTCBBitX;        /* Bit mask to access bit position in ready table               */
	    INT8U            OSTCBBitY;        /* Bit mask to access bit position in ready group               */
	#else
	    INT16U           OSTCBBitX;        /* Bit mask to access bit position in ready table               */
	    INT16U           OSTCBBitY;        /* Bit mask to access bit position in ready group               */
	#endif
	
	#if OS_TASK_DEL_EN > 0
	    INT8U            OSTCBDelReq;      /* Indicates whether a task needs to delete itself              */
	#endif
	
	#if OS_TASK_PROFILE_EN > 0
	    INT32U           OSTCBCtxSwCtr;    /* Number of time the task was switched in                      */
	    INT32U           OSTCBCyclesTot;   /* Total number of clock cycles the task has been running       */
	    INT32U           OSTCBCyclesStart; /* Snapshot of cycle counter at start of task resumption        */
	    OS_STK          *OSTCBStkBase;     /* Pointer to the beginning of the task stack                   */
	    INT32U           OSTCBStkUsed;     /* Number of bytes used from the stack                          */
	#endif
	
	#if OS_TASK_NAME_SIZE > 1
    INT8U            OSTCBTaskName[OS_TASK_NAME_SIZE];
	#endif
	} OS_TCB;
	
	/*
	*********************************************************************************************************
	*                                            GLOBAL VARIABLES
	*********************************************************************************************************
	*/
	
	OS_EXT  INT32U            OSCtxSwCtr;               /* Counter of number of context switches           */
	
	#if OS_EVENT_EN && (OS_MAX_EVENTS > 0)
	OS_EXT  OS_EVENT         *OSEventFreeList;          /* Pointer to list of free EVENT control blocks    */
	OS_EXT  OS_EVENT          OSEventTbl[OS_MAX_EVENTS];/* Table of EVENT control blocks                   */
	#endif
	
	#if (OS_VERSION >= 251) && (OS_FLAG_EN > 0) && (OS_MAX_FLAGS > 0)
	OS_EXT  OS_FLAG_GRP       OSFlagTbl[OS_MAX_FLAGS];  /* Table containing event flag groups              */
	OS_EXT  OS_FLAG_GRP      *OSFlagFreeList;           /* Pointer to free list of event flag groups       */
	#endif
	
	#if OS_TASK_STAT_EN > 0
	OS_EXT  INT8S             OSCPUUsage;               /* Percentage of CPU used                          */
	OS_EXT  INT32U            OSIdleCtrMax;             /* Max. value that idle ctr can take in 1 sec.     */
	OS_EXT  INT32U            OSIdleCtrRun;             /* Val. reached by idle ctr at run time in 1 sec.  */
	OS_EXT  BOOLEAN           OSStatRdy;                /* Flag indicating that the statistic task is rdy  */
	OS_EXT  OS_STK            OSTaskStatStk[OS_TASK_STAT_STK_SIZE];      /* Statistics task stack          */
	#endif
	
	OS_EXT  INT8U             OSIntNesting;             /* Interrupt nesting level                         */
	
	OS_EXT  INT8U             OSLockNesting;            /* Multitasking lock nesting level                 */
	
	OS_EXT  INT8U             OSPrioCur;                /* Priority of current task                        */
	OS_EXT  INT8U             OSPrioHighRdy;            /* Priority of highest priority task               */
	
	#if OS_LOWEST_PRIO <= 63
	OS_EXT  INT8U             OSRdyGrp;                        /* Ready list group                         */
	OS_EXT  INT8U             OSRdyTbl[OS_RDY_TBL_SIZE];       /* Table of tasks which are ready to run    */
	#else
	OS_EXT  INT16U            OSRdyGrp;                        /* Ready list group                         */
	OS_EXT  INT16U            OSRdyTbl[OS_RDY_TBL_SIZE];       /* Table of tasks which are ready to run    */
	#endif
	
	OS_EXT  BOOLEAN           OSRunning;                       /* Flag indicating that kernel is running   */
	
	OS_EXT  INT8U             OSTaskCtr;                       /* Number of tasks created                  */
	
	OS_EXT  volatile  INT32U  OSIdleCtr;                                 /* Idle counter                   */
	
	OS_EXT  OS_STK            OSTaskIdleStk[OS_TASK_IDLE_STK_SIZE];      /* Idle task stack                */
	
	
	OS_EXT  OS_TCB           *OSTCBCur;                        /* Pointer to currently running TCB         */
	OS_EXT  OS_TCB           *OSTCBFreeList;                   /* Pointer to list of free TCBs             */
	OS_EXT  OS_TCB           *OSTCBHighRdy;                    /* Pointer to highest priority TCB R-to-R   */
	OS_EXT  OS_TCB           *OSTCBList;                       /* Pointer to doubly linked list of TCBs    */
	OS_EXT  OS_TCB           *OSTCBPrioTbl[OS_LOWEST_PRIO + 1];/* Table of pointers to created TCBs        */
	OS_EXT  OS_TCB            OSTCBTbl[OS_MAX_TASKS + OS_N_SYS_TASKS];   /* Table of TCBs                  */
	
	#if OS_TICK_STEP_EN > 0
	OS_EXT  INT8U             OSTickStepState;          /* Indicates the state of the tick step feature    */
	#endif
	
	#if (OS_MEM_EN > 0) && (OS_MAX_MEM_PART > 0)
	OS_EXT  OS_MEM           *OSMemFreeList;            /* Pointer to free list of memory partitions       */
	OS_EXT  OS_MEM            OSMemTbl[OS_MAX_MEM_PART];/* Storage for memory partition manager            */
	#endif
	
	#if (OS_Q_EN > 0) && (OS_MAX_QS > 0)
	OS_EXT  OS_Q             *OSQFreeList;              /* Pointer to list of free QUEUE control blocks    */
	OS_EXT  OS_Q              OSQTbl[OS_MAX_QS];        /* Table of QUEUE control blocks                   */
	#endif
	
	#if OS_TIME_GET_SET_EN > 0
	OS_EXT  volatile  INT32U  OSTime;                   /* Current value of system time (in ticks)         */
	#endif
	
	
	extern  INT8U   const     OSUnMapTbl[256];          /* Priority->Index    lookup table                 */
	
	/*
	*********************************************************************************************************
	*                                          FUNCTION PROTOTYPES
	*                                     (Target Independent Functions)
	*********************************************************************************************************
	*/
	
	/*
	*********************************************************************************************************
	*                                            MISCELLANEOUS
	*********************************************************************************************************
	*/
	
	#if OS_EVENT_EN && (OS_EVENT_NAME_SIZE > 1)
	INT8U         OSEventNameGet(OS_EVENT *pevent, INT8U *pname, INT8U *err);
	void          OSEventNameSet(OS_EVENT *pevent, INT8U *pname, INT8U *err);
	#endif
	
	/*
	*********************************************************************************************************
	*                                         EVENT FLAGS MANAGEMENT
	*********************************************************************************************************
	*/
	
	#if (OS_VERSION >= 251) && (OS_FLAG_EN > 0) && (OS_MAX_FLAGS > 0)
	
	#if OS_FLAG_ACCEPT_EN > 0
	OS_FLAGS      OSFlagAccept(OS_FLAG_GRP *pgrp, OS_FLAGS flags, INT8U wait_type, INT8U *err);
	#endif
	
	OS_FLAG_GRP  *OSFlagCreate(OS_FLAGS flags, INT8U *err);
	
	#if OS_FLAG_DEL_EN > 0
	OS_FLAG_GRP  *OSFlagDel(OS_FLAG_GRP *pgrp, INT8U opt, INT8U *err);
	#endif
	
	#if (OS_FLAG_EN > 0) && (OS_FLAG_NAME_SIZE > 1)
	INT8U         OSFlagNameGet(OS_FLAG_GRP *pgrp, INT8U *pname, INT8U *err);
	void          OSFlagNameSet(OS_FLAG_GRP *pgrp, INT8U *pname, INT8U *err);
	#endif
	
	OS_FLAGS      OSFlagPend(OS_FLAG_GRP *pgrp, OS_FLAGS flags, INT8U wait_type, INT16U timeout, INT8U *err);
	OS_FLAGS      OSFlagPendGetFlagsRdy(void);
	OS_FLAGS      OSFlagPost(OS_FLAG_GRP *pgrp, OS_FLAGS flags, INT8U opt, INT8U *err);
	
	#if OS_FLAG_QUERY_EN > 0
	OS_FLAGS      OSFlagQuery(OS_FLAG_GRP *pgrp, INT8U *err);
	#endif
	#endif
	
	/*
	*********************************************************************************************************
	*                                        MESSAGE MAILBOX MANAGEMENT
	*********************************************************************************************************
	*/
	
	#if OS_MBOX_EN > 0
	
	#if OS_MBOX_ACCEPT_EN > 0
	void         *OSMboxAccept(OS_EVENT *pevent);
	#endif
	
	OS_EVENT     *OSMboxCreate(void *msg);
	
	#if OS_MBOX_DEL_EN > 0
	OS_EVENT     *OSMboxDel(OS_EVENT *pevent, INT8U opt, INT8U *err);
	#endif
	
	void         *OSMboxPend(OS_EVENT *pevent, INT16U timeout, INT8U *err);
	
	#if OS_MBOX_POST_EN > 0
	INT8U         OSMboxPost(OS_EVENT *pevent, void *msg);
	#endif
	
	#if OS_MBOX_POST_OPT_EN > 0
	INT8U         OSMboxPostOpt(OS_EVENT *pevent, void *msg, INT8U opt);
	#endif
	
	#if OS_MBOX_QUERY_EN > 0
	INT8U         OSMboxQuery(OS_EVENT *pevent, OS_MBOX_DATA *p_mbox_data);
	#endif
	#endif
	
	/*
	*********************************************************************************************************
	*                                           MEMORY MANAGEMENT
	*********************************************************************************************************
	*/
	
	#if (OS_MEM_EN > 0) && (OS_MAX_MEM_PART > 0)
	
	OS_MEM       *OSMemCreate(void *addr, INT32U nblks, INT32U blksize, INT8U *err);
	void         *OSMemGet(OS_MEM *pmem, INT8U *err);
	#if OS_MEM_NAME_SIZE > 1
	INT8U         OSMemNameGet(OS_MEM *pmem, INT8U *pname, INT8U *err);
	void          OSMemNameSet(OS_MEM *pmem, INT8U *pname, INT8U *err);
	#endif
	INT8U         OSMemPut(OS_MEM *pmem, void *pblk);
	
	#if OS_MEM_QUERY_EN > 0
	INT8U         OSMemQuery(OS_MEM *pmem, OS_MEM_DATA *p_mem_data);
	#endif
	
	#endif
	
	/*
	*********************************************************************************************************
	*                                MUTUAL EXCLUSION SEMAPHORE MANAGEMENT
	*********************************************************************************************************
	*/
	
	#if OS_MUTEX_EN > 0
	
	#if OS_MUTEX_ACCEPT_EN > 0
	INT8U         OSMutexAccept(OS_EVENT *pevent, INT8U *err);
	#endif
	
	OS_EVENT     *OSMutexCreate(INT8U prio, INT8U *err);
	
	#if OS_MUTEX_DEL_EN > 0
	OS_EVENT     *OSMutexDel(OS_EVENT *pevent, INT8U opt, INT8U *err);
	#endif
	
	void          OSMutexPend(OS_EVENT *pevent, INT16U timeout, INT8U *err);
	INT8U         OSMutexPost(OS_EVENT *pevent);
	
	#if OS_MUTEX_QUERY_EN > 0
	INT8U         OSMutexQuery(OS_EVENT *pevent, OS_MUTEX_DATA *p_mutex_data);
	#endif
	
	#endif
	
	/*
	*********************************************************************************************************
	*                                         MESSAGE QUEUE MANAGEMENT
	*********************************************************************************************************
	*/
	
	#if (OS_Q_EN > 0) && (OS_MAX_QS > 0)
	
	#if OS_Q_ACCEPT_EN > 0
	void         *OSQAccept(OS_EVENT *pevent, INT8U *err);
	#endif
	
	OS_EVENT     *OSQCreate(void **start, INT16U size);
	
	#if OS_Q_DEL_EN > 0
	OS_EVENT     *OSQDel(OS_EVENT *pevent, INT8U opt, INT8U *err);
	#endif
	
	#if OS_Q_FLUSH_EN > 0
	INT8U         OSQFlush(OS_EVENT *pevent);
	#endif
	
	void         *OSQPend(OS_EVENT *pevent, INT16U timeout, INT8U *err);
	
	#if OS_Q_POST_EN > 0
	INT8U         OSQPost(OS_EVENT *pevent, void *msg);
	#endif
	
	#if OS_Q_POST_FRONT_EN > 0
	INT8U         OSQPostFront(OS_EVENT *pevent, void *msg);
	#endif
	
	#if OS_Q_POST_OPT_EN > 0
	INT8U         OSQPostOpt(OS_EVENT *pevent, void *msg, INT8U opt);
	#endif
	
	#if OS_Q_QUERY_EN > 0
	INT8U         OSQQuery(OS_EVENT *pevent, OS_Q_DATA *p_q_data);
	#endif
	
	#endif
	
	/*
	*********************************************************************************************************
	*                                          SEMAPHORE MANAGEMENT
	*********************************************************************************************************
	*/
	#if OS_SEM_EN > 0
	
	#if OS_SEM_ACCEPT_EN > 0
	INT16U        OSSemAccept(OS_EVENT *pevent);
	#endif
	
	OS_EVENT     *OSSemCreate(INT16U cnt);
	
	#if OS_SEM_DEL_EN > 0
	OS_EVENT     *OSSemDel(OS_EVENT *pevent, INT8U opt, INT8U *err);
	#endif
	
	void          OSSemPend(OS_EVENT *pevent, INT16U timeout, INT8U *err);
	INT8U         OSSemPost(OS_EVENT *pevent);
	
	#if OS_SEM_QUERY_EN > 0
	INT8U         OSSemQuery(OS_EVENT *pevent, OS_SEM_DATA *p_sem_data);
	#endif
	
	#if OS_SEM_SET_EN > 0
	void          OSSemSet(OS_EVENT *pevent, INT16U cnt, INT8U *err);
	#endif
	
	#endif
	
	/*
	*********************************************************************************************************
	*                                            TASK MANAGEMENT
	*********************************************************************************************************
	*/
	#if OS_TASK_CHANGE_PRIO_EN > 0
	INT8U         OSTaskChangePrio(INT8U oldprio, INT8U newprio);
	#endif
	
	#if OS_TASK_CREATE_EN > 0
	INT8U         OSTaskCreate(void (*task)(void *p_arg), void *p_arg, OS_STK *ptos, INT8U prio);
	#endif
	
	#if OS_TASK_CREATE_EXT_EN > 0
	INT8U         OSTaskCreateExt(void   (*task)(void *p_arg),
	                              void    *p_arg,
	                              OS_STK  *ptos,
	                              INT8U    prio,
	                              INT16U   id,
	                              OS_STK  *pbos,
	                              INT32U   stk_size,
	                              void    *pext,
	                              INT16U   opt);
	#endif
	
	#if OS_TASK_DEL_EN > 0
	INT8U         OSTaskDel(INT8U prio);
	INT8U         OSTaskDelReq(INT8U prio);
	#endif
	
	#if OS_TASK_NAME_SIZE > 1
	INT8U         OSTaskNameGet(INT8U prio, INT8U *pname, INT8U *err);
	void          OSTaskNameSet(INT8U prio, INT8U *pname, INT8U *err);
	#endif
	
	#if OS_TASK_SUSPEND_EN > 0
	INT8U         OSTaskResume(INT8U prio);
	INT8U         OSTaskSuspend(INT8U prio);
	#endif
	
	#if OS_TASK_CREATE_EXT_EN > 0
	INT8U         OSTaskStkChk(INT8U prio, OS_STK_DATA *p_stk_data);
	#endif
	
	#if OS_TASK_QUERY_EN > 0
	INT8U         OSTaskQuery(INT8U prio, OS_TCB *p_task_data);
	#endif
	
	/*
	*********************************************************************************************************
	*                                            TIME MANAGEMENT
	*********************************************************************************************************
	*/
	
	void          OSTimeDly(INT16U ticks);
	
	#if OS_TIME_DLY_HMSM_EN > 0
	INT8U         OSTimeDlyHMSM(INT8U hours, INT8U minutes, INT8U seconds, INT16U milli);
	#endif
	
	#if OS_TIME_DLY_RESUME_EN > 0
	INT8U         OSTimeDlyResume(INT8U prio);
	#endif
	
	#if OS_TIME_GET_SET_EN > 0
	INT32U        OSTimeGet(void);
	void          OSTimeSet(INT32U ticks);
	#endif
	
	void          OSTimeTick(void);
	
	/*
	*********************************************************************************************************
	*                                             MISCELLANEOUS
	*********************************************************************************************************
	*/
	
	void          OSInit(void);
	
	void          OSIntEnter(void);
	void          OSIntExit(void);
	
	#if OS_SCHED_LOCK_EN > 0
	void          OSSchedLock(void);
	void          OSSchedUnlock(void);
	#endif
	
	void          OSStart(void);
	
	void          OSStatInit(void);
	
	INT16U        OSVersion(void);
	
	/*
	*********************************************************************************************************
	*                                      INTERNAL FUNCTION PROTOTYPES
	*                            (Your application MUST NOT call these functions)
	*********************************************************************************************************
	*/
	
	#if OS_TASK_DEL_EN > 0
	void          OS_Dummy(void);
	#endif
	
	#if OS_EVENT_EN
	INT8U         OS_EventTaskRdy(OS_EVENT *pevent, void *msg, INT8U msk);
	void          OS_EventTaskWait(OS_EVENT *pevent);
	void          OS_EventTO(OS_EVENT *pevent);
	void          OS_EventWaitListInit(OS_EVENT *pevent);
	#endif
	
	#if (OS_VERSION >= 251) && (OS_FLAG_EN > 0) && (OS_MAX_FLAGS > 0)
	void          OS_FlagInit(void);
	void          OS_FlagUnlink(OS_FLAG_NODE *pnode);
	#endif
	
	void          OS_MemClr(INT8U *pdest, INT16U size);
	void          OS_MemCopy(INT8U *pdest, INT8U *psrc, INT16U size);
	
	#if (OS_MEM_EN > 0) && (OS_MAX_MEM_PART > 0)
	void          OS_MemInit(void);
	#endif
	
	#if OS_Q_EN > 0
	void          OS_QInit(void);
	#endif
	
	void          OS_Sched(void);
	
	#if (OS_EVENT_NAME_SIZE > 1) || (OS_FLAG_NAME_SIZE > 1) || (OS_MEM_NAME_SIZE > 1) || (OS_TASK_NAME_SIZE > 1)
	INT8U         OS_StrCopy(INT8U *pdest, INT8U *psrc);
	INT8U         OS_StrLen(INT8U *psrc);
	#endif
	
	void          OS_TaskIdle(void *p_arg);
	
	#if OS_TASK_STAT_EN > 0
	void          OS_TaskStat(void *p_arg);
	#endif
	
	#if OS_TASK_CREATE_EXT_EN > 0
	void          OS_TaskStkClr(OS_STK *pbos, INT32U size, INT16U opt);
	#endif
	
	#if (OS_TASK_STAT_STK_CHK_EN > 0) && (OS_TASK_CREATE_EXT_EN > 0)
	void          OS_TaskStatStkChk(void);
	#endif
	
	INT8U         OS_TCBInit(INT8U prio, OS_STK *ptos, OS_STK *pbos, INT16U id, INT32U stk_size, void *pext, INT16U opt);
	
	/*
	*********************************************************************************************************
	*                                          FUNCTION PROTOTYPES
	*                                      (Target Specific Functions)
	*********************************************************************************************************
	*/
	
	#if OS_VERSION >= 270 && OS_DEBUG_EN > 0
	void          OSDebugInit(void);
	#endif
	
	#if OS_VERSION >= 204
	void          OSInitHookBegin(void);
	void          OSInitHookEnd(void);
	#endif
	
	#ifndef OS_ISR_PROTO_EXT
	void          OSIntCtxSw(void);
	
	void          OSStartHighRdy(void);
	#endif
	
	void          OSTaskCreateHook(OS_TCB *ptcb);
	void          OSTaskDelHook(OS_TCB *ptcb);
	
	#if OS_VERSION >= 251
	void          OSTaskIdleHook(void);
	#endif
	
	void          OSTaskStatHook(void);
	OS_STK       *OSTaskStkInit(void (*task)(void *p_arg), void *p_arg, OS_STK *ptos, INT16U opt);
	
	#if OS_TASK_SW_HOOK_EN > 0
	void          OSTaskSwHook(void);
	#endif
	
	#if OS_VERSION >= 204
	void          OSTCBInitHook(OS_TCB *ptcb);
	#endif
	
	#if OS_TIME_TICK_HOOK_EN > 0
	void          OSTimeTickHook(void);
	#endif
	
	/*
	*********************************************************************************************************
	*                                          FUNCTION PROTOTYPES
	*                                  (Compiler Specific ISR prototypes)
	*********************************************************************************************************
	*/
	
	#ifndef OS_ISR_PROTO_EXT
	void          OSCtxSw(void);
	#endif


	typedef unsigned int   OS_CPU_SR;           // Define size of CPU status register (PSR = 32 bits)

	#define  OS_CRITICAL_METHOD    3
	
	#define  OS_ENTER_CRITICAL()  {cpu_sr = OS_CPU_SR_Save();}
	#define  OS_EXIT_CRITICAL()   {OS_CPU_SR_Restore(cpu_sr);}
	
	/********************************************************************************************************
	*                                         ARM Miscellaneous
	********************************************************************************************************/
	#define  OS_STK_GROWTH        1             // Stack grows from HIGH to LOW memory on ARM
	#define  OS_TASK_SW()         OSCtxSw()
	
	/********************************************************************************************************
	*                                              PROTOTYPES
	********************************************************************************************************/
	extern OS_CPU_SR  OS_CPU_SR_Save(void);
	extern void       OS_CPU_SR_Restore(OS_CPU_SR cpu_sr);
	
	extern void       OS_CPU_IRQ_ISR(void);
	extern void       OS_CPU_FIQ_ISR(void);
	
	extern void       OSCtxSw(void);
	extern void       OSIntCtxSw(void);
	extern void       OSStartHighRdy(void);
	
//#elif _OPERATING_SYSTEM == _OS_NUCLEUS

//#endif


// message
#define MESSAGE_SIZE	200

typedef struct stMsg
{
	INT32U Type;
	union
	{
		INT32U Param1;	//parameter 1
		INT32U XValue;	//coor x
		INT32U KeyValue;	//key value
	}Param1;
	union
	{
		INT32U Param2;	//parameter 2
		INT32U YValue;	//coor y
	}Param2;
	INT32U Reserved;		//reserve
} STMsg;


#define GetMsgType(pmsg)		(pmsg->Type)
#define GetMsgParam1(pmsg)		(pmsg->Param1.Param1)
#define GetMsgParam2(pmsg)		(pmsg->Param2.Param2)
#define GetMsgKeyValue(pmsg)	(pmsg->Param1.KeyValue)
#define GetMsgXValue(pmsg)		(pmsg->Param1.XValue)
#define GetMsgYValue(pmsg)		(pmsg->Param2.YValue)

//extern OS_EVENT* hDispatchQ;
extern OS_EVENT* hApMessageQ;

STMsg* GetMSG(OS_EVENT* hevent);
STMsg* GetMsgMainQ( void );
STMsg* MallocMSG( void );
void MsgInit( void );
void FlushMsg(OS_EVENT* hevent);
void FreeMSG(STMsg* pmsg);
//INT32U SysQDelByType(OS_EVENT *hevent,INT32U type);
INT32U SendFrontMSG(OS_EVENT* hevent,INT32U type,INT32U param1);
INT32U SendMSG(OS_EVENT* hevent,INT32U type,INT32U param1);
INT32U SendTpMSG(OS_EVENT* hevent,INT32U type,INT32U param1,INT32U param2,INT32U tptype);
INT32U SendTpFrontMSG(OS_EVENT* hevent,INT32U type,INT32U param1,INT32U param2,INT32U tptype);
INT32U TransmitMSG(OS_EVENT*,STMsg* pmsg);
//OS_EVENT* GethMainQValue(void);
/*
*****************************************************************************
*			define msg return value
*****************************************************************************
*/

#define MSG_NO_ERR 			0			/*     no error      						*/
#define MSG_NOMEM_ERR		1			/*    can't allocate memory				*/
#define MSG_SEND_ERR		2			/*    send error, messageq full or other error	*/
#define MSG_PARAM_ERR		3			/*Parameter not valid*/
/*
*****************************************************************************
*			define msg type
*****************************************************************************
*/
#define	MSG_INVALID					0x0000	// 如果gMsgContent中STMsg表项的Type == 0x0000，表示该表项未用
#define	MSG_MALLOCED				0xffff
#define MSG_INT_LVD1				0x0001	
#define MSG_INT_LVD2				0x0002	
#define MSG_INT_LVD3				0x0003	
#define MSG_INT_LVD4				0x0004	
#define MSG_INT_LVD5				0x0005	
#define MSG_INT_LVD6				0x0006	
#define MSG_INT_LVD7				0x0007	
#define MSG_INT_LVD8				0x0008	
#define MSG_INT_YEAR				0x000A	
#define MSG_INT_MONTH				0x000B
#define MSG_INT_DAY					0x000C
#define MSG_INT_HOUR				0x000D
#define MSG_INT_MINUTE				0x000E
#define MSG_INT_SECOND				0x000F
#define MSG_INT_HALFSEC				0x0010
#define MSG_INT_REMINDER1	        0x0018        
#define MSG_INT_REMINDER2	        0x0019
#define MSG_INT_REMINDER3	        0x001A
#define MSG_INT_REMINDER4	        0x001B
#define MSG_INT_EXTPIN1  	        0x0020
#define MSG_INT_EXTPIN2  	        0x0021
#define MSG_INT_EXTPIN3  	        0x0022
#define MSG_INT_EXTPIN4  	        0x0023
#define	MSG_INT_SHUTDOWN			0x0024
#define MSG_INT_ONOFFKEY	       	0x0030
#define MSG_INT_INTKEY	            0x0031
#define MSG_INT_EXTKEY	            0x0032
#define MSG_INT_EXTKEY_UP    	   	0x0050
#define	MSG_INT_EXTKEY_DOWN			0x0051
#define MSG_COUNTDOWNTIME			0x0033  
#define MSG_CHANGESCHEDULE			0x0034
#define MSG_ANNI_CH					0x0035	//open by Mona on Aug.1,2004 for Remind.
#define MSG_CHANGEDAILY		        0x0036  
#define MSG_CHANGECLOCK		        0x0037 
#define MSG_CHANGESYSTIME			0x0038
                                      
//2004-09-13 cfwei                    
#define MSG_SND_BEGIN				0x0040
#define MSG_SND_END					0x0041
#define MSG_SND_STOP				0x0042
#define MSG_SND_RECBEGIN			0x0043
#define MSG_SND_RECEND				0x0044
#define MSG_SND_RECSTOP				0x0044
#define MSG_SND_RECFULL				0x0045
#define MSG_SND_ERROR				0x0047
#define MSG_INT_TP					0x0046//pengtj 2005/03/11
#define MSG_SND_CHANGE				0x0048

#define	MSG_USB_CONNECT				0x0080                                     

#define MSG_AP_RESTART				0x0100
#define MSG_AP_SWAPIN				0x0101
#define MSG_AP_SWAPOUT				0x0102
#define MSG_AP_RQEXIT				0x0103
#define MSG_AP_ACKEXIT				0x0104
#define MSG_AP_SUICIDE				0x0105
#define MSG_AP_REDRAW				0x0106
#define MSG_KEYHELP					0x0107//zhouli 2004/09/16


#define	MSG_OFF2ON					0x200			
#define	MSG_ON2OFF					0x201
#define	MSG_ON2OFF_ACK				0x202

//usb detect status message,add by mona on May.5,2006.
#define MSG_UDISK_INSERT			0x300
#define MSG_UDISK_OUT				0x301

//added by wangmeng.2006/05/23
#define MSG_INFO_DISP				0x310

/*
#define MSG_USER_TIMER0				0xffa0
#define MSG_USER_TIMER1				0xffa1
#define MSG_USER_TIMER2				0xffa2
#define MSG_USER_TIMER3				0xffa3
#define MSG_USER_TIMER4				0xffa4
*/

#define	TP_DOWN						0x0001		//TouchPanel Reserved Msg
#define	TP_MOVE						0x0002		//shuaiqun add
#define	TP_UP						0x0004		//

#define	KEY_TYPE_UP					'U'//38		// UP
#define	KEY_TYPE_DOWN				'D'//64		// DOWN
#define	KEY_TYPE_LEFT				'L'//56		// LEFT
#define	KEY_TYPE_RIGHT				'R'//46		// RIGHT
#define	KEY_TYPE_OK					'E'//48		// ENTER
#define	KEY_TYPE_ESC				'Q'//58		// QUIT

#define MSG_DRV_MOUSE			(0x0310)

#define MSG_DRV_MUS_LB_SCLK		(MSG_DRV_MOUSE+1)
#define MSG_DRV_MUS_LB_DCLK		(MSG_DRV_MOUSE+2)
#define MSG_DRV_MUS_LB_DOWN		(MSG_DRV_MOUSE+3)
#define MSG_DRV_MUS_LB_UP		(MSG_DRV_MOUSE+4)

#define MSG_DRV_MUS_RB_SCLK		(MSG_DRV_MOUSE+5)
#define MSG_DRV_MUS_RB_DCLK		(MSG_DRV_MOUSE+6)
#define MSG_DRV_MUS_RB_DOWN		(MSG_DRV_MOUSE+7)
#define MSG_DRV_MUS_RB_UP		(MSG_DRV_MOUSE+8)

#define	MSG_DRV_MUS_MOVE		(MSG_DRV_MOUSE+9)

#define	MSG_DRV_MUS_ROLLER_UP	(MSG_DRV_MOUSE+10)
#define	MSG_DRV_MUS_ROLLER_DOWN	(MSG_DRV_MOUSE+11)

#define MSG_PHOTO_DECODE_END		(0x400)
#define MSG_ALL_PHOTO_DECODE_END	(0x401)
#define MSG_PHOTO_DECODE			(0x402)
#define MSG_AVI_TIMER				(0x403)

// IR KEY VALUE DEFINE
#define MSG_IR_KEY					(0x500)

#define MSG_IR_POWER				(MSG_IR_KEY + 0x10)
#define MSG_IR_TV_MODE				(MSG_IR_KEY + 0x03)
#define MSG_IR_AV					(MSG_IR_KEY + 0x01)
#define MSG_IR_MUTE					(MSG_IR_KEY + 0x06)

#define MSG_IR_DISPLAY				(MSG_IR_KEY + 0x09)
#define MSG_IR_ROTATE				(MSG_IR_KEY + 0x1d)
#define MSG_IR_SLIDE_SHOW			(MSG_IR_KEY + 0x1f)
#define MSG_IR_PHOTO				(MSG_IR_KEY + 0x0d)

#define MSG_IR_SLIDE_INTERVAL		(MSG_IR_KEY + 0x19)
#define MSG_IR_ZOOM					(MSG_IR_KEY + 0x1b)
#define MSG_IR_AUDIO				(MSG_IR_KEY + 0x11)
#define MSG_IR_MUSIC				(MSG_IR_KEY + 0x15)

#define MSG_IR_SETUP				(MSG_IR_KEY + 0x17)
#define MSG_IR_UP					(MSG_IR_KEY + 0x12)
#define MSG_IR_CARD_SELECTION		(MSG_IR_KEY + 0x16)
#define MSG_IR_MOVIE				(MSG_IR_KEY + 0x4c)

#define MSG_IR_LEFT					(MSG_IR_KEY + 0x40)
#define MSG_IR_ENTER				(MSG_IR_KEY + 0x48)
#define MSG_IR_RIGHT				(MSG_IR_KEY + 0x04)
#define MSG_IR_FILE					(MSG_IR_KEY + 0x00)

#define MSG_IR_PRINT				(MSG_IR_KEY + 0x02)
#define MSG_IR_DOWN					(MSG_IR_KEY + 0x05)
#define MSG_IR_VOL_INC				(MSG_IR_KEY + 0x54)
#define MSG_IR_VOL_DEC				(MSG_IR_KEY + 0x4d)

#define MSG_IR_PLAY					(MSG_IR_KEY + 0x0a)
#define MSG_IR_PAUSE				(MSG_IR_KEY + 0x1e)
#define MSG_IR_STOP					(MSG_IR_KEY + 0x0e)
#define MSG_IR_RETURN				(MSG_IR_KEY + 0x1a)

#define MSG_IR_SPEED_DOWN			(MSG_IR_KEY + 0x1c)
#define MSG_IR_SPEED_UP				(MSG_IR_KEY + 0x14)
#define MSG_IR_BACKWARD				(MSG_IR_KEY + 0x0f)
#define MSG_IR_FORWARD				(MSG_IR_KEY + 0x0c)

// OS init function
extern void os_init(void);


#endif		// __OS_H__
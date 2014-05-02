/*  ----------------------------------- Sample Headers              */
#include <multDSP_config.h>

#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */

#if !defined (DSP_BOOTMODE_NOBOOT)
/* Extern declaration for msgq parameters for the array of pools */
ZCPYMQT_Params mqtParams = {SAMPLE_POOL_ID};
#endif

/* Definitions required for the sample Message queue.
 * Using a Zero-copy based transport on the shared memory physical link. */
#if defined MSGQ_ZCPY_LINK
#define SAMPLEMQT_FXNS          ZCPYMQT_FXNS
#define SAMPLEMQT_init          ZCPYMQT_init
#define SAMPLEMQT_CTRLMSG_SIZE  ZCPYMQT_CTRLMSG_SIZE

#define SAMPLEPOOL_PARAMS       &SamplePoolParams
#define SAMPLEPOOL_FXNS         SMAPOOL_FXNS
#define SAMPLEPOOL_init         SMAPOOL_init

SMAPOOL_Params SamplePoolParams =
{
    0, /* Pool ID */
    TRUE
};
#endif /* if defined MSGQ_ZCPY_LINK */

/* Array of local message queues */
static MSGQ_Obj msgQueues[NUM_MSG_QUEUES];

/* Array of transports. */
MSGQ_TransportObj transports[MAX_PROCESSORS] =
{
#if (MAX_PROCESSORS == 2 ) /* In case of one DSP and one GPP processor */
     MSGQ_NOTRANSPORT,    /* Represents the local processor */
#endif
     {
        &SAMPLEMQT_init,  /* Init Function                 */
        &SAMPLEMQT_FXNS,  /* Transport interface functions */
        &mqtParams,       /* Transport params              */
        NULL,             /* Filled in by transport        */
        ID_GPP            /* Processor Id                  */
     }
};

/* MSGQ configuration information.
 * MSGQ_config is a required global variable. */
MSGQ_Config MSGQ_config =
{
    msgQueues,
    transports,
    NUM_MSG_QUEUES,
    MAX_PROCESSORS,
    0,
    MSGQ_INVALIDMSGQ,
    POOL_INVALIDID
};

/* Array of pools. */
static POOL_Obj pools[NUM_POOLS] =
{
    {
        &SAMPLEPOOL_init,               /* Init Function                      */
        (POOL_Fxns *) &SAMPLEPOOL_FXNS, /* Pool interface functions           */
        SAMPLEPOOL_PARAMS,              /* Pool params                        */
        NULL                            /* Pool object: Set within pool impl. */
    }
};

/* POOL configuration information.
 * POOL_config is a required global variable. */
POOL_Config POOL_config = {pools, NUM_POOLS};


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

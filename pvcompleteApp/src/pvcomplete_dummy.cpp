#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>
#include <time.h>
#include <sstream>

#include "epicsStdlib.h"
#include "epicsString.h"
#include "dbDefs.h"
#include "epicsMutex.h"
#include "dbBase.h"
#include "dbStaticLib.h"
#include "dbFldTypes.h"
#include "dbCommon.h"
#include "dbAccessDefs.h"
#include <epicsTypes.h>
#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsString.h>
#include <epicsTimer.h>
#include <epicsMutex.h>
#include <iocsh.h>
#include "macLib.h"
#include "epicsExit.h"
#include "initHooks.h"

#include "utilities.h"

#include <epicsExport.h>

static int pvcomplete(int disable)
{
    return 0;
}
		
// EPICS iocsh shell commands 

static const iocshArg initArg0 = { "disable", iocshArgInt };

static const iocshArg * const initArgs[] = { &initArg0 };

static const iocshFuncDef initFuncDef = {"pvcomplete", sizeof(initArgs) / sizeof(iocshArg*), initArgs};

static void initCallFunc(const iocshArgBuf *args)
{
    pvcomplete(args[0].ival);
}

static void myHookFunction(initHookState state)
{
    switch(state) 
    {
        case initHookAfterIocRunning:
            pvcomplete(0);
            break;

        default:
            break;
    }
}

extern "C" 
{

static void pvcompleteRegister(void)
{
    iocshRegister(&initFuncDef, initCallFunc);
    initHookRegister(myHookFunction);
}

epicsExportRegistrar(pvcompleteRegister);

}


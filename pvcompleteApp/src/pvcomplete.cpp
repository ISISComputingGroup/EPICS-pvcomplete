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

#include "utilities.h"

#ifdef _WIN32
#include <process.h>
#include <windows.h>
#include <editline/readline.h>
#else
#include <unistd.h>
#include <readline/readline.h>
#endif /* _WIN32 */

#include <epicsExport.h>

static std::vector<std::string> the_pvs;

// based on iocsh dbl command from epics_base/src/db/dbTest.c 
// return an std map, currently key is pv and value is recordType (if that is defined)
static void dump_pvs(const char *precordTypename, const char *fields, std::vector<std::string>& pvs)
{
    DBENTRY dbentry;
    DBENTRY *pdbentry=&dbentry;
    long status, status2;
    int nfields = 0;
    int ifield;
    char *fieldnames = 0;
    char **papfields = 0;

    pvs.clear();
    if (!pdbbase) {
        return;
        //throw std::runtime_error("No database loaded\n");
    }
    if (precordTypename &&
        ((*precordTypename == '\0') || !strcmp(precordTypename,"*")))
        precordTypename = NULL;
    if (fields && (*fields == '\0'))
        fields = NULL;
    if (fields) {
        char *pnext;

        fieldnames = epicsStrDup(fields);
        nfields = 1;
        pnext = fieldnames;
        while (*pnext && (pnext = strchr(pnext,' '))) {
            nfields++;
            while (*pnext == ' ') pnext++;
        }
        papfields = static_cast<char**>(dbCalloc(nfields,sizeof(char *)));
        pnext = fieldnames;
        for (ifield = 0; ifield < nfields; ifield++) {
            papfields[ifield] = pnext;
            if (ifield < nfields - 1) {
                pnext = strchr(pnext, ' ');
                *pnext++ = 0;
                while (*pnext == ' ') pnext++;
            }
        }
    }
    dbInitEntry(pdbbase, pdbentry);
    if (!precordTypename)
        status = dbFirstRecordType(pdbentry);
    else
        status = dbFindRecordType(pdbentry,precordTypename);
    if (status) {
        //printf("No record type\n");
    }
    while (!status) {
        status = dbFirstRecord(pdbentry);
        while (!status) {
			pvs.push_back(dbGetRecordName(pdbentry));
            status = dbNextRecord(pdbentry);
        }
        if (precordTypename) break;
        status = dbNextRecordType(pdbentry);
    }
    if (nfields > 0) {
        free((void *)papfields);
        free((void *)fieldnames);
    }
    dbFinishEntry(pdbentry);
}

static char* pv_complete_generator(const char* text, int state)
{
    static int list_index = 0, len = 0;
    const char *name;

    /* If this is a new word to complete, initialize now.  This includes
       saving the length of TEXT for efficiency, and initializing the index
       variable to 0. */
    if (!state)
    {
        list_index = 0;
        len = strlen (text);
    }

    /* Return the next name which partially matches from the command list. */
	while(list_index < the_pvs.size())
	{
	    name = the_pvs[list_index].c_str();
		++list_index;
        if (epicsStrnCaseCmp (name, text, len) == 0)
		{
            return strcpy((char*)malloc(1+strlen(name)), name);		/* the memory is free'd by readline */
		}
	}
    /* If no names matched, then return NULL. */
    return ((char *)NULL);
}

static char** pv_completion_function(const char *text, int start, int end)
{
    char** matches = 0;
    // if (start == 0)  the word to complete is at the start of the line
    matches = rl_completion_matches(text, pv_complete_generator);
    return matches;
}

static int pvcomplete(int disable)
{
    if (disable)
	{
		printf("pvcomplete: disabling pv command line completion\n");
	    rl_attempted_completion_function = NULL;
	}
	else
	{
		dump_pvs(NULL, NULL, the_pvs);
		printf("pvcomplete: loaded %d pvs for command line completion\n", the_pvs.size());
	    rl_attempted_completion_function = pv_completion_function;
	}
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

extern "C" 
{

static void pvcompleteRegister(void)
{
    iocshRegister(&initFuncDef, initCallFunc);
}

epicsExportRegistrar(pvcompleteRegister);

}


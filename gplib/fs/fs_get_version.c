#include "fsystem.h"
#include "globals.h"

static const char *filesystem_ver="GP$0100";		// v0.1.00

const char *fs_get_version(void)
{
	return 	filesystem_ver;
}


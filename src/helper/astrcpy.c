#include "astrcpy.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

char * Astrcpy(char const *pszSrc)
{
	return Astrlcpy(pszSrc, (int)strlen(pszSrc));
}

char * Astrlcpy(char const *pszSrc, int cbSrc)
{
	char *psz = NULL;
	if (cbSrc > -1) {
		psz = calloc(cbSrc + 1, 1);
		if (psz) {
			memcpy(psz, pszSrc, cbSrc);
		}
	}
	return psz;
}

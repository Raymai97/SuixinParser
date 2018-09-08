
static char * Strlcpy(char *pszDst, char const *pszSrc, int cbDstMax)
{
	char *p = pszDst;
	char const *q = pszSrc;
	int i = 0;
	for (; (i < cbDstMax) && (p[i] = q[i], q[i]); ++i);
	return pszDst;
}

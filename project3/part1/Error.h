#ifndef _ERROR_H_
#define _ERROR_H_
#include <stdio.h>

void _setError(const char *contextMessage, const char *filename, const char *function, int lineNumber);

#define errorWithContext(context)   _setError((context), __FILE__, __FUNCTION__, __LINE__)
#define errorNoContext()   errorWithContext("An error occurred")

#endif

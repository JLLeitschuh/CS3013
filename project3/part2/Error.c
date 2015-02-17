#include "Error.h"

void _setError(const char *contextMessage, const char *filename, const char *function, int lineNumber){
  printf("ERROR: %s File: %s Function: %s Line: %i\n\r", contextMessage, filename, function, lineNumber);
}

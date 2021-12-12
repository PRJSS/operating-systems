#include <stddef.h>
#define _SUCCESS 0
#define _WRONG_PARAMS -1
#define _MEMORY_LACK -2
#define _UNKNOWN_ERR 1
#define _SEGMENT_ACCESS_VIOLATION -2

typedef char* VA;
int _malloc(VA* ptr, size_t szBlock);

int _free(VA ptr);

int _read(VA ptr, void* pBuffer, size_t szBuffer);

int _write(VA ptr, void* pBuffer, size_t szBuffer);

int _init(int n, int szPage);

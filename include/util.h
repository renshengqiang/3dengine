#ifndef UTIL_H
#define	UTIL_H

#ifndef WIN32
#include <unistd.h>
#endif

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

#ifdef WIN32
#define SNPRINTF _snprintf_s
#else
#define SNPRINTF snprintf
#endif
#define INVALID_OBJECT_VALUE 0xffffffff
#define INVALID_MATERIAL 0xffffffff
#define INVALID_HANDLER 0xffffffff


#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }

#endif	/* UTIL_H */


#ifndef _Export_H
#define _Export_H

/*
 * The following is only used when building DLLs under WIN32...
 */

#define ENGINE_DLL 1

#  if defined(ENGINE_DLL) && (defined(_MSC_VER) || defined(__MWERKS__) || defined(__BORLANDC__))
#    ifdef ENGINE_LIBRARY
#      define ENGINE_EXPORT	__declspec(dllexport)
#    else
#      define ENGINE_EXPORT	__declspec(dllimport)
#    endif /* ENGINE_LIBRARY */
#  else
#    define ENGINE_EXPORT
#  endif /* ENGINE_DLL */

#endif /* !Export_H */


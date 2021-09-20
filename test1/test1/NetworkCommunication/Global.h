#ifndef __NETWORKCOMMUNICATION_GLOBAL_H__
#define __NETWORKCOMMUNICATION_GLOBAL_H__

#ifdef NETWORKCOMMUNICATION_LIB_EXPORT_API
# define  __declspec(dllexport)
#else
# define  __declspec(dllimport)
#endif


#endif // __NETWORKCOMMUNICATION_GLOBAL_H__
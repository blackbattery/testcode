#ifndef _STDAFX_H_
#define _STDAFX_H_

#ifndef _WIN32_WINNT        // 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0501    // 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

//#define _BIND_TO_CURRENT_VCLIBS_VERSION 1

#pragma warning(disable:4996 4355 4251 4275)

// warning C4251 dll导出类中使用了模板，这里屏蔽了这个warning，但是要注意的是，调用端要与库使用相同版本的模板


#include <stdio.h>
//#include <tchar.h>
#include <assert.h>

//#include <iostream>
#include <string>

//#include <bitset>
//#include <set>
#include <vector>
#include <map>
#include <list>
//#include <memory>
//#include <set>
//#include <queue>

#include <mutex>
//#include <time.h>
//#include <sstream>
//#include <random>
//#include <io.h>
//#include <atomic>
//#include <future>
//#include <algorithm>
//#include <condition_variable>
#include <thread>

using namespace std;

#if (defined(_WIN32))
#   ifndef _WIN32_WINNT                // 允许使用特定于 Windows XP 或更高版本的功能。
#       define _WIN32_WINNT 0x0501    // 将此值更改为相应的值，以适用于 Windows 的其他版本。
#   endif
#   include <Winsock2.h>            // 解决Windows.h与HPR头文件冲突，包含了HPR_Config.h头以后必须在这里包含此头文件
#   include <Windows.h>             // 用到HCNetSDk的地方需要用到此头文件
#endif 

// HClientWND的跨平台定义

#if defined(_WIN32)
typedef HWND HClientWND;
#elif defined(__linux__)
typedef unsigned int HClientWND;
#   define __stdcall
#elif defined(__APPLE__)
typedef  void * HClientWND;
#   define __stdcall
#else
#   error "未知的平台，HClientWND无法定义"
#endif

#include "Base/Errors.h"
#include "interface/ErrorCode.h"

namespace NetworkCommunication
{

}

#endif // _STDAFX_H_
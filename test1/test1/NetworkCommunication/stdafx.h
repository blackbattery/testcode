#ifndef _STDAFX_H_
#define _STDAFX_H_

#ifndef _WIN32_WINNT        // ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define _WIN32_WINNT 0x0501    // ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif

//#define _BIND_TO_CURRENT_VCLIBS_VERSION 1

#pragma warning(disable:4996 4355 4251 4275)

// warning C4251 dll��������ʹ����ģ�壬�������������warning������Ҫע����ǣ����ö�Ҫ���ʹ����ͬ�汾��ģ��


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
#   ifndef _WIN32_WINNT                // ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#       define _WIN32_WINNT 0x0501    // ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#   endif
#   include <Winsock2.h>            // ���Windows.h��HPRͷ�ļ���ͻ��������HPR_Config.hͷ�Ժ���������������ͷ�ļ�
#   include <Windows.h>             // �õ�HCNetSDk�ĵط���Ҫ�õ���ͷ�ļ�
#endif 

// HClientWND�Ŀ�ƽ̨����

#if defined(_WIN32)
typedef HWND HClientWND;
#elif defined(__linux__)
typedef unsigned int HClientWND;
#   define __stdcall
#elif defined(__APPLE__)
typedef  void * HClientWND;
#   define __stdcall
#else
#   error "δ֪��ƽ̨��HClientWND�޷�����"
#endif

#include "Base/Errors.h"
#include "interface/ErrorCode.h"

namespace NetworkCommunication
{

}

#endif // _STDAFX_H_
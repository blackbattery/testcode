#ifndef __NETWORKCOMMUNICATION_NETUTILSMANAGER_H__
#define __NETWORKCOMMUNICATION_NETUTILSMANAGER_H__

#include "interface/BasicTypes.h"
#include "Base/Pattern/Singleton.h"
#include <map>

namespace NetworkCommunication
{
	using namespace clientframework;

    class CNetiUtilsManager : public Base::CSingleton<CNetiUtilsManager>
    {
        friend class Base::CSingleton <CNetiUtilsManager>;
        CNetiUtilsManager();

    public:
        ~CNetiUtilsManager();

       bool InitNetUtils();
       int GetConnectNVRHandles(ID device_id);

    private:
        bool b_net_utils_init_;
        std::map <ID, int> connect_handles_;
    };
}

#endif // __NETWORKCOMMUNICATION_NETUTILSMANAGER_H__
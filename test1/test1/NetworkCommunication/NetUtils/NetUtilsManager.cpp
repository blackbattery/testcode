#include "stdafx.h"
#include "NetUtilsManager.h"
#include "HCNetSDK/HCNetUtils.h"

namespace NetworkCommunication
{
    CNetiUtilsManager::CNetiUtilsManager()
        : b_net_utils_init_(false)
    {

    }

    CNetiUtilsManager::~CNetiUtilsManager()
    {
        if (b_net_utils_init_)
        {
            NET_UTILS_Fini();
            connect_handles_.clear();
            b_net_utils_init_ = false;
        }
    }

    bool CNetiUtilsManager::InitNetUtils()
    {
        if (!b_net_utils_init_)
        {
            b_net_utils_init_ = NET_UTILS_Init();
        }

        return b_net_utils_init_;
    }

    int CNetiUtilsManager::GetConnectNVRHandles(ID device_id)
    {
        if (device_id == kInvalidID)
        {
            return kInvalidHandle;
        }

        if (connect_handles_.find(device_id) != connect_handles_.end())
        {
            return connect_handles_[device_id];
        }

        int connect_nvr_handle = static_cast<int>(NET_HTTPCLIENT_Create());
        
        connect_handles_[device_id] = connect_nvr_handle;

        return connect_nvr_handle;
    }
}
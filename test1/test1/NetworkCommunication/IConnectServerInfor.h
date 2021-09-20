/** @file IConnectServerInfor.h
 *  @note HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
 *  @brief  连接服务器需要的信息，虚接口
 *
 *  @author       zhanglili
 *  @date        2016/10/20
*/
#ifndef __NETWORKCOMMUNICATION_ICONNECTSERVERINFOR_H__
#define __NETWORKCOMMUNICATION_ICONNECTSERVERINFOR_H__

#include "interface/BasicTypes.h"

namespace NetworkCommunication
{
    using namespace std;

    //class IConnectServerInfor;

    /** @class IPreviewCamera
     *  @brief 用于网络通信，这些提供虚接口，需要库调用者实例化
     *
     *  职责：网络通信需要的信息
     *  
     * 
     */
    class  IConnectServerInfor
    {
    public:
        IConnectServerInfor(){};

        virtual ~IConnectServerInfor() {};

    public: 
        // 获取服务器的IP地址
        virtual std::string GetIPAddress() const = 0;

        // 获取服务器的端口号
        virtual unsigned short GetPort() const = 0;

    public:
        //获取session
        virtual string GetSession() const = 0;

        //获取Express的session
        virtual bool CreateTokenForExpress(string& token, const string& session_id, const string& token_version, const string& key) const = 0;

        // 释放无效session对应的token 
        // error_module  错误模块
        // error_code   错误值
        // session_id   会话id
        virtual bool SercurityRelease(const int error_module, const int error_code, const string& session_id) const = 0;

		//释放无效session对应的token,一体机的
		virtual bool ReleaseTokenFroExpress(const int error_module, const int error_code, const string& session_id) const = 0;

        // 获取token
        //通过VMS交互，建立socket通信，需要进行token安全认证
        // str_token    token数据
        // session_id   会话id
        virtual bool GetToken(const string& session_id, string& str_token) const = 0;

		//获取SHA
		virtual void GetSHARule(string& challenge, int& round_num) const = 0;

		virtual void GetEcryptionPwd(string& str_ecryption_pwd) const = 0;
    };
}

#endif // __NETWORKCOMMUNICATION_ICONNECTSERVERINFOR_H__
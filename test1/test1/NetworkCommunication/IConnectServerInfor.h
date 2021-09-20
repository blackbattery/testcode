/** @file IConnectServerInfor.h
 *  @note HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
 *  @brief  ���ӷ�������Ҫ����Ϣ����ӿ�
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
     *  @brief ��������ͨ�ţ���Щ�ṩ��ӿڣ���Ҫ�������ʵ����
     *
     *  ְ������ͨ����Ҫ����Ϣ
     *  
     * 
     */
    class  IConnectServerInfor
    {
    public:
        IConnectServerInfor(){};

        virtual ~IConnectServerInfor() {};

    public: 
        // ��ȡ��������IP��ַ
        virtual std::string GetIPAddress() const = 0;

        // ��ȡ�������Ķ˿ں�
        virtual unsigned short GetPort() const = 0;

    public:
        //��ȡsession
        virtual string GetSession() const = 0;

        //��ȡExpress��session
        virtual bool CreateTokenForExpress(string& token, const string& session_id, const string& token_version, const string& key) const = 0;

        // �ͷ���Чsession��Ӧ��token 
        // error_module  ����ģ��
        // error_code   ����ֵ
        // session_id   �Ựid
        virtual bool SercurityRelease(const int error_module, const int error_code, const string& session_id) const = 0;

		//�ͷ���Чsession��Ӧ��token,һ�����
		virtual bool ReleaseTokenFroExpress(const int error_module, const int error_code, const string& session_id) const = 0;

        // ��ȡtoken
        //ͨ��VMS����������socketͨ�ţ���Ҫ����token��ȫ��֤
        // str_token    token����
        // session_id   �Ựid
        virtual bool GetToken(const string& session_id, string& str_token) const = 0;

		//��ȡSHA
		virtual void GetSHARule(string& challenge, int& round_num) const = 0;

		virtual void GetEcryptionPwd(string& str_ecryption_pwd) const = 0;
    };
}

#endif // __NETWORKCOMMUNICATION_ICONNECTSERVERINFOR_H__
#ifndef _NETWORKCOMMUNICATION_NETLIBWRAPPER_H_
#define _NETWORKCOMMUNICATION_NETLIBWRAPPER_H_

#include "Global.h"
#include "interface/BasicTypes.h"
#include "Base/Pattern/Singleton.h"
#include "Base/Errors.h"
#include "VsmNetLib/VsmNetworkData.h"

namespace NetworkCommunication
{
	using namespace std;
	using namespace vsmnetlib;

	class CNetlibWrapper;
	class IConnectServerInfor;

    typedef void(CALLBACK *Fun_WebSocketCallBackEx)(LONG clent_handle, const WebsocketRecv& recv_data, void* ptr_user_data);

	class  CNetlibWrapper : public Base::CSingleton<CNetlibWrapper>
	{
        friend class Base::CSingleton<CNetlibWrapper>;

    private:
        CNetlibWrapper();

    public:

		enum RequestPattern
		{
			nomalMode,				//����ģʽ
			binary_request,			//����������
			GetPatch,				//get�������� ��Ȼʹ����post����(�в�����4.0�汾Ϊ�˽��get����url�����ض������޸ĵ�)����ʵ����get��Ϊ
		};
		/************************************************************************/
		/* �豸ժҪ��Ҫ                                                                     */
		/************************************************************************/
		/* ժҪ�ϱ���ͷ֤��Ϣ�ṹ */
		struct CsSendAuth
		{
			bool active;          // �Ƿ����ժҪ��Ϣ
			std::string userName;
			std::string realm;
			std::string nonce;
			std::string uri;
			std::string cnonce;
			std::string nc;
			std::string qop;
			std::string response;
			CsSendAuth()
				:active(false)
			{
			}
		};

		/* �ͻ��˷�����Ϣ */
		struct ClientSendInfoExtend
		{
			ClientSendInfoExtend()
				:uCMSPort(80),
				time_out(15000),
				request_pattern(nomalMode),
				ptr_image_buffer(nullptr),
				image_size(0),
				content_type("application/xml; charset=\"UTF-8\"")
			{}

			std::string str_method;      // ����
			std::string str_isapi;       // ISAPIЭ�飺/ISAPI/Bumblebee/Picture
			std::string str_request;     // ��������
			std::string str_send_data;   // ��������
			std::string strCMSIp;        // CMSip
			unsigned int uCMSPort;       // CMS�˿ں�
			std::string strSessionID;
			std::string str_access_type;  // https || http
			DWORD time_out;
			CsSendAuth  client_send_auto;
			RequestPattern	 request_pattern;
			char* ptr_image_buffer;
			unsigned int image_size;
			string content_type;
		};

		/* �ͻ��˽�����Ϣ */
		struct ClientRecvInfoExtend
		{
			ClientRecvInfoExtend()
				: bool_auth(false)
			{
			}

			std::string receive_data;   // �������ݻ��壬����˷���ԭʼ���ݣ�������ͼƬ

			Base::Error::ErrorCode error_code_;

			std::string digest;
			std::string realm;
			std::string nonce;
			std::string qop;
			bool bool_auth;
		};

		/************************************************************************/


        /* �ͻ��˷�����Ϣ */
        struct ClientSendInfo
        {
            std::string str_server_ip;
            unsigned int u_port;
            std::string str_method;      // ����
            std::string str_isapi;       // ISAPIЭ�飺/ISAPI/Bumblebee/Picture
            std::string str_request;     // ��������
            std::string str_send_data;   // ��������
			std::string str_access_type;  // https || http
            DWORD time_out;
			RequestPattern	 request_pattern;
            ClientSendInfo()
                :str_server_ip(""),
                u_port(0),
                str_method(""),
                str_request(""),
                str_send_data(""),
                time_out(15000),
				request_pattern(nomalMode)
            {

            }
        };

        /* �ͻ��˽�����Ϣ */
        struct ClientRecvInfo
        {
			ClientRecvInfo()
			{
			}
			std::string receive_data;   // �������ݻ��壬����˷���ԭʼ���ݣ�������ͼƬ
			Base::Error::ErrorCode error_code_;
        };

        /*websock �����Ӳ���*/
        struct WebsocketClientLoginParam
        {
            std::string str_server_ip;
            unsigned int u_port;
            bool re_link;             // false:�ر��������ܣ�true��������������
            DWORD send_timeout;       // ���ͳ�ʱ,Ĭ��5000ms
            DWORD recv_timeout;       // ���ճ�ʱ,Ĭ��30000ms
            DWORD relink_interval;    // ��λms�����������������ʧ�ܺ��ٴγ��������ļ��ʱ��
            DWORD connect_timeOut;    // ��λms�����ӳ�ʱ��Ĭ��3000ms

            WebsocketClientLoginParam():
                str_server_ip(""),
                u_port(0),
                re_link(true),
                send_timeout(5000),
                recv_timeout(30000),
                relink_interval(5000),
                connect_timeOut(5000)
            {}
        };

        /*�������ö��ֵ����һֱ,������޸�websock �����Ӳ���*/
        enum WebsocketMsg
        {
            no_type = -1,                   //��ʱ�����մ������ݵȣ��ر�����ʱ����
            continuation_msg = 0,
            text_msg = 1,
            binary_mgs = 2,
            close_msg = 8,
            ping_msg = 9,
            pong_msg = 10,
            establish_msg = 11,             //�������
            handshake_reg_msg = 12,         //��������
            reconn_start = 13,              //������ʼ
            reconn_succ = 14,                //�����ɹ�
			connect_excepetion = 100  // �����쳣,reconn_start֮����ܳ������״̬�����������Ѿ��Ͽ�����Ҫҵ������ֽ�������
        };

    public:
        ~CNetlibWrapper();

    public:
        // ��ʼ��
        bool Initialize();
        // ����ʼ��
        bool Uninitialize();

    public:
		clientframework::VsmVersion GetVsmVersion();

		bool ClientSynSendForVsmVersion(ClientRecvInfo& receive_info, const string str_server_ip, unsigned int uPort, const string login_protocol = "http");
        //���������
        // �ͻ���ͬ�����ͽӿ�
		bool ClientSynSend(ClientRecvInfo& receive_info, const ClientSendInfo& send_info, IConnectServerInfor* ptr_server_infor);

		// �ͻ���ͬ�����ͽӿ� for playback and preview
		bool ClientSynSend(ClientRecvInfoExtend& receive_info, const ClientSendInfoExtend& send_info,
											const IConnectServerInfor* ptr_server_infor, bool bool_vsm = true);
        //��½ʱû��session
        bool ClientSynSendForLogin(ClientRecvInfo& receive_info, const ClientSendInfo& send_info, const string str_server_ip, const unsigned int uPort);

		// ����Express
		bool ClientSynSendForExpress(ClientRecvInfo& receive_info, const ClientSendInfo& send_info, const std::string& ip, const int port, const std::string& session, const std::string& security_version, const string& token_secret_key, IConnectServerInfor* ptr_server_infor);

		// ����Զ��վ��
		// 2017-3-16 Ŀǰ��Express��һ���ģ�����Ϊ����չ�����ǵ����ó���
		bool ClientSynSendForRemoteSite(ClientRecvInfo& receive_info, const ClientSendInfo& send_info, const std::string& session, const std::string& security_version, const string& token_secret_key, const IConnectServerInfor* ptr_server_infor);

		// �����豸��ISAPI��
		bool ClientSynSendForDevice(ClientRecvInfoExtend& receive_info, const ClientSendInfoExtend& send_info);

        // ���ó�ʱϵ��
		void SetTimeoutFacotor(clientframework::SystemConfig::NetOverTime enum_net_over_time);

		void GetLoginprotocol(string& str_protocal)
		{
			str_protocal = str_protocol_type_;
		}
    public:
        //���������
        // ��������
        LONG CNetlibWrapper::WebsocketClientSynCreat(LONG& client_handle, const string& str_server_ip, const unsigned int& uPort, const Fun_WebSocketCallBackEx client_callback);
        //��ʼ����
        LONG CNetlibWrapper::WebsocketClientStart(const LONG client_handle);
        //�ر�����
        LONG WebsocketClientStop(const LONG client_handle);
        // ��������
        LONG WebsocketClientSend(LONG client_handle, WebsocketMsg msg_type, char* send_data, DWORD data_len, bool is_fin = true);
    private:
        bool is_initialized_; // �Ƿ��ʼ��
        double time_out_fator_;
		//V1.0.0.0~V1.0.0.6   V1.1.0.0~V1.1.0.6
		clientframework::VsmVersion   vsm_version_;
		//protocol type
		string str_protocol_type_;
	    
	};
}

#endif // _NETWORKCOMMUNICATION_NETLIBWRAPPER_H_

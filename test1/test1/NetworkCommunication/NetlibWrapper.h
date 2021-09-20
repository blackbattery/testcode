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
			nomalMode,				//常规模式
			binary_request,			//二进制请求
			GetPatch,				//get方法补丁 虽然使用了post方法(有部分是4.0版本为了解决get方法url过长截断问题修改的)，但实质是get行为
		};
		/************************************************************************/
		/* 设备摘要需要                                                                     */
		/************************************************************************/
		/* 摘要认报文头证信息结构 */
		struct CsSendAuth
		{
			bool active;          // 是否存在摘要信息
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

		/* 客户端发送信息 */
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

			std::string str_method;      // 方法
			std::string str_isapi;       // ISAPI协议：/ISAPI/Bumblebee/Picture
			std::string str_request;     // 请求数据
			std::string str_send_data;   // 发送数据
			std::string strCMSIp;        // CMSip
			unsigned int uCMSPort;       // CMS端口号
			std::string strSessionID;
			std::string str_access_type;  // https || http
			DWORD time_out;
			CsSendAuth  client_send_auto;
			RequestPattern	 request_pattern;
			char* ptr_image_buffer;
			unsigned int image_size;
			string content_type;
		};

		/* 客户端接收信息 */
		struct ClientRecvInfoExtend
		{
			ClientRecvInfoExtend()
				: bool_auth(false)
			{
			}

			std::string receive_data;   // 接收数据缓冲，服务端返回原始数据，可能是图片

			Base::Error::ErrorCode error_code_;

			std::string digest;
			std::string realm;
			std::string nonce;
			std::string qop;
			bool bool_auth;
		};

		/************************************************************************/


        /* 客户端发送信息 */
        struct ClientSendInfo
        {
            std::string str_server_ip;
            unsigned int u_port;
            std::string str_method;      // 方法
            std::string str_isapi;       // ISAPI协议：/ISAPI/Bumblebee/Picture
            std::string str_request;     // 请求数据
            std::string str_send_data;   // 发送数据
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

        /* 客户端接收信息 */
        struct ClientRecvInfo
        {
			ClientRecvInfo()
			{
			}
			std::string receive_data;   // 接收数据缓冲，服务端返回原始数据，可能是图片
			Base::Error::ErrorCode error_code_;
        };

        /*websock 长连接参数*/
        struct WebsocketClientLoginParam
        {
            std::string str_server_ip;
            unsigned int u_port;
            bool re_link;             // false:关闭重连功能，true：开启重连功能
            DWORD send_timeout;       // 发送超时,默认5000ms
            DWORD recv_timeout;       // 接收超时,默认30000ms
            DWORD relink_interval;    // 单位ms，重连间隔，即重连失败后，再次尝试重连的间隔时间
            DWORD connect_timeOut;    // 单位ms，连接超时，默认3000ms

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

        /*与网络库枚举值保持一直,勿随便修改websock 长连接参数*/
        enum WebsocketMsg
        {
            no_type = -1,                   //超时、接收错误数据等，关闭连接时发送
            continuation_msg = 0,
            text_msg = 1,
            binary_mgs = 2,
            close_msg = 8,
            ping_msg = 9,
            pong_msg = 10,
            establish_msg = 11,             //握手完成
            handshake_reg_msg = 12,         //握手请求
            reconn_start = 13,              //重连开始
            reconn_succ = 14,                //重连成功
			connect_excepetion = 100  // 连接异常,reconn_start之后可能出现这个状态，表明连接已经断开，需要业务层重现建立连接
        };

    public:
        ~CNetlibWrapper();

    public:
        // 初始化
        bool Initialize();
        // 反初始化
        bool Uninitialize();

    public:
		clientframework::VsmVersion GetVsmVersion();

		bool ClientSynSendForVsmVersion(ClientRecvInfo& receive_info, const string str_server_ip, unsigned int uPort, const string login_protocol = "http");
        //短连接相关
        // 客户端同步发送接口
		bool ClientSynSend(ClientRecvInfo& receive_info, const ClientSendInfo& send_info, IConnectServerInfor* ptr_server_infor);

		// 客户端同步发送接口 for playback and preview
		bool ClientSynSend(ClientRecvInfoExtend& receive_info, const ClientSendInfoExtend& send_info,
											const IConnectServerInfor* ptr_server_infor, bool bool_vsm = true);
        //登陆时没有session
        bool ClientSynSendForLogin(ClientRecvInfo& receive_info, const ClientSendInfo& send_info, const string str_server_ip, const unsigned int uPort);

		// 请求Express
		bool ClientSynSendForExpress(ClientRecvInfo& receive_info, const ClientSendInfo& send_info, const std::string& ip, const int port, const std::string& session, const std::string& security_version, const string& token_secret_key, IConnectServerInfor* ptr_server_infor);

		// 请求远端站点
		// 2017-3-16 目前跟Express是一样的，但是为了扩展，还是单独拿出来
		bool ClientSynSendForRemoteSite(ClientRecvInfo& receive_info, const ClientSendInfo& send_info, const std::string& session, const std::string& security_version, const string& token_secret_key, const IConnectServerInfor* ptr_server_infor);

		// 请求设备（ISAPI）
		bool ClientSynSendForDevice(ClientRecvInfoExtend& receive_info, const ClientSendInfoExtend& send_info);

        // 设置超时系数
		void SetTimeoutFacotor(clientframework::SystemConfig::NetOverTime enum_net_over_time);

		void GetLoginprotocol(string& str_protocal)
		{
			str_protocal = str_protocol_type_;
		}
    public:
        //长连接相关
        // 创建连接
        LONG CNetlibWrapper::WebsocketClientSynCreat(LONG& client_handle, const string& str_server_ip, const unsigned int& uPort, const Fun_WebSocketCallBackEx client_callback);
        //开始握手
        LONG CNetlibWrapper::WebsocketClientStart(const LONG client_handle);
        //关闭连接
        LONG WebsocketClientStop(const LONG client_handle);
        // 发送数据
        LONG WebsocketClientSend(LONG client_handle, WebsocketMsg msg_type, char* send_data, DWORD data_len, bool is_fin = true);
    private:
        bool is_initialized_; // 是否初始化
        double time_out_fator_;
		//V1.0.0.0~V1.0.0.6   V1.1.0.0~V1.1.0.6
		clientframework::VsmVersion   vsm_version_;
		//protocol type
		string str_protocol_type_;
	    
	};
}

#endif // _NETWORKCOMMUNICATION_NETLIBWRAPPER_H_

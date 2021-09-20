#include "stdafx.h"
#include "NetlibWrapper.h"
#include "IConnectServerInfor.h"
#include "NetworkCommunication/Proto/XmlProto.h"
#include "interface/ErrorCode.h"
#include "interface/CMSErrorCode.h"
#include "Base/Util/StringUtil.h"
#include "interface/LogType.h"
#include "VsmNetLib/VsmNetwork.h"

namespace NetworkCommunication
{
    namespace
    {
        string HexNum[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f" };

        // 把字符串转换为16进制
        void ToHexString(string& data_out, const string& data_in)
        {
            for (auto& c : data_in)
            {
                byte ch = static_cast<byte>(c);
                int q1 = ch / 16;
                int q2 = ch % 16;

                data_out.append(HexNum[q1] + HexNum[q2]);
            }
        }
    }

    using namespace vsmnetlib;

    // 构造函数
    CNetlibWrapper::CNetlibWrapper()
        : is_initialized_(false),
        time_out_fator_(1),
		vsm_version_(clientframework::Version110)
    {
		str_protocol_type_ = "http";
    }

    CNetlibWrapper::~CNetlibWrapper()
    {

    }

    // 初始化
    bool CNetlibWrapper::Initialize()
    {
        if (is_initialized_)
        {
            return true;
        }

		
        // 初始化网络库
        is_initialized_ = CVsmNetLib::VsmNetLibInit();
        if (!is_initialized_)
        {
            assert(0); // 网络库的客户端初始化失败
            IVMS_LOG_ERROR("ClientWarningAndMore", "CVsmNetLib::VsmNetLibInit failed");
        }

		//CVsmNetLib::StartSdkLog(3, true, "c:/SdkUtilsLog/");

        return is_initialized_;
    }

    // 反初始化
    bool CNetlibWrapper::Uninitialize()
    {
        if (!is_initialized_)
        {
            return true;
        }

        // 反初始化网络库
        return CVsmNetLib::VsmNetLibUnInit();
    }

	bool CNetlibWrapper::ClientSynSendForVsmVersion(ClientRecvInfo& receive_info, const string str_server_ip, unsigned int uPort, const string login_protocol)
	{
		string str_isapi;
		HttpRequest http_request;
		// 方法		
		http_request.http_method = "GET";
		str_isapi = "/ISAPI/Bumblebee/Version";
		http_request.http_url = login_protocol + "://" + str_server_ip + ":" + Base::StringUtil::T2String(uPort) + str_isapi;
		// 当前连接句柄,返回值
		LONG client_handle = -1;
		// 客户端接收数据结构体,返回值
		HttpClientRecv recv_data;
		// 客户端发送函数,接收的数据处理完成后,需要关闭当前连接
		LONG ret_value = CVsmNetLib::HttpClientSynSend(client_handle, recv_data, http_request);
		//ClientRecvInfo receive_info;
		if (NET_ERR_NONE == ret_value)
		{
			// 解析收到数据,首先判断http错误码
			if (recv_data.http_code == 200)
			{
				// 比较http协议头的数据长度和收到的数据长度是否一致,说明数据接收完成
				if ((recv_data.content_len == recv_data.recv_size) && (recv_data.recv_size != 0))
				{
					receive_info.receive_data.resize(recv_data.recv_size);
					memcpy(const_cast<char*>(receive_info.receive_data.c_str()), recv_data.recv_data, recv_data.recv_size);
					if (!receive_info.receive_data.empty())
					{
						unique_ptr<CXmlProto> pro_req(new CXmlProto());
						pro_req->set_str(receive_info.receive_data);
						receive_info.error_code_.iFirstCode = pro_req->get_int_v("/ResponseStatus/ErrorModule");
						receive_info.error_code_.iSecondCode = pro_req->get_int_v("/ResponseStatus/ErrorCode");
						receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_VSM);
						receive_info.error_code_.SetCasadingErrorInfo(pro_req->get_v("/ResponseStatus/CascadingErrorInfo"));
						
						string str_vsm_version;
						str_vsm_version = pro_req->get_v("/ResponseStatus/Data/Version/SoftVersion");
						string str_protocol_type = pro_req->get_v("/ResponseStatus/Data/Version/AccessType");
						if (!str_protocol_type.empty())
						{
							str_protocol_type_ = str_protocol_type;
						}

						//V1.0.0.0  最后一位是build号 不比较
						if (!str_vsm_version.empty())
						{
							str_vsm_version.replace(str_vsm_version.begin(), str_vsm_version.begin() + 1, "");		//1.0.0.0
							str_vsm_version.replace(1, 1, "");		//10.0.0
							str_vsm_version.replace(2, 1, "");		//100.0
							vsm_version_ = static_cast<clientframework::VsmVersion>(atoi(str_vsm_version.c_str()));
						}
					}
				}
				else
				{
					// 接收数据长度错误
					assert(0);

					receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
					receive_info.error_code_.iFirstCode = IVMS_COMPONENT_LOCAL;
					receive_info.error_code_.iSecondCode = IVMS_INVALID_XML_CONTENT;

					IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSendForVsmVersion] failed, [errorcode: %d],[request: %s]", IVMS_INVALID_XML_CONTENT, http_request.http_url.c_str());
					// 关闭连接
					CVsmNetLib::HttpClostClientLink(client_handle);

					return false;
				}
			}
			else
			{
				// http协议错误
				assert(0);
				receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
				receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
				receive_info.error_code_.iSecondCode = recv_data.http_code;
				IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSendForVsmVersion] failed, [errorcode: %d],[request: %s]", recv_data.http_code, http_request.http_url.c_str());

				// 关闭连接
				CVsmNetLib::HttpClostClientLink(client_handle);

				return false;
			}
		}
		else
		{
			// 接收数据失败
			receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
			receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
			receive_info.error_code_.iSecondCode = ret_value;
			IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSendForVsmVersion] failed, [errorcode: %d],[request: %s]", ret_value, http_request.http_url.c_str());

			// 关闭连接
			CVsmNetLib::HttpClostClientLink(client_handle);

			return false;
		}

		// 数据处理完成后,需要关闭连接
		CVsmNetLib::HttpClostClientLink(client_handle);

		return true;
	}

    bool CNetlibWrapper::ClientSynSendForLogin(ClientRecvInfo& receive_info, const ClientSendInfo& send_info, const string str_server_ip, unsigned int uPort)
    {
        // 赋值请求参数
		string str_isapi  = send_info.str_isapi;
        HttpRequest http_request;
		// 方法
		if (send_info.str_method == "GET" && send_info.request_pattern != binary_request)
		{
			http_request.http_method = "POST";
			
			int nSize = str_isapi.find("?");
			if (nSize == -1)
			{
				str_isapi += "?MT=GET";
			}
			else
			{
				str_isapi.insert(nSize+1, "MT=GET&");
			}


			//普通的get改成post之后，http_request.send_buff不能为空，必须赋值

			http_request.send_size = send_info.str_request.size();
			http_request.send_buff = const_cast<char*>(send_info.str_request.c_str());
			IVMS_LOG_INFO("CommLayer", "[CNetlibWrapper::ClientSynSendForLogin] http_request send_buff %s", send_info.str_request.c_str());

		}
		else if (send_info.str_method == "POST" && send_info.request_pattern == GetPatch)
		{
			//虽然也使用了post方法(有部分是4.0版本为了解决get方法url过长截断问题修改的)，但实质是get行为，故也在Query中增加MT=GET
			int nSize = str_isapi.find("?");
			if (nSize == -1)
			{
				str_isapi += "?MT=GET";
			}
			else
			{
				str_isapi.insert(nSize + 1, "MT=GET&");
			}
			http_request.http_method = send_info.str_method;
		}
		else
			http_request.http_method = send_info.str_method;


        // 请求url
        //eg: http://10.17.133.26:80/ISAPI/PTZCtrl/channels/1/patrols/capabilities //str_protocol_type_
		http_request.http_url = str_protocol_type_ + "://" + str_server_ip + ":" + Base::StringUtil::T2String(uPort) + str_isapi;

        IVMS_LOG_INFO("CommLayer", "[CNetlibWrapper::ClientSynSendForLogin] http_request url %s requst method :%s", http_request.http_url.c_str(), http_request.http_method.c_str());

        // 发送数据
        if (!send_info.str_send_data.empty())
        {
            http_request.send_size = send_info.str_send_data.size();
            http_request.send_buff = const_cast<char*>(send_info.str_send_data.c_str());
            //IVMS_LOG_INFO("CommLayer", "[CNetlibWrapper::ClientSynSendForLogin] http_request str_send_data %s", send_info.str_send_data.c_str());
        }


        // 当前连接句柄,返回值
        LONG client_handle = -1;
        // 客户端接收数据结构体,返回值
        HttpClientRecv recv_data;

		http_request.recv_timeout = send_info.time_out;
        
        // 客户端发送函数,接收的数据处理完成后,需要关闭当前连接
        LONG ret_value = CVsmNetLib::HttpClientSynSend(client_handle, recv_data, http_request);
        if (NET_ERR_NONE == ret_value)
        {
            // 解析收到数据,首先判断http错误码
            if (recv_data.http_code == 200)
            {
                // 比较http协议头的数据长度和收到的数据长度是否一致,说明数据接收完成
                if ((recv_data.content_len == recv_data.recv_size) && (recv_data.recv_size != 0))
                {
                    receive_info.receive_data.resize(recv_data.recv_size);
                    memcpy(const_cast<char*>(receive_info.receive_data.c_str()), recv_data.recv_data, recv_data.recv_size);

                    if (!receive_info.receive_data.empty())
                    {
						unique_ptr<CXmlProto> pro_req(new CXmlProto());
						bool b_ret = pro_req->set_str(receive_info.receive_data);
						receive_info.error_code_.iFirstCode = pro_req->get_int_v("/ResponseStatus/ErrorModule");
						receive_info.error_code_.iSecondCode = pro_req->get_int_v("/ResponseStatus/ErrorCode");
						receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_VSM);
						receive_info.error_code_.SetCasadingErrorInfo(pro_req->get_v("/ResponseStatus/CascadingErrorInfo"));

						//存在目前IP 不可达，中间网络设备返回数据的情况，但数据非VSM 返回格式
						if (!b_ret)
						{
							receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
							receive_info.error_code_.iFirstCode = IVMS_COMPONENT_LOCAL;
							receive_info.error_code_.iSecondCode = IVMS_INVALID_XML_CONTENT;

							IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSendForLogin] failed, [errorcode: %d],[request: %s],[respond:%s]", IVMS_INVALID_XML_CONTENT, http_request.http_url.c_str(), receive_info.receive_data.c_str());
						}
                    }
                }
                else
                {
                    // 接收数据长度错误
                    assert(0);

                    receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
					receive_info.error_code_.iFirstCode = IVMS_COMPONENT_LOCAL;
                    receive_info.error_code_.iSecondCode = IVMS_INVALID_XML_CONTENT;

                    IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSendForLogin] failed, [errorcode: %d],[request: %s]", IVMS_INVALID_XML_CONTENT, http_request.http_url.c_str());
                    // 关闭连接
                    CVsmNetLib::HttpClostClientLink(client_handle);

                    return false;
                }
            }
            else
            {
                // http协议错误
				assert(0);

                receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
				receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
                receive_info.error_code_.iSecondCode = recv_data.http_code;
                IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSendForLogin] failed, [errorcode: %d],[request: %s]", recv_data.http_code, http_request.http_url.c_str());

                // 关闭连接
                CVsmNetLib::HttpClostClientLink(client_handle);

                return false;
            }
        }
        else
        {
            // 接收数据失败
			receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
			receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
            receive_info.error_code_.iSecondCode = ret_value;
            IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSendForLogin] failed, [errorcode: %d],[request: %s]", ret_value, http_request.http_url.c_str());

            // 关闭连接
            CVsmNetLib::HttpClostClientLink(client_handle);

            return false;
        }

        // 数据处理完成后,需要关闭连接
        CVsmNetLib::HttpClostClientLink(client_handle);

        return true;
    }

    // 客户端同步发送接口
	bool CNetlibWrapper::ClientSynSend(ClientRecvInfo& receive_info, const ClientSendInfo& send_info, IConnectServerInfor* ptr_server_infor)
    {
		if (ptr_server_infor == NULL)
		{
			receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
			receive_info.error_code_.iFirstCode = IVMS_COMPONENT_LOCAL;
			receive_info.error_code_.iSecondCode = IVMS_INPUT_PARAMETERS_ERROR;
			return false;
		}

		string strCMSIP = ptr_server_infor->GetIPAddress();
		unsigned int uPort = ptr_server_infor->GetPort();
		string strSessionID = ptr_server_infor->GetSession();
		string strSessionIDEx = strSessionID;
        //外部传入则以传入的为准
        if (!send_info.str_server_ip.empty())
        {
            strCMSIP = send_info.str_server_ip;
        }

        if (send_info.u_port > 0)
        {
            uPort = send_info.u_port;
        }

        // 赋值请求参数
        HttpRequest http_request;

		// 方法
		if (send_info.str_method == "GET" && send_info.request_pattern != binary_request)
		{
			http_request.http_method = "POST";

			strSessionIDEx += "&MT=GET";

			//普通的get改成post之后，http_request.send_buff不能为空，必须赋值

			http_request.send_size = send_info.str_request.size();
			http_request.send_buff = const_cast<char*>(send_info.str_request.c_str());
			IVMS_LOG_INFO("CommLayer", "[CNetlibWrapper::ClientSynSend] http_request send_buff %s", send_info.str_request.c_str());
		}
		else if (send_info.str_method == "POST" && send_info.request_pattern == GetPatch)
		{
			//虽然也使用了post方法(有部分是4.0版本为了解决get方法url过长截断问题修改的)，但实质是get行为，故也在Query中增加MT=GET
			strSessionIDEx += "&MT=GET";
			http_request.http_method = send_info.str_method;
		}
		else
			http_request.http_method = send_info.str_method;

		//isapi协议
		string str_isapi;
		if (!send_info.str_request.empty())
		{
			if (send_info.str_method == "GET" && send_info.request_pattern == binary_request)
			{
				str_isapi = send_info.str_isapi + "?SID=" + strSessionIDEx + "&" + send_info.str_request;
			}
			else
			{
				//HC1.4版本删除data绑定，1.3版本系统测试后修改，所以不敢直接删除
				if (send_info.str_request.size() < 1400)
				{
					str_isapi = send_info.str_isapi + "?SID=" + strSessionIDEx + "&request_parameter_xml=" + send_info.str_request;
				}
				else
				{
					str_isapi = send_info.str_isapi + "?SID=" + strSessionIDEx;
					IVMS_LOG_INFO(LOG_COMM_LAYER, "Url size > 1400.");
				}
			}
		}
		else
		{
			str_isapi = send_info.str_isapi + "?SID=" + strSessionIDEx;
		}

        // 请求url
        //string str_isapi = send_info.str_isapi + "?SID=" + strSessionID + "&" + send_info.str_request;//str_protocol_type_
		http_request.http_url = str_protocol_type_ + "://" + strCMSIP + ":" + Base::StringUtil::T2String(uPort) + str_isapi;

        IVMS_LOG_INFO("CommLayer", "[CNetlibWrapper::ClientSynSend] http_request url %s requst method :%s", http_request.http_url.c_str(), http_request.http_method.c_str());

        // token
		if (!ptr_server_infor->GetToken(strSessionID, http_request.token_value))
        {
            assert(0);
			receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
            receive_info.error_code_.iFirstCode = IVMS_COMPONENT_Security;
			// receive_info.error_code_.iSecondCode = ; //目前没有返回错误码！！
            IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSend] CSecurityManager::Instance().GetToken failure");
            return false;
        }

        // 发送数据  原先get方法必定为空 否则和前面冲突了
        if (!send_info.str_send_data.empty())
        {
            http_request.send_size = send_info.str_send_data.size();
            http_request.send_buff = const_cast<char*>(send_info.str_send_data.c_str());			
            //IVMS_LOG_INFO("CommLayer", "[CNetlibWrapper::ClientSynSendForLogin] http_request str_send_data %s", send_info.str_send_data.c_str());
        }

        http_request.recv_timeout = send_info.time_out * time_out_fator_;
        http_request.connect_tiemout *= time_out_fator_;
        http_request.send_timeout *= time_out_fator_;

        // 当前连接句柄,返回值
        LONG client_handle = -1;
        // 客户端接收数据结构体,返回值
        HttpClientRecv recv_data;

        // 客户端发送函数,接收的数据处理完成后,需要关闭当前连接
        LONG ret_value = CVsmNetLib::HttpClientSynSend(client_handle, recv_data, http_request);
        if (NET_ERR_NONE == ret_value)
        {
            // 解析收到数据,首先判断http错误码
            if (recv_data.http_code == 200)
            {
                // 比较http协议头的数据长度和收到的数据长度是否一致,说明数据接收完成
                if ((recv_data.content_len == recv_data.recv_size) && (recv_data.recv_size != 0))
                {
                    receive_info.receive_data.resize(recv_data.recv_size);
                    memcpy(const_cast<char*>(receive_info.receive_data.c_str()), recv_data.recv_data, recv_data.recv_size);
                    if (!receive_info.receive_data.empty())
                    {
						unique_ptr<CXmlProto> pro_req(new CXmlProto());
						pro_req->set_str(receive_info.receive_data);
						receive_info.error_code_.iFirstCode = pro_req->get_int_v("/ResponseStatus/ErrorModule");
						receive_info.error_code_.iSecondCode = pro_req->get_int_v("/ResponseStatus/ErrorCode");
						receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_VSM);
						receive_info.error_code_.SetCasadingErrorInfo(pro_req->get_v("/ResponseStatus/CascadingErrorInfo"));
						if (CMS_MODULE_CMS == receive_info.error_code_.iFirstCode && CMS_SESSIONID_INVALID == receive_info.error_code_.iSecondCode)
						{
							//特殊错误打印日志
							IVMS_LOG_WARN("CommLayer", "[CNetlibWrapper::ClientSynSend] http_url:%s requeset CMS_SESSIONID_INVALID session:%s, release token:%s", http_request.http_url.c_str(), strSessionID.c_str(), http_request.token_value.c_str());
							// 无效Seeeion，释放Token
							ptr_server_infor->SercurityRelease(receive_info.error_code_.iFirstCode, receive_info.error_code_.iSecondCode, strSessionID);
						}
						if (CMS_MODULE_CMS == receive_info.error_code_.iFirstCode
							&& (CMS_TOKEN_INVALID == receive_info.error_code_.iSecondCode || CMS_TOKEN_OVERDUE == receive_info.error_code_.iSecondCode))
						{
							IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSend] http_url:%s ", http_request.http_url.c_str());
							IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSend] requeset CMS_SESSIONID_INVALID session : %s, token : %s", strSessionID.c_str(), http_request.token_value.c_str());

							int round_num = 100;
							string str_challenge;
							ptr_server_infor->GetSHARule(str_challenge, round_num);
							//IVMS_LOG_INFO("CommLayer", "[CNetlibWrapper::ClientSynSend] str_challenge:%s  round_num : %d", str_challenge.c_str(), round_num);

							string tmp_key;
							ptr_server_infor->GetEcryptionPwd(tmp_key);

							string token_key;
							ToHexString(token_key, tmp_key);
							//IVMS_LOG_INFO("CommLayer", "[CNetlibWrapper::ClientSynSend] requeset  token_key : %s", token_key.c_str());
						}                      
                    }
                }
                else
                {
                    // 接收数据长度错误
                    assert(0);

					receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
					receive_info.error_code_.iFirstCode = IVMS_COMPONENT_LOCAL;
                    receive_info.error_code_.iSecondCode = IVMS_INVALID_XML_CONTENT;
                    // 关闭连接
                    CVsmNetLib::HttpClostClientLink(client_handle);

                    return false;
                }
            }
            else
            {
                // http协议错误
				assert(0);

                receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
				receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
                receive_info.error_code_.iSecondCode = recv_data.http_code;
                IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSend] failed, [errorcode: %d],[request: %s]", recv_data.http_code, http_request.http_url.c_str());

                // 关闭连接
                CVsmNetLib::HttpClostClientLink(client_handle);

                return false;
            }
        }
        else
        {
			// 接收数据失败
            receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
			receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
            receive_info.error_code_.iSecondCode = ret_value;
			IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSend] failed, [errorcode: %d],[request: %s]", ret_value, http_request.http_url.c_str());
            // 关闭连接
            CVsmNetLib::HttpClostClientLink(client_handle);

            return false;
        }

        // 数据处理完成后,需要关闭连接
        CVsmNetLib::HttpClostClientLink(client_handle);

        return true;
    }

	// 客户端同步发送接口
	bool CNetlibWrapper::ClientSynSend(ClientRecvInfoExtend& receive_info, const ClientSendInfoExtend& send_info,
										const IConnectServerInfor* ptr_server_infor, bool bool_vsm)
	{
		if (nullptr == ptr_server_infor)
		{
			receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
			receive_info.error_code_.iFirstCode = IVMS_COMPONENT_LOCAL;
			receive_info.error_code_.iSecondCode = IVMS_INPUT_PARAMETERS_ERROR;
			return false;
		}
		// 赋值请求参数
		HttpRequest http_request;
		string str_isapi = send_info.str_isapi;
		// 方法
		if (send_info.str_method == "GET" && send_info.request_pattern != binary_request && bool_vsm)
		{
			http_request.http_method = "POST";
			int nSize = str_isapi.find("?");
			if (nSize == -1)
			{
				receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
				receive_info.error_code_.iFirstCode = IVMS_COMPONENT_LOCAL;
				receive_info.error_code_.iSecondCode = IVMS_INPUT_PARAMETERS_ERROR;
				return false;
			}
			else
			{
				str_isapi.insert(nSize+1, "MT=GET&");
			}

			//普通的get改成post之后，http_request.send_buff不能为空，必须赋值

			http_request.send_size = send_info.str_request.size();
			http_request.send_buff = const_cast<char*>(send_info.str_request.c_str());
		}
		else if (send_info.str_method == "POST" && send_info.request_pattern == GetPatch)
		{
			//虽然也使用了post方法(有部分是4.0版本为了解决get方法url过长截断问题修改的)，但实质是get行为，故也在Query中增加MT=GET
			int nSize = str_isapi.find("?");
			if (nSize == -1)
			{
				receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
				receive_info.error_code_.iFirstCode = IVMS_COMPONENT_LOCAL;
				receive_info.error_code_.iSecondCode = IVMS_INPUT_PARAMETERS_ERROR;
				return false;
			}
			else
			{
				str_isapi.insert(nSize + 1, "MT=GET&");
			}

			http_request.http_method = send_info.str_method;
		}
		else
			http_request.http_method = send_info.str_method;
		// 请求url
// 		string str_final_request = "";
// 		if (!send_info.str_isapi.empty())
// 		{
// 			//str_isapi = send_info.str_isapi + "?SID=" + strSessionID + "&request_parameter_xml=" + send_info.str_request;
// 			////新的XML实现出来是<?xml version="1.0" encoding="UTF-8"?>\n<DeviceStatusRequest>。。。</DeviceStatusRequest >\n
// 			////多出了前面一段<?xml version="1.0" encoding="UTF-8"?>,但是这里的UTF-8可能会变。
// 			str_final_request = send_info.str_isapi;
// 			string head = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
// 			size_t nPos = send_info.str_isapi.find(head);
// 			if (nPos != -1)
// 			{
// 				str_final_request.erase(nPos, head.size());
// 				str_final_request = str_final_request.substr(0, str_final_request.size() - 1);
// 			}
// 		}

		//std::string str_isapi = send_info.str_isapi + "?SID=" + send_info.strSessionID + "&" + send_info.str_request;
		string protocal_type = send_info.str_access_type.empty() ? str_protocol_type_ : send_info.str_access_type;
		const char* ch_url = "%s://%s:%d%s";
		//http_request.http_url = protocal_type + "://" + send_info.strCMSIp + ":" + Base::StringUtil::T2String(send_info.uCMSPort) + str_isapi;
		http_request.http_url = Base::StringUtil::str_fmt(ch_url, protocal_type.c_str(), send_info.strCMSIp.c_str(), send_info.uCMSPort, str_isapi.c_str());
		// token
		if (bool_vsm)
		{
			//走vsm摘要认证
			if (!ptr_server_infor->GetToken(send_info.strSessionID, http_request.token_value))
			{
				//token 建立失败
				receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
				receive_info.error_code_.iFirstCode = IVMS_COMPONENT_LOCAL;
				receive_info.error_code_.iSecondCode = IVMS_INVALID_TOKEN;
				return false;
			}
		}
		else
		{
			http_request.recv_timeout = 5000;
		}

		if (send_info.client_send_auto.active)
		{
			http_request.client_auth.active = true;
			http_request.client_auth.cnonce = send_info.client_send_auto.cnonce;
			http_request.client_auth.nc = send_info.client_send_auto.nc;
			http_request.client_auth.nonce = send_info.client_send_auto.nonce;
			http_request.client_auth.qop = send_info.client_send_auto.qop;
			http_request.client_auth.realm = send_info.client_send_auto.realm;
			http_request.client_auth.uri = send_info.client_send_auto.uri;
			http_request.client_auth.username = send_info.client_send_auto.userName;
			http_request.client_auth.response = send_info.client_send_auto.response;
		}
		// 发送数据
		if (!send_info.str_send_data.empty())
		{
			http_request.send_size = send_info.str_send_data.size();
			http_request.send_buff = const_cast<char*>(send_info.str_send_data.c_str());
		}

		// 当前连接句柄,返回值
		LONG client_handle = -1;
		// 客户端接收数据结构体,返回值
		HttpClientRecv recv_data;

		// 客户端发送函数,接收的数据处理完成后,需要关闭当前连接
		LONG ret_value = CVsmNetLib::HttpClientSynSend(client_handle, recv_data, http_request);
		if (NET_ERR_NONE == ret_value)
		{
			// 解析收到数据,首先判断http错误码
			if (recv_data.http_code == 200 || recv_data.http_code == 401)
			{
				// 比较http协议头的数据长度和收到的数据长度是否一致,说明数据接收完成
				if ((recv_data.content_len == recv_data.recv_size) && (recv_data.recv_size != 0))
				{
					receive_info.receive_data.resize(recv_data.recv_size);
					memcpy(const_cast<char*>(receive_info.receive_data.c_str()), recv_data.recv_data, recv_data.recv_size);
					if (!receive_info.receive_data.empty())
					{
						//size_t t1 = receive_info.receive_data.find("xmlns");
						if (string::npos != receive_info.receive_data.find("xmlns"))
						{
							//string s = Base::StringUtil::StringReplace(receive_info.receive_data, "xmlns", "xmln");
							receive_info.receive_data = Base::StringUtil::StringReplace(receive_info.receive_data, "xmlns", "xmln");
						}
						unique_ptr<CXmlProto> pro_req(new CXmlProto());
						pro_req->set_str(receive_info.receive_data);
						receive_info.error_code_.iFirstCode = pro_req->get_int_v("/ResponseStatus/ErrorModule");
						receive_info.error_code_.iSecondCode = pro_req->get_int_v("/ResponseStatus/ErrorCode");
						receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_VSM);
						receive_info.error_code_.SetCasadingErrorInfo(pro_req->get_v("/ResponseStatus/CascadingErrorInfo"));
						if (CMS_MODULE_CMS == receive_info.error_code_.iFirstCode && CMS_SESSIONID_INVALID == receive_info.error_code_.iSecondCode)
                        {
                            // 无效Seeeion，释放Token
							ptr_server_infor->SercurityRelease(receive_info.error_code_.iFirstCode, receive_info.error_code_.iSecondCode, send_info.strSessionID);
                        }
					}

					if (recv_data.http_code == 401)
					{
						receive_info.bool_auth = true;
						receive_info.digest = recv_data.client_auth.digest;
						receive_info.realm = recv_data.client_auth.realm;
						receive_info.nonce = recv_data.client_auth.nonce;
						receive_info.qop = recv_data.client_auth.qop;
					}
				}
				else
				{
					// 接收数据长度错误
					assert(0);

					receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
					receive_info.error_code_.iFirstCode = IVMS_COMPONENT_LOCAL;
					receive_info.error_code_.iSecondCode = IVMS_INVALID_XML_CONTENT;
					// 关闭连接
					CVsmNetLib::HttpClostClientLink(client_handle);

					return false;
				}
			}
			else
			{
				// http协议错误
				assert(0);

				// TODO 错误码
				receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
				receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
				receive_info.error_code_.iSecondCode = recv_data.http_code;
				IVMS_LOG_ERROR("CommLayer", "[CPlaybackNetlibWrapper::ClientSynSend] failed, [errorcode: %d],[request: %s]", recv_data.http_code, http_request.http_url.c_str());
				// 关闭连接
				CVsmNetLib::HttpClostClientLink(client_handle);

				return false;
			}
		}
		else
		{
			// 接收数据失败
			receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
			receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
			receive_info.error_code_.iSecondCode = ret_value;

			IVMS_LOG_ERROR("CommLayer", "[CPlaybackNetlibWrapper::ClientSynSend] failed, [errorcode: %d],[request: %s]", ret_value, http_request.http_url.c_str());
			// 关闭连接
			CVsmNetLib::HttpClostClientLink(client_handle);

			return false;
		}

		// 数据处理完成后,需要关闭连接
		CVsmNetLib::HttpClostClientLink(client_handle);

		return true;
	}

	//直接走express不修改Get方法
	bool CNetlibWrapper::ClientSynSendForExpress(ClientRecvInfo& receive_info, const ClientSendInfo& send_info, const string& ip, const int port, const string& session, const string& security_version, const string& token_secret_key, IConnectServerInfor* ptr_server_infor)
	{
		// 2017-6-27 HikCentral1.1不接入Express，注释此接口 by yaojz
		return false;
		if (ip.empty() || session.empty() || security_version.empty())
		{
			receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
			receive_info.error_code_.iFirstCode = IVMS_COMPONENT_LOCAL;
			receive_info.error_code_.iSecondCode = IVMS_INPUT_PARAMETERS_ERROR;
            IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSendForExpress] input paramerters error");
			return false;
		}		

        // 赋值请求参数
        HttpRequest http_request;
        // 方法
        http_request.http_method = send_info.str_method;

		string str_isapi = send_info.str_isapi;
        // 请求url
		if (!send_info.str_request.empty())
		{
			////新的XML实现出来是<?xml version="1.0" encoding="UTF-8"?>\n<DeviceStatusRequest>。。。</DeviceStatusRequest >\n
			////多出了前面一段<?xml version="1.0" encoding="UTF-8"?>,但是这里的UTF-8可能会变。
			string str_final_request = send_info.str_request;
			string head = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
			if (send_info.str_request.find(head) != string::npos)
			{
				str_final_request = send_info.str_request.substr(head.size());
				str_final_request = str_final_request.substr(0, str_final_request.size() - 1);
			}
			str_isapi += "&request_parameter_xml=" + str_final_request;
		}		
		
		http_request.http_url =   "http://" + ip + ":" + Base::StringUtil::T2String(port) + str_isapi;

        IVMS_LOG_INFO("CommLayer", "[CNetlibWrapper::ClientSynSendForExpress] http_request url %s requst method :%s", http_request.http_url.c_str(), http_request.http_method.c_str());

        // token 需判断返回值 
		ptr_server_infor->CreateTokenForExpress(http_request.token_value, session, security_version, token_secret_key);
        // 发送数据
        if (!send_info.str_send_data.empty())
        {
            http_request.send_size = send_info.str_send_data.size();
            http_request.send_buff = const_cast<char*>(send_info.str_send_data.c_str());
            //IVMS_LOG_INFO("CommLayer", "[CNetlibWrapper::ClientSynSendForExpress] http_request str_send_data %s", send_info.str_send_data.c_str());
        }

		// 网络情况设置放大因子
		http_request.recv_timeout = send_info.time_out * time_out_fator_;
		http_request.connect_tiemout *= time_out_fator_;
		http_request.send_timeout *= time_out_fator_;

        // 当前连接句柄,返回值
        LONG client_handle = -1;
        // 客户端接收数据结构体,返回值
        HttpClientRecv recv_data;

        // 客户端发送函数,接收的数据处理完成后,需要关闭当前连接
        LONG ret_value = CVsmNetLib::HttpClientSynSend(client_handle, recv_data, http_request);
        if (NET_ERR_NONE == ret_value)
        {
            // 解析收到数据,首先判断http错误码
            if (recv_data.http_code == 200)
            {
                // 比较http协议头的数据长度和收到的数据长度是否一致,说明数据接收完成
                if ((recv_data.content_len == recv_data.recv_size) && (recv_data.recv_size != 0))
                {
                    receive_info.receive_data.resize(recv_data.recv_size);
                    memcpy(const_cast<char*>(receive_info.receive_data.c_str()), recv_data.recv_data, recv_data.recv_size);
                    if (!receive_info.receive_data.empty())
                    {
						// express返回错误码，客户端进行调整
						// 子系统：客户端
						// 错误模块：Express
						// 错误码：50
						// 级联信息：Express返回的错误码
						unique_ptr<CXmlProto> pro_req(new CXmlProto());
						pro_req->set_str(receive_info.receive_data);
						receive_info.error_code_.iFirstCode = IVMS_COMPONENT_RemoteSite;
						receive_info.error_code_.iSecondCode = IVMS_CASCADING_ERROR_CODE;
						receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
						receive_info.error_code_.SetCasadingErrorInfo(Base::Error::MakeCascadingErrorStr(pro_req->get_int_v("/ResponseStatus/ErrorModule"), pro_req->get_int_v("/ResponseStatus/ErrorCode")));
						if (CMS_MODULE_CMS == receive_info.error_code_.iFirstCode && CMS_SESSIONID_INVALID == receive_info.error_code_.iSecondCode)
						{
							// 无效Seeeion，释放Token
							ptr_server_infor->SercurityRelease(receive_info.error_code_.iFirstCode, receive_info.error_code_.iSecondCode, session);
						}

						if (pro_req->get_int_v("/ResponseStatus/ErrorCode") != 0)
						{
							// session无效的200码需要特殊处理
							if (CMS_SESSIONID_INVALID == pro_req->get_int_v("/ResponseStatus/ErrorCode"))
							{
								IVMS_LOG_WARN("CommLayer", "[CNetlibWrapper::ClientSynSendForExpress] http_url:%s requeset CMS_SESSIONID_INVALID session:%s, release token:%s", http_request.http_url.c_str(), session.c_str(), http_request.token_value.c_str());
								// 无效Seeeion，释放Token
								ptr_server_infor->ReleaseTokenFroExpress(receive_info.error_code_.iFirstCode, receive_info.error_code_.iSecondCode, session);
							}
							else
							{
								return false;
							}
						}
                    }
                }
                else
                {
                    // 接收数据长度错误
					assert(0);

					receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
					receive_info.error_code_.iFirstCode = IVMS_COMPONENT_LOCAL;
					receive_info.error_code_.iSecondCode = IVMS_INVALID_DATA;
                    // 关闭连接
                    CVsmNetLib::HttpClostClientLink(client_handle);

                    return false;
                }
            }
            else
            {
                // http协议错误
				assert(0);

				receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
				receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
                receive_info.error_code_.iSecondCode = recv_data.http_code;
			
                // 关闭连接
                CVsmNetLib::HttpClostClientLink(client_handle);

                return false;
            }
        }
        else
        {
            // 接收数据失败
			receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
			receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
			receive_info.error_code_.iSecondCode = ret_value;
			IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSendForExpress] failed, [errorcode: %d],[request: %s]", ret_value, http_request.http_url.c_str());
            // 关闭连接
            CVsmNetLib::HttpClostClientLink(client_handle);

            return false;
        }

        // 数据处理完成后,需要关闭连接
        CVsmNetLib::HttpClostClientLink(client_handle);

        return true;
	}

	bool CNetlibWrapper::ClientSynSendForDevice(ClientRecvInfoExtend& receive_info, const ClientSendInfoExtend& send_info)
	{
		// 赋值请求参数
		HttpRequest http_request;
		// 方法
		http_request.http_method = send_info.str_method;
		// 请求url
		//std::string str_isapi = send_info.str_isapi + "?SID=" + send_info.strSessionID + "&" + send_info.str_request;

        string access_type = send_info.str_access_type.empty() ? str_protocol_type_ : send_info.str_access_type;

        http_request.http_url = access_type + "://" + send_info.strCMSIp + ":" + Base::StringUtil::T2String(send_info.uCMSPort) + send_info.str_isapi;

        //IVMS_LOG_INFO("CommLayer", "[CNetlibWrapper::ClientSynSendForDevice] http_request url %s requst method :%s", http_request.http_url.c_str(), http_request.http_method.c_str());

		if (send_info.client_send_auto.active)
		{
			http_request.client_auth.active = true;
			http_request.client_auth.cnonce = send_info.client_send_auto.cnonce;
			http_request.client_auth.nc = send_info.client_send_auto.nc;
			http_request.client_auth.nonce = send_info.client_send_auto.nonce;
			http_request.client_auth.qop = send_info.client_send_auto.qop;
			http_request.client_auth.realm = send_info.client_send_auto.realm;
			http_request.client_auth.uri = send_info.client_send_auto.uri;
			http_request.client_auth.username = send_info.client_send_auto.userName;
			http_request.client_auth.response = send_info.client_send_auto.response;
          /*  IVMS_LOG_INFO("CommLayer", "[CNetlibWrapper::ClientSynSendForDevice] http_request auth info active：%d,cnonce:%s, nc:%s, nonce:%s qop:%s, realm:%s, uri:%s, username:%s,response:%s",
                http_request.client_auth.active, http_request.client_auth.cnonce.c_str(),
                http_request.client_auth.nc.c_str(), http_request.client_auth.nonce.c_str(),
                http_request.client_auth.qop.c_str(), http_request.client_auth.realm.c_str(),
                http_request.client_auth.uri.c_str(), http_request.client_auth.username.c_str(),
                http_request.client_auth.response.c_str());*/
		}

		// 发送数据
		http_request.content_type = send_info.content_type;
		if (!send_info.str_send_data.empty())
		{
			http_request.send_size = send_info.str_send_data.size();
			http_request.send_buff = const_cast<char*>(send_info.str_send_data.c_str());
			// IVMS_LOG_INFO("CommLayer", "[CNetlibWrapper::ClientSynSendForDevice] http_request str_send_data %s", send_info.str_send_data.c_str());
		}
		else
		{
			http_request.send_size = send_info.image_size;
			http_request.send_buff = send_info.ptr_image_buffer;
		}

		// 网络情况设置放大因子
		http_request.recv_timeout = send_info.time_out * time_out_fator_;
		http_request.connect_tiemout *= time_out_fator_;
		http_request.send_timeout *= time_out_fator_;

		// 当前连接句柄,返回值
		LONG client_handle = -1;
		// 客户端接收数据结构体,返回值
		HttpClientRecv recv_data;

		// 客户端发送函数,接收的数据处理完成后,需要关闭当前连接
		LONG ret_value = CVsmNetLib::HttpClientSynSend(client_handle, recv_data, http_request);
		if (NET_ERR_NONE == ret_value)
		{
			// 解析收到数据,首先判断http错误码
			if (recv_data.http_code == 200 || recv_data.http_code == 401)
			{
				// 比较http协议头的数据长度和收到的数据长度是否一致,说明数据接收完成
				if ((recv_data.content_len == recv_data.recv_size) && (recv_data.recv_size != 0))
				{
					receive_info.receive_data.resize(recv_data.recv_size);
					memcpy(const_cast<char*>(receive_info.receive_data.c_str()), recv_data.recv_data, recv_data.recv_size);
					if (!receive_info.receive_data.empty())
					{
						if (string::npos != receive_info.receive_data.find("xmlns"))
						{
							//string s = Base::StringUtil::StringReplace(receive_info.receive_data, "xmlns", "xmln");
							receive_info.receive_data = Base::StringUtil::StringReplace(receive_info.receive_data, "xmlns", "xmln");
						}
						unique_ptr<CXmlProto> pro_req(new CXmlProto());
						pro_req->set_str(receive_info.receive_data);
						receive_info.error_code_.iFirstCode = pro_req->get_int_v("/ResponseStatus/ErrorModule");
						receive_info.error_code_.iSecondCode = pro_req->get_int_v("/ResponseStatus/ErrorCode");
						receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_VSM);
						receive_info.error_code_.SetCasadingErrorInfo(pro_req->get_v("/ResponseStatus/CascadingErrorInfo"));
					}

					if (recv_data.http_code == 401)
					{
						receive_info.bool_auth = true;
						receive_info.digest = recv_data.client_auth.digest;
						receive_info.realm = recv_data.client_auth.realm;
						receive_info.nonce = recv_data.client_auth.nonce;
						receive_info.qop = recv_data.client_auth.qop;
					}
				}
				else
				{
					// 接收数据长度错误
					assert(0);

					receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
					receive_info.error_code_.iFirstCode = IVMS_COMPONENT_LOCAL;
					receive_info.error_code_.iSecondCode = IVMS_INVALID_DATA;
					// 关闭连接
					CVsmNetLib::HttpClostClientLink(client_handle);

					return false;
				}
			}
			else
			{
				// http协议错误
				assert(0);

				receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
				receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
				receive_info.error_code_.iSecondCode = recv_data.http_code;


				// 关闭连接
				CVsmNetLib::HttpClostClientLink(client_handle);

				return false;
			}
		}
		else
		{
			// 接收数据失败
			receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
			receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
            receive_info.error_code_.iSecondCode = ret_value;
			//IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSendForDevice] failed, [errorcode: %d],[request: %s]", ret_value, http_request.http_url.c_str());
			// 关闭连接
			CVsmNetLib::HttpClostClientLink(client_handle);

			return false;
		}

		// 数据处理完成后,需要关闭连接
		CVsmNetLib::HttpClostClientLink(client_handle);

		return true;
	
	}

    LONG CNetlibWrapper::WebsocketClientSynCreat(LONG& client_handle, const string& str_server_ip, const unsigned int& uPort, const Fun_WebSocketCallBackEx client_callback)
    {
        //转换网络接口数据
        WebsocketClientParam client_param;
        //http://127.0.0.1:80/websocket
		//client_param.url = str_protocol_type_ + "://" + str_server_ip + ":" + Base::StringUtil::T2String(uPort) + "/websocket";
		client_param.url = str_protocol_type_ +"://" + str_server_ip + ":" + Base::StringUtil::T2String(uPort) + "/websocket";

        //CVsmNetLib::StartSdkLog(3, false, "c://SDK//");
        return  CVsmNetLib::WebsocketClientSynCreat(client_handle, client_param, client_callback);
    }

    LONG CNetlibWrapper::WebsocketClientStart(const LONG client_handle)
    {
        return  CVsmNetLib::WebsocketClientStart(client_handle);
    }

    LONG CNetlibWrapper::WebsocketClientStop(const LONG client_handle)
    {
        return CVsmNetLib::WebsocketClientStop(client_handle);
    }

    // 发送
    LONG CNetlibWrapper::WebsocketClientSend(LONG client_handle, WebsocketMsg msg_type, char* send_data, DWORD data_len, bool is_fin)
    {
        //cs 只发text
        WebsocketMsgtype web_msg_type = WebsocketMsgtype::text_msg;
        switch (msg_type)
        {
        case WebsocketMsg::continuation_msg:
            web_msg_type = WebsocketMsgtype::continuation_msg;
            break;
        case WebsocketMsg::text_msg:
            web_msg_type = WebsocketMsgtype::text_msg;
            break;
        case WebsocketMsg::binary_mgs:
            web_msg_type = WebsocketMsgtype::binary_mgs;
            break;
        case WebsocketMsg::close_msg:
            web_msg_type = WebsocketMsgtype::close_msg;
            break;
        case WebsocketMsg::ping_msg:
            web_msg_type = WebsocketMsgtype::ping_msg;
            break;
        case WebsocketMsg::pong_msg:
            web_msg_type = WebsocketMsgtype::pong_msg;
            break;
        case WebsocketMsg::establish_msg:
            web_msg_type = WebsocketMsgtype::establish_msg;
            break;
        case WebsocketMsg::handshake_reg_msg:
            web_msg_type = WebsocketMsgtype::handshake_reg_msg;
            break;
        case WebsocketMsg::reconn_start:
            web_msg_type = WebsocketMsgtype::reconn_start;
            break;//重连开始
        case WebsocketMsg::reconn_succ:
            web_msg_type = WebsocketMsgtype::reconn_succ;
            break;
        default:
            web_msg_type = WebsocketMsgtype::text_msg;
            break;
        }

        return CVsmNetLib::WebsocketClientSend(client_handle, web_msg_type, send_data, data_len, is_fin);
    }

	void CNetlibWrapper::SetTimeoutFacotor(clientframework::SystemConfig::NetOverTime enum_net_over_time)
    {
		switch (enum_net_over_time)
        {
		case clientframework::SystemConfig::NetOverTime::DefaultOvetTime:
            time_out_fator_ = 1;
            break;
		case clientframework::SystemConfig::NetOverTime::OneAndHalfTimes:
            time_out_fator_ = 1.5;
            break;
		case clientframework::SystemConfig::NetOverTime::DoubleTimes:
            time_out_fator_ = 2.0;
            break;
        default:
            assert(0);
			time_out_fator_ = 1;
            break;
        }
		IVMS_LOG_INFO("CommLayer", "[CNetlibWrapper::SetTimeoutFacotor] factor: %d", enum_net_over_time);
    }

	clientframework::VsmVersion CNetlibWrapper::GetVsmVersion()
	{
		return vsm_version_;
	}

	bool CNetlibWrapper::ClientSynSendForRemoteSite(ClientRecvInfo& receive_info, const ClientSendInfo& send_info, const std::string& session, const std::string& security_version, const string& token_secret_key, const IConnectServerInfor* ptr_server_infor)
	{
		if (session.empty() || security_version.empty())
		{
			receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
			receive_info.error_code_.iFirstCode = IVMS_COMPONENT_LOCAL;
			receive_info.error_code_.iSecondCode = IVMS_INPUT_PARAMETERS_ERROR;
			IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSendForExpress] input paramerters error");
			return false;
		}

		// 赋值请求参数
		HttpRequest http_request;
		// 方法
		http_request.http_method = send_info.str_method;

        string str_isapi = send_info.str_isapi;
        // 方法
        if (send_info.str_method == "GET" && send_info.request_pattern != binary_request)
        {
            http_request.http_method = "POST";
            int nSize = str_isapi.find("?");
            if (nSize == -1)
            {
                receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
                receive_info.error_code_.iFirstCode = IVMS_COMPONENT_LOCAL;
                receive_info.error_code_.iSecondCode = IVMS_INPUT_PARAMETERS_ERROR;
                return false;
            }
            else
            {
                str_isapi.insert(nSize + 1, "MT=GET&");
                http_request.send_size = send_info.str_request.size();
                http_request.send_buff = const_cast<char*>(send_info.str_request.c_str());
            }
        }
        else if (send_info.str_method == "POST" && send_info.request_pattern == GetPatch)
        {
            //虽然也使用了post方法(有部分是4.0版本为了解决get方法url过长截断问题修改的)，但实质是get行为，故也在Query中增加MT=GET
            int nSize = str_isapi.find("?");
            if (nSize == -1)
            {
                receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
                receive_info.error_code_.iFirstCode = IVMS_COMPONENT_LOCAL;
                receive_info.error_code_.iSecondCode = IVMS_INPUT_PARAMETERS_ERROR;
                return false;
            }
            else
            {
                str_isapi.insert(nSize + 1, "MT=GET&");
            }

            http_request.http_method = send_info.str_method;
        }
        else
            http_request.http_method = send_info.str_method;


        //isapi协议
        //string str_isapi;
        if (!send_info.str_request.empty())
        {
            if (send_info.str_method == "GET" && send_info.request_pattern == binary_request)
            {
                str_isapi += "&" + send_info.str_request;
            }
            else
            {
				//HC1.4版本删除data绑定，1.3版本系统测试后修改，所以不敢直接删除
				if (send_info.str_request.size() < 1400)
				{
					str_isapi += "&request_parameter_xml=" + send_info.str_request;
				}
				else
				{
					IVMS_LOG_INFO(LOG_COMM_LAYER, "Url size > 1400.");
				}
                
            }
        }

		//string str_isapi = send_info.str_isapi;
		// 请求url
		//if (!send_info.str_request.empty())
		//{
		//	////新的XML实现出来是<?xml version="1.0" encoding="UTF-8"?>\n<DeviceStatusRequest>。。。</DeviceStatusRequest >\n
		//	////多出了前面一段<?xml version="1.0" encoding="UTF-8"?>,但是这里的UTF-8可能会变。
		//	string str_final_request = send_info.str_request;
		//	string head = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
		//	if (send_info.str_request.find(head) != string::npos)
		//	{
		//		str_final_request = send_info.str_request.substr(head.size());
		//		str_final_request = str_final_request.substr(0, str_final_request.size() - 1);
		//	}
		//	str_isapi += "&request_parameter_xml=" + str_final_request;
		//}

		string protocal_type = send_info.str_access_type.empty() ? "http" : send_info.str_access_type;
		http_request.http_url = protocal_type + "://" + send_info.str_server_ip + ":" + Base::StringUtil::T2String(send_info.u_port) + str_isapi;

	//	IVMS_LOG_INFO("CommLayer", "[CNetlibWrapper::ClientSynSendForExpress] http_request url %s requst method :%s", http_request.http_url.c_str(), http_request.http_method.c_str());

		// token 需判断返回值 
		ptr_server_infor->CreateTokenForExpress(http_request.token_value, session, security_version, token_secret_key);
		// 发送数据
		if (!send_info.str_send_data.empty())
		{
			http_request.send_size = send_info.str_send_data.size();
			http_request.send_buff = const_cast<char*>(send_info.str_send_data.c_str());
		//	IVMS_LOG_INFO("CommLayer", "[CNetlibWrapper::ClientSynSendForExpress] http_request str_send_data %s", send_info.str_send_data.c_str());
		}

		// 网络情况设置放大因子
		http_request.recv_timeout = send_info.time_out * time_out_fator_;
		http_request.connect_tiemout *= time_out_fator_;
		http_request.send_timeout *= time_out_fator_;

		// 当前连接句柄,返回值
		LONG client_handle = -1;
		// 客户端接收数据结构体,返回值
		HttpClientRecv recv_data;

		// 客户端发送函数,接收的数据处理完成后,需要关闭当前连接
		LONG ret_value = CVsmNetLib::HttpClientSynSend(client_handle, recv_data, http_request);

		// session用完就释放
		ptr_server_infor->ReleaseTokenFroExpress(receive_info.error_code_.iFirstCode, receive_info.error_code_.iSecondCode, session);

		if (NET_ERR_NONE == ret_value)
		{
			// 解析收到数据,首先判断http错误码
			if (recv_data.http_code == 200)
			{
				// 比较http协议头的数据长度和收到的数据长度是否一致,说明数据接收完成
				if ((recv_data.content_len == recv_data.recv_size) && (recv_data.recv_size != 0))
				{
					receive_info.receive_data.resize(recv_data.recv_size);
					memcpy(const_cast<char*>(receive_info.receive_data.c_str()), recv_data.recv_data, recv_data.recv_size);
                    if (!receive_info.receive_data.empty() && send_info.request_pattern != binary_request)
					{
						// express返回错误码，客户端进行调整
						// 子系统：客户端
						// 错误模块：Express
						// 错误码：50
						// 级联信息：Express返回的错误码
						unique_ptr<CXmlProto> pro_req(new CXmlProto());
						pro_req->set_str(receive_info.receive_data);
// 						receive_info.error_code_.iFirstCode = IVMS_COMPONENT_RemoteSite;
// 						receive_info.error_code_.iSecondCode = IVMS_CASCADING_ERROR_CODE;
// 						receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
// 						receive_info.error_code_.SetCasadingErrorInfo(Base::Error::MakeCascadingErrorStr(pro_req->get_int_v("/ResponseStatus/ErrorModule"), pro_req->get_int_v("/ResponseStatus/ErrorCode")));
// 						if (CMS_MODULE_CMS == receive_info.error_code_.iFirstCode && CMS_SESSIONID_INVALID == receive_info.error_code_.iSecondCode)
// 						{
// 							// 无效Seeeion，释放Token
// 							ptr_server_infor->SercurityRelease(receive_info.error_code_.iFirstCode, receive_info.error_code_.iSecondCode, session);
// 						}

						if (pro_req->get_int_v("/ResponseStatus/ErrorCode") != 0)
						{
							receive_info.error_code_.iFirstCode = IVMS_COMPONENT_RemoteSite;
							receive_info.error_code_.iSecondCode = IVMS_CASCADING_ERROR_CODE;
							receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
							receive_info.error_code_.SetCasadingErrorInfo(Base::Error::MakeCascadingErrorStr(pro_req->get_int_v("/ResponseStatus/ErrorModule"), pro_req->get_int_v("/ResponseStatus/ErrorCode")));
							return false;
							// session无效的200码需要特殊处理
// 							if (CMS_SESSIONID_INVALID == pro_req->get_int_v("/ResponseStatus/ErrorCode"))
// 							{
// 								IVMS_LOG_WARN("CommLayer", "[CNetlibWrapper::ClientSynSendForExpress] http_url:%s requeset CMS_SESSIONID_INVALID session:%s, release token:%s", http_request.http_url.c_str(), session.c_str(), http_request.token_value.c_str());
// 								// 无效Seeeion，释放Token
// 								ptr_server_infor->ReleaseTokenFroExpress(receive_info.error_code_.iFirstCode, receive_info.error_code_.iSecondCode, session);
// 								return false;
// 							}
// 							else
// 							{
// 								return false;
// 							}
						}
					}
				}
				else
				{
					// 接收数据长度错误
					assert(0);

					receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
					receive_info.error_code_.iFirstCode = IVMS_COMPONENT_LOCAL;
					receive_info.error_code_.iSecondCode = IVMS_INVALID_DATA;
					// 关闭连接
					CVsmNetLib::HttpClostClientLink(client_handle);

					return false;
				}
			}
			else
			{
				// http协议错误
				assert(0);

				receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
				receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
				receive_info.error_code_.iSecondCode = recv_data.http_code;

				// 关闭连接
				CVsmNetLib::HttpClostClientLink(client_handle);

				return false;
			}
		}
		else
		{
			// 接收数据失败
			receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
			receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
			receive_info.error_code_.iSecondCode = ret_value;
		//	IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSendForExpress] failed, [errorcode: %d],[request: %s]", ret_value, http_request.http_url.c_str());
			// 关闭连接
			CVsmNetLib::HttpClostClientLink(client_handle);

			return false;
		}

		// 数据处理完成后,需要关闭连接
		CVsmNetLib::HttpClostClientLink(client_handle);

		return true;
	}

}
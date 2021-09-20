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

        // ���ַ���ת��Ϊ16����
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

    // ���캯��
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

    // ��ʼ��
    bool CNetlibWrapper::Initialize()
    {
        if (is_initialized_)
        {
            return true;
        }

		
        // ��ʼ�������
        is_initialized_ = CVsmNetLib::VsmNetLibInit();
        if (!is_initialized_)
        {
            assert(0); // �����Ŀͻ��˳�ʼ��ʧ��
            IVMS_LOG_ERROR("ClientWarningAndMore", "CVsmNetLib::VsmNetLibInit failed");
        }

		//CVsmNetLib::StartSdkLog(3, true, "c:/SdkUtilsLog/");

        return is_initialized_;
    }

    // ����ʼ��
    bool CNetlibWrapper::Uninitialize()
    {
        if (!is_initialized_)
        {
            return true;
        }

        // ����ʼ�������
        return CVsmNetLib::VsmNetLibUnInit();
    }

	bool CNetlibWrapper::ClientSynSendForVsmVersion(ClientRecvInfo& receive_info, const string str_server_ip, unsigned int uPort, const string login_protocol)
	{
		string str_isapi;
		HttpRequest http_request;
		// ����		
		http_request.http_method = "GET";
		str_isapi = "/ISAPI/Bumblebee/Version";
		http_request.http_url = login_protocol + "://" + str_server_ip + ":" + Base::StringUtil::T2String(uPort) + str_isapi;
		// ��ǰ���Ӿ��,����ֵ
		LONG client_handle = -1;
		// �ͻ��˽������ݽṹ��,����ֵ
		HttpClientRecv recv_data;
		// �ͻ��˷��ͺ���,���յ����ݴ�����ɺ�,��Ҫ�رյ�ǰ����
		LONG ret_value = CVsmNetLib::HttpClientSynSend(client_handle, recv_data, http_request);
		//ClientRecvInfo receive_info;
		if (NET_ERR_NONE == ret_value)
		{
			// �����յ�����,�����ж�http������
			if (recv_data.http_code == 200)
			{
				// �Ƚ�httpЭ��ͷ�����ݳ��Ⱥ��յ������ݳ����Ƿ�һ��,˵�����ݽ������
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

						//V1.0.0.0  ���һλ��build�� ���Ƚ�
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
					// �������ݳ��ȴ���
					assert(0);

					receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
					receive_info.error_code_.iFirstCode = IVMS_COMPONENT_LOCAL;
					receive_info.error_code_.iSecondCode = IVMS_INVALID_XML_CONTENT;

					IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSendForVsmVersion] failed, [errorcode: %d],[request: %s]", IVMS_INVALID_XML_CONTENT, http_request.http_url.c_str());
					// �ر�����
					CVsmNetLib::HttpClostClientLink(client_handle);

					return false;
				}
			}
			else
			{
				// httpЭ�����
				assert(0);
				receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
				receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
				receive_info.error_code_.iSecondCode = recv_data.http_code;
				IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSendForVsmVersion] failed, [errorcode: %d],[request: %s]", recv_data.http_code, http_request.http_url.c_str());

				// �ر�����
				CVsmNetLib::HttpClostClientLink(client_handle);

				return false;
			}
		}
		else
		{
			// ��������ʧ��
			receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
			receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
			receive_info.error_code_.iSecondCode = ret_value;
			IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSendForVsmVersion] failed, [errorcode: %d],[request: %s]", ret_value, http_request.http_url.c_str());

			// �ر�����
			CVsmNetLib::HttpClostClientLink(client_handle);

			return false;
		}

		// ���ݴ�����ɺ�,��Ҫ�ر�����
		CVsmNetLib::HttpClostClientLink(client_handle);

		return true;
	}

    bool CNetlibWrapper::ClientSynSendForLogin(ClientRecvInfo& receive_info, const ClientSendInfo& send_info, const string str_server_ip, unsigned int uPort)
    {
        // ��ֵ�������
		string str_isapi  = send_info.str_isapi;
        HttpRequest http_request;
		// ����
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


			//��ͨ��get�ĳ�post֮��http_request.send_buff����Ϊ�գ����븳ֵ

			http_request.send_size = send_info.str_request.size();
			http_request.send_buff = const_cast<char*>(send_info.str_request.c_str());
			IVMS_LOG_INFO("CommLayer", "[CNetlibWrapper::ClientSynSendForLogin] http_request send_buff %s", send_info.str_request.c_str());

		}
		else if (send_info.str_method == "POST" && send_info.request_pattern == GetPatch)
		{
			//��ȻҲʹ����post����(�в�����4.0�汾Ϊ�˽��get����url�����ض������޸ĵ�)����ʵ����get��Ϊ����Ҳ��Query������MT=GET
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


        // ����url
        //eg: http://10.17.133.26:80/ISAPI/PTZCtrl/channels/1/patrols/capabilities //str_protocol_type_
		http_request.http_url = str_protocol_type_ + "://" + str_server_ip + ":" + Base::StringUtil::T2String(uPort) + str_isapi;

        IVMS_LOG_INFO("CommLayer", "[CNetlibWrapper::ClientSynSendForLogin] http_request url %s requst method :%s", http_request.http_url.c_str(), http_request.http_method.c_str());

        // ��������
        if (!send_info.str_send_data.empty())
        {
            http_request.send_size = send_info.str_send_data.size();
            http_request.send_buff = const_cast<char*>(send_info.str_send_data.c_str());
            //IVMS_LOG_INFO("CommLayer", "[CNetlibWrapper::ClientSynSendForLogin] http_request str_send_data %s", send_info.str_send_data.c_str());
        }


        // ��ǰ���Ӿ��,����ֵ
        LONG client_handle = -1;
        // �ͻ��˽������ݽṹ��,����ֵ
        HttpClientRecv recv_data;

		http_request.recv_timeout = send_info.time_out;
        
        // �ͻ��˷��ͺ���,���յ����ݴ�����ɺ�,��Ҫ�رյ�ǰ����
        LONG ret_value = CVsmNetLib::HttpClientSynSend(client_handle, recv_data, http_request);
        if (NET_ERR_NONE == ret_value)
        {
            // �����յ�����,�����ж�http������
            if (recv_data.http_code == 200)
            {
                // �Ƚ�httpЭ��ͷ�����ݳ��Ⱥ��յ������ݳ����Ƿ�һ��,˵�����ݽ������
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

						//����ĿǰIP ���ɴ�м������豸�������ݵ�����������ݷ�VSM ���ظ�ʽ
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
                    // �������ݳ��ȴ���
                    assert(0);

                    receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
					receive_info.error_code_.iFirstCode = IVMS_COMPONENT_LOCAL;
                    receive_info.error_code_.iSecondCode = IVMS_INVALID_XML_CONTENT;

                    IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSendForLogin] failed, [errorcode: %d],[request: %s]", IVMS_INVALID_XML_CONTENT, http_request.http_url.c_str());
                    // �ر�����
                    CVsmNetLib::HttpClostClientLink(client_handle);

                    return false;
                }
            }
            else
            {
                // httpЭ�����
				assert(0);

                receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
				receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
                receive_info.error_code_.iSecondCode = recv_data.http_code;
                IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSendForLogin] failed, [errorcode: %d],[request: %s]", recv_data.http_code, http_request.http_url.c_str());

                // �ر�����
                CVsmNetLib::HttpClostClientLink(client_handle);

                return false;
            }
        }
        else
        {
            // ��������ʧ��
			receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
			receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
            receive_info.error_code_.iSecondCode = ret_value;
            IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSendForLogin] failed, [errorcode: %d],[request: %s]", ret_value, http_request.http_url.c_str());

            // �ر�����
            CVsmNetLib::HttpClostClientLink(client_handle);

            return false;
        }

        // ���ݴ�����ɺ�,��Ҫ�ر�����
        CVsmNetLib::HttpClostClientLink(client_handle);

        return true;
    }

    // �ͻ���ͬ�����ͽӿ�
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
        //�ⲿ�������Դ����Ϊ׼
        if (!send_info.str_server_ip.empty())
        {
            strCMSIP = send_info.str_server_ip;
        }

        if (send_info.u_port > 0)
        {
            uPort = send_info.u_port;
        }

        // ��ֵ�������
        HttpRequest http_request;

		// ����
		if (send_info.str_method == "GET" && send_info.request_pattern != binary_request)
		{
			http_request.http_method = "POST";

			strSessionIDEx += "&MT=GET";

			//��ͨ��get�ĳ�post֮��http_request.send_buff����Ϊ�գ����븳ֵ

			http_request.send_size = send_info.str_request.size();
			http_request.send_buff = const_cast<char*>(send_info.str_request.c_str());
			IVMS_LOG_INFO("CommLayer", "[CNetlibWrapper::ClientSynSend] http_request send_buff %s", send_info.str_request.c_str());
		}
		else if (send_info.str_method == "POST" && send_info.request_pattern == GetPatch)
		{
			//��ȻҲʹ����post����(�в�����4.0�汾Ϊ�˽��get����url�����ض������޸ĵ�)����ʵ����get��Ϊ����Ҳ��Query������MT=GET
			strSessionIDEx += "&MT=GET";
			http_request.http_method = send_info.str_method;
		}
		else
			http_request.http_method = send_info.str_method;

		//isapiЭ��
		string str_isapi;
		if (!send_info.str_request.empty())
		{
			if (send_info.str_method == "GET" && send_info.request_pattern == binary_request)
			{
				str_isapi = send_info.str_isapi + "?SID=" + strSessionIDEx + "&" + send_info.str_request;
			}
			else
			{
				//HC1.4�汾ɾ��data�󶨣�1.3�汾ϵͳ���Ժ��޸ģ����Բ���ֱ��ɾ��
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

        // ����url
        //string str_isapi = send_info.str_isapi + "?SID=" + strSessionID + "&" + send_info.str_request;//str_protocol_type_
		http_request.http_url = str_protocol_type_ + "://" + strCMSIP + ":" + Base::StringUtil::T2String(uPort) + str_isapi;

        IVMS_LOG_INFO("CommLayer", "[CNetlibWrapper::ClientSynSend] http_request url %s requst method :%s", http_request.http_url.c_str(), http_request.http_method.c_str());

        // token
		if (!ptr_server_infor->GetToken(strSessionID, http_request.token_value))
        {
            assert(0);
			receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
            receive_info.error_code_.iFirstCode = IVMS_COMPONENT_Security;
			// receive_info.error_code_.iSecondCode = ; //Ŀǰû�з��ش����룡��
            IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSend] CSecurityManager::Instance().GetToken failure");
            return false;
        }

        // ��������  ԭ��get�����ض�Ϊ�� �����ǰ���ͻ��
        if (!send_info.str_send_data.empty())
        {
            http_request.send_size = send_info.str_send_data.size();
            http_request.send_buff = const_cast<char*>(send_info.str_send_data.c_str());			
            //IVMS_LOG_INFO("CommLayer", "[CNetlibWrapper::ClientSynSendForLogin] http_request str_send_data %s", send_info.str_send_data.c_str());
        }

        http_request.recv_timeout = send_info.time_out * time_out_fator_;
        http_request.connect_tiemout *= time_out_fator_;
        http_request.send_timeout *= time_out_fator_;

        // ��ǰ���Ӿ��,����ֵ
        LONG client_handle = -1;
        // �ͻ��˽������ݽṹ��,����ֵ
        HttpClientRecv recv_data;

        // �ͻ��˷��ͺ���,���յ����ݴ�����ɺ�,��Ҫ�رյ�ǰ����
        LONG ret_value = CVsmNetLib::HttpClientSynSend(client_handle, recv_data, http_request);
        if (NET_ERR_NONE == ret_value)
        {
            // �����յ�����,�����ж�http������
            if (recv_data.http_code == 200)
            {
                // �Ƚ�httpЭ��ͷ�����ݳ��Ⱥ��յ������ݳ����Ƿ�һ��,˵�����ݽ������
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
							//��������ӡ��־
							IVMS_LOG_WARN("CommLayer", "[CNetlibWrapper::ClientSynSend] http_url:%s requeset CMS_SESSIONID_INVALID session:%s, release token:%s", http_request.http_url.c_str(), strSessionID.c_str(), http_request.token_value.c_str());
							// ��ЧSeeeion���ͷ�Token
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
                    // �������ݳ��ȴ���
                    assert(0);

					receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
					receive_info.error_code_.iFirstCode = IVMS_COMPONENT_LOCAL;
                    receive_info.error_code_.iSecondCode = IVMS_INVALID_XML_CONTENT;
                    // �ر�����
                    CVsmNetLib::HttpClostClientLink(client_handle);

                    return false;
                }
            }
            else
            {
                // httpЭ�����
				assert(0);

                receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
				receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
                receive_info.error_code_.iSecondCode = recv_data.http_code;
                IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSend] failed, [errorcode: %d],[request: %s]", recv_data.http_code, http_request.http_url.c_str());

                // �ر�����
                CVsmNetLib::HttpClostClientLink(client_handle);

                return false;
            }
        }
        else
        {
			// ��������ʧ��
            receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
			receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
            receive_info.error_code_.iSecondCode = ret_value;
			IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSend] failed, [errorcode: %d],[request: %s]", ret_value, http_request.http_url.c_str());
            // �ر�����
            CVsmNetLib::HttpClostClientLink(client_handle);

            return false;
        }

        // ���ݴ�����ɺ�,��Ҫ�ر�����
        CVsmNetLib::HttpClostClientLink(client_handle);

        return true;
    }

	// �ͻ���ͬ�����ͽӿ�
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
		// ��ֵ�������
		HttpRequest http_request;
		string str_isapi = send_info.str_isapi;
		// ����
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

			//��ͨ��get�ĳ�post֮��http_request.send_buff����Ϊ�գ����븳ֵ

			http_request.send_size = send_info.str_request.size();
			http_request.send_buff = const_cast<char*>(send_info.str_request.c_str());
		}
		else if (send_info.str_method == "POST" && send_info.request_pattern == GetPatch)
		{
			//��ȻҲʹ����post����(�в�����4.0�汾Ϊ�˽��get����url�����ض������޸ĵ�)����ʵ����get��Ϊ����Ҳ��Query������MT=GET
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
		// ����url
// 		string str_final_request = "";
// 		if (!send_info.str_isapi.empty())
// 		{
// 			//str_isapi = send_info.str_isapi + "?SID=" + strSessionID + "&request_parameter_xml=" + send_info.str_request;
// 			////�µ�XMLʵ�ֳ�����<?xml version="1.0" encoding="UTF-8"?>\n<DeviceStatusRequest>������</DeviceStatusRequest >\n
// 			////�����ǰ��һ��<?xml version="1.0" encoding="UTF-8"?>,���������UTF-8���ܻ�䡣
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
			//��vsmժҪ��֤
			if (!ptr_server_infor->GetToken(send_info.strSessionID, http_request.token_value))
			{
				//token ����ʧ��
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
		// ��������
		if (!send_info.str_send_data.empty())
		{
			http_request.send_size = send_info.str_send_data.size();
			http_request.send_buff = const_cast<char*>(send_info.str_send_data.c_str());
		}

		// ��ǰ���Ӿ��,����ֵ
		LONG client_handle = -1;
		// �ͻ��˽������ݽṹ��,����ֵ
		HttpClientRecv recv_data;

		// �ͻ��˷��ͺ���,���յ����ݴ�����ɺ�,��Ҫ�رյ�ǰ����
		LONG ret_value = CVsmNetLib::HttpClientSynSend(client_handle, recv_data, http_request);
		if (NET_ERR_NONE == ret_value)
		{
			// �����յ�����,�����ж�http������
			if (recv_data.http_code == 200 || recv_data.http_code == 401)
			{
				// �Ƚ�httpЭ��ͷ�����ݳ��Ⱥ��յ������ݳ����Ƿ�һ��,˵�����ݽ������
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
                            // ��ЧSeeeion���ͷ�Token
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
					// �������ݳ��ȴ���
					assert(0);

					receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
					receive_info.error_code_.iFirstCode = IVMS_COMPONENT_LOCAL;
					receive_info.error_code_.iSecondCode = IVMS_INVALID_XML_CONTENT;
					// �ر�����
					CVsmNetLib::HttpClostClientLink(client_handle);

					return false;
				}
			}
			else
			{
				// httpЭ�����
				assert(0);

				// TODO ������
				receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
				receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
				receive_info.error_code_.iSecondCode = recv_data.http_code;
				IVMS_LOG_ERROR("CommLayer", "[CPlaybackNetlibWrapper::ClientSynSend] failed, [errorcode: %d],[request: %s]", recv_data.http_code, http_request.http_url.c_str());
				// �ر�����
				CVsmNetLib::HttpClostClientLink(client_handle);

				return false;
			}
		}
		else
		{
			// ��������ʧ��
			receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
			receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
			receive_info.error_code_.iSecondCode = ret_value;

			IVMS_LOG_ERROR("CommLayer", "[CPlaybackNetlibWrapper::ClientSynSend] failed, [errorcode: %d],[request: %s]", ret_value, http_request.http_url.c_str());
			// �ر�����
			CVsmNetLib::HttpClostClientLink(client_handle);

			return false;
		}

		// ���ݴ�����ɺ�,��Ҫ�ر�����
		CVsmNetLib::HttpClostClientLink(client_handle);

		return true;
	}

	//ֱ����express���޸�Get����
	bool CNetlibWrapper::ClientSynSendForExpress(ClientRecvInfo& receive_info, const ClientSendInfo& send_info, const string& ip, const int port, const string& session, const string& security_version, const string& token_secret_key, IConnectServerInfor* ptr_server_infor)
	{
		// 2017-6-27 HikCentral1.1������Express��ע�ʹ˽ӿ� by yaojz
		return false;
		if (ip.empty() || session.empty() || security_version.empty())
		{
			receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
			receive_info.error_code_.iFirstCode = IVMS_COMPONENT_LOCAL;
			receive_info.error_code_.iSecondCode = IVMS_INPUT_PARAMETERS_ERROR;
            IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSendForExpress] input paramerters error");
			return false;
		}		

        // ��ֵ�������
        HttpRequest http_request;
        // ����
        http_request.http_method = send_info.str_method;

		string str_isapi = send_info.str_isapi;
        // ����url
		if (!send_info.str_request.empty())
		{
			////�µ�XMLʵ�ֳ�����<?xml version="1.0" encoding="UTF-8"?>\n<DeviceStatusRequest>������</DeviceStatusRequest >\n
			////�����ǰ��һ��<?xml version="1.0" encoding="UTF-8"?>,���������UTF-8���ܻ�䡣
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

        // token ���жϷ���ֵ 
		ptr_server_infor->CreateTokenForExpress(http_request.token_value, session, security_version, token_secret_key);
        // ��������
        if (!send_info.str_send_data.empty())
        {
            http_request.send_size = send_info.str_send_data.size();
            http_request.send_buff = const_cast<char*>(send_info.str_send_data.c_str());
            //IVMS_LOG_INFO("CommLayer", "[CNetlibWrapper::ClientSynSendForExpress] http_request str_send_data %s", send_info.str_send_data.c_str());
        }

		// ����������÷Ŵ�����
		http_request.recv_timeout = send_info.time_out * time_out_fator_;
		http_request.connect_tiemout *= time_out_fator_;
		http_request.send_timeout *= time_out_fator_;

        // ��ǰ���Ӿ��,����ֵ
        LONG client_handle = -1;
        // �ͻ��˽������ݽṹ��,����ֵ
        HttpClientRecv recv_data;

        // �ͻ��˷��ͺ���,���յ����ݴ�����ɺ�,��Ҫ�رյ�ǰ����
        LONG ret_value = CVsmNetLib::HttpClientSynSend(client_handle, recv_data, http_request);
        if (NET_ERR_NONE == ret_value)
        {
            // �����յ�����,�����ж�http������
            if (recv_data.http_code == 200)
            {
                // �Ƚ�httpЭ��ͷ�����ݳ��Ⱥ��յ������ݳ����Ƿ�һ��,˵�����ݽ������
                if ((recv_data.content_len == recv_data.recv_size) && (recv_data.recv_size != 0))
                {
                    receive_info.receive_data.resize(recv_data.recv_size);
                    memcpy(const_cast<char*>(receive_info.receive_data.c_str()), recv_data.recv_data, recv_data.recv_size);
                    if (!receive_info.receive_data.empty())
                    {
						// express���ش����룬�ͻ��˽��е���
						// ��ϵͳ���ͻ���
						// ����ģ�飺Express
						// �����룺50
						// ������Ϣ��Express���صĴ�����
						unique_ptr<CXmlProto> pro_req(new CXmlProto());
						pro_req->set_str(receive_info.receive_data);
						receive_info.error_code_.iFirstCode = IVMS_COMPONENT_RemoteSite;
						receive_info.error_code_.iSecondCode = IVMS_CASCADING_ERROR_CODE;
						receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
						receive_info.error_code_.SetCasadingErrorInfo(Base::Error::MakeCascadingErrorStr(pro_req->get_int_v("/ResponseStatus/ErrorModule"), pro_req->get_int_v("/ResponseStatus/ErrorCode")));
						if (CMS_MODULE_CMS == receive_info.error_code_.iFirstCode && CMS_SESSIONID_INVALID == receive_info.error_code_.iSecondCode)
						{
							// ��ЧSeeeion���ͷ�Token
							ptr_server_infor->SercurityRelease(receive_info.error_code_.iFirstCode, receive_info.error_code_.iSecondCode, session);
						}

						if (pro_req->get_int_v("/ResponseStatus/ErrorCode") != 0)
						{
							// session��Ч��200����Ҫ���⴦��
							if (CMS_SESSIONID_INVALID == pro_req->get_int_v("/ResponseStatus/ErrorCode"))
							{
								IVMS_LOG_WARN("CommLayer", "[CNetlibWrapper::ClientSynSendForExpress] http_url:%s requeset CMS_SESSIONID_INVALID session:%s, release token:%s", http_request.http_url.c_str(), session.c_str(), http_request.token_value.c_str());
								// ��ЧSeeeion���ͷ�Token
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
                    // �������ݳ��ȴ���
					assert(0);

					receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
					receive_info.error_code_.iFirstCode = IVMS_COMPONENT_LOCAL;
					receive_info.error_code_.iSecondCode = IVMS_INVALID_DATA;
                    // �ر�����
                    CVsmNetLib::HttpClostClientLink(client_handle);

                    return false;
                }
            }
            else
            {
                // httpЭ�����
				assert(0);

				receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
				receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
                receive_info.error_code_.iSecondCode = recv_data.http_code;
			
                // �ر�����
                CVsmNetLib::HttpClostClientLink(client_handle);

                return false;
            }
        }
        else
        {
            // ��������ʧ��
			receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
			receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
			receive_info.error_code_.iSecondCode = ret_value;
			IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSendForExpress] failed, [errorcode: %d],[request: %s]", ret_value, http_request.http_url.c_str());
            // �ر�����
            CVsmNetLib::HttpClostClientLink(client_handle);

            return false;
        }

        // ���ݴ�����ɺ�,��Ҫ�ر�����
        CVsmNetLib::HttpClostClientLink(client_handle);

        return true;
	}

	bool CNetlibWrapper::ClientSynSendForDevice(ClientRecvInfoExtend& receive_info, const ClientSendInfoExtend& send_info)
	{
		// ��ֵ�������
		HttpRequest http_request;
		// ����
		http_request.http_method = send_info.str_method;
		// ����url
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
          /*  IVMS_LOG_INFO("CommLayer", "[CNetlibWrapper::ClientSynSendForDevice] http_request auth info active��%d,cnonce:%s, nc:%s, nonce:%s qop:%s, realm:%s, uri:%s, username:%s,response:%s",
                http_request.client_auth.active, http_request.client_auth.cnonce.c_str(),
                http_request.client_auth.nc.c_str(), http_request.client_auth.nonce.c_str(),
                http_request.client_auth.qop.c_str(), http_request.client_auth.realm.c_str(),
                http_request.client_auth.uri.c_str(), http_request.client_auth.username.c_str(),
                http_request.client_auth.response.c_str());*/
		}

		// ��������
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

		// ����������÷Ŵ�����
		http_request.recv_timeout = send_info.time_out * time_out_fator_;
		http_request.connect_tiemout *= time_out_fator_;
		http_request.send_timeout *= time_out_fator_;

		// ��ǰ���Ӿ��,����ֵ
		LONG client_handle = -1;
		// �ͻ��˽������ݽṹ��,����ֵ
		HttpClientRecv recv_data;

		// �ͻ��˷��ͺ���,���յ����ݴ�����ɺ�,��Ҫ�رյ�ǰ����
		LONG ret_value = CVsmNetLib::HttpClientSynSend(client_handle, recv_data, http_request);
		if (NET_ERR_NONE == ret_value)
		{
			// �����յ�����,�����ж�http������
			if (recv_data.http_code == 200 || recv_data.http_code == 401)
			{
				// �Ƚ�httpЭ��ͷ�����ݳ��Ⱥ��յ������ݳ����Ƿ�һ��,˵�����ݽ������
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
					// �������ݳ��ȴ���
					assert(0);

					receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
					receive_info.error_code_.iFirstCode = IVMS_COMPONENT_LOCAL;
					receive_info.error_code_.iSecondCode = IVMS_INVALID_DATA;
					// �ر�����
					CVsmNetLib::HttpClostClientLink(client_handle);

					return false;
				}
			}
			else
			{
				// httpЭ�����
				assert(0);

				receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
				receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
				receive_info.error_code_.iSecondCode = recv_data.http_code;


				// �ر�����
				CVsmNetLib::HttpClostClientLink(client_handle);

				return false;
			}
		}
		else
		{
			// ��������ʧ��
			receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
			receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
            receive_info.error_code_.iSecondCode = ret_value;
			//IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSendForDevice] failed, [errorcode: %d],[request: %s]", ret_value, http_request.http_url.c_str());
			// �ر�����
			CVsmNetLib::HttpClostClientLink(client_handle);

			return false;
		}

		// ���ݴ�����ɺ�,��Ҫ�ر�����
		CVsmNetLib::HttpClostClientLink(client_handle);

		return true;
	
	}

    LONG CNetlibWrapper::WebsocketClientSynCreat(LONG& client_handle, const string& str_server_ip, const unsigned int& uPort, const Fun_WebSocketCallBackEx client_callback)
    {
        //ת������ӿ�����
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

    // ����
    LONG CNetlibWrapper::WebsocketClientSend(LONG client_handle, WebsocketMsg msg_type, char* send_data, DWORD data_len, bool is_fin)
    {
        //cs ֻ��text
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
            break;//������ʼ
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

		// ��ֵ�������
		HttpRequest http_request;
		// ����
		http_request.http_method = send_info.str_method;

        string str_isapi = send_info.str_isapi;
        // ����
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
            //��ȻҲʹ����post����(�в�����4.0�汾Ϊ�˽��get����url�����ض������޸ĵ�)����ʵ����get��Ϊ����Ҳ��Query������MT=GET
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


        //isapiЭ��
        //string str_isapi;
        if (!send_info.str_request.empty())
        {
            if (send_info.str_method == "GET" && send_info.request_pattern == binary_request)
            {
                str_isapi += "&" + send_info.str_request;
            }
            else
            {
				//HC1.4�汾ɾ��data�󶨣�1.3�汾ϵͳ���Ժ��޸ģ����Բ���ֱ��ɾ��
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
		// ����url
		//if (!send_info.str_request.empty())
		//{
		//	////�µ�XMLʵ�ֳ�����<?xml version="1.0" encoding="UTF-8"?>\n<DeviceStatusRequest>������</DeviceStatusRequest >\n
		//	////�����ǰ��һ��<?xml version="1.0" encoding="UTF-8"?>,���������UTF-8���ܻ�䡣
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

		// token ���жϷ���ֵ 
		ptr_server_infor->CreateTokenForExpress(http_request.token_value, session, security_version, token_secret_key);
		// ��������
		if (!send_info.str_send_data.empty())
		{
			http_request.send_size = send_info.str_send_data.size();
			http_request.send_buff = const_cast<char*>(send_info.str_send_data.c_str());
		//	IVMS_LOG_INFO("CommLayer", "[CNetlibWrapper::ClientSynSendForExpress] http_request str_send_data %s", send_info.str_send_data.c_str());
		}

		// ����������÷Ŵ�����
		http_request.recv_timeout = send_info.time_out * time_out_fator_;
		http_request.connect_tiemout *= time_out_fator_;
		http_request.send_timeout *= time_out_fator_;

		// ��ǰ���Ӿ��,����ֵ
		LONG client_handle = -1;
		// �ͻ��˽������ݽṹ��,����ֵ
		HttpClientRecv recv_data;

		// �ͻ��˷��ͺ���,���յ����ݴ�����ɺ�,��Ҫ�رյ�ǰ����
		LONG ret_value = CVsmNetLib::HttpClientSynSend(client_handle, recv_data, http_request);

		// session������ͷ�
		ptr_server_infor->ReleaseTokenFroExpress(receive_info.error_code_.iFirstCode, receive_info.error_code_.iSecondCode, session);

		if (NET_ERR_NONE == ret_value)
		{
			// �����յ�����,�����ж�http������
			if (recv_data.http_code == 200)
			{
				// �Ƚ�httpЭ��ͷ�����ݳ��Ⱥ��յ������ݳ����Ƿ�һ��,˵�����ݽ������
				if ((recv_data.content_len == recv_data.recv_size) && (recv_data.recv_size != 0))
				{
					receive_info.receive_data.resize(recv_data.recv_size);
					memcpy(const_cast<char*>(receive_info.receive_data.c_str()), recv_data.recv_data, recv_data.recv_size);
                    if (!receive_info.receive_data.empty() && send_info.request_pattern != binary_request)
					{
						// express���ش����룬�ͻ��˽��е���
						// ��ϵͳ���ͻ���
						// ����ģ�飺Express
						// �����룺50
						// ������Ϣ��Express���صĴ�����
						unique_ptr<CXmlProto> pro_req(new CXmlProto());
						pro_req->set_str(receive_info.receive_data);
// 						receive_info.error_code_.iFirstCode = IVMS_COMPONENT_RemoteSite;
// 						receive_info.error_code_.iSecondCode = IVMS_CASCADING_ERROR_CODE;
// 						receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
// 						receive_info.error_code_.SetCasadingErrorInfo(Base::Error::MakeCascadingErrorStr(pro_req->get_int_v("/ResponseStatus/ErrorModule"), pro_req->get_int_v("/ResponseStatus/ErrorCode")));
// 						if (CMS_MODULE_CMS == receive_info.error_code_.iFirstCode && CMS_SESSIONID_INVALID == receive_info.error_code_.iSecondCode)
// 						{
// 							// ��ЧSeeeion���ͷ�Token
// 							ptr_server_infor->SercurityRelease(receive_info.error_code_.iFirstCode, receive_info.error_code_.iSecondCode, session);
// 						}

						if (pro_req->get_int_v("/ResponseStatus/ErrorCode") != 0)
						{
							receive_info.error_code_.iFirstCode = IVMS_COMPONENT_RemoteSite;
							receive_info.error_code_.iSecondCode = IVMS_CASCADING_ERROR_CODE;
							receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
							receive_info.error_code_.SetCasadingErrorInfo(Base::Error::MakeCascadingErrorStr(pro_req->get_int_v("/ResponseStatus/ErrorModule"), pro_req->get_int_v("/ResponseStatus/ErrorCode")));
							return false;
							// session��Ч��200����Ҫ���⴦��
// 							if (CMS_SESSIONID_INVALID == pro_req->get_int_v("/ResponseStatus/ErrorCode"))
// 							{
// 								IVMS_LOG_WARN("CommLayer", "[CNetlibWrapper::ClientSynSendForExpress] http_url:%s requeset CMS_SESSIONID_INVALID session:%s, release token:%s", http_request.http_url.c_str(), session.c_str(), http_request.token_value.c_str());
// 								// ��ЧSeeeion���ͷ�Token
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
					// �������ݳ��ȴ���
					assert(0);

					receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
					receive_info.error_code_.iFirstCode = IVMS_COMPONENT_LOCAL;
					receive_info.error_code_.iSecondCode = IVMS_INVALID_DATA;
					// �ر�����
					CVsmNetLib::HttpClostClientLink(client_handle);

					return false;
				}
			}
			else
			{
				// httpЭ�����
				assert(0);

				receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
				receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
				receive_info.error_code_.iSecondCode = recv_data.http_code;

				// �ر�����
				CVsmNetLib::HttpClostClientLink(client_handle);

				return false;
			}
		}
		else
		{
			// ��������ʧ��
			receive_info.error_code_.SetSubSystemErrorCode(IVMS_SUBSYSTEM_CLIENT);
			receive_info.error_code_.iFirstCode = IVMS_COMPONENT_NETWORK;
			receive_info.error_code_.iSecondCode = ret_value;
		//	IVMS_LOG_ERROR("CommLayer", "[CNetlibWrapper::ClientSynSendForExpress] failed, [errorcode: %d],[request: %s]", ret_value, http_request.http_url.c_str());
			// �ر�����
			CVsmNetLib::HttpClostClientLink(client_handle);

			return false;
		}

		// ���ݴ�����ɺ�,��Ҫ�ر�����
		CVsmNetLib::HttpClostClientLink(client_handle);

		return true;
	}

}
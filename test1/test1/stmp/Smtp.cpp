#include "../stdafx.h"
#include<stdlib.h>
#include <stdio.h>
#include "Smtp.h"  
#include "Base64.h"  
#include "..\curl\curl.h" 
#include <iostream>  
#include <sstream>  
#include <fstream>  
#include <string.h>
//#include <ClientCommon.h>
#include "StringConversions.h"

using namespace std;



#define CHECK_CURL_RESULT(res) \
if (res != CURLE_OK) \
{ \
    SetErrorCode(m_errorCode, res);\
    cout << "smtp send mail failed, error:" << res;\
    return false; \
};


    
#define DEFAULT_CONNECT_TIMEOUT 5;
#define DEFAULT_SEND_TIMEOUT 10;



	void  SetErrorCode(CSmtpSendMail::SmtpErrorCode& smtp_error, CURLcode curl_error)
	{
		switch (curl_error)
		{
		case CURLE_COULDNT_CONNECT:
			smtp_error = CSmtpSendMail::SmtpErrorCode::SMTP_NETWORK_FAIL_CONNECT;
			break;
		default:
			smtp_error = CSmtpSendMail::SmtpErrorCode::SMTP_UNKNOWN_ERROR;
			break;
		}
	}
 

       

        CSmtpSendMail::CSmtpSendMail(const std::string & charset)
        {
            m_strCharset = charset;
            m_vRecvMail.clear();
            m_vCCRecvMail.clear();
            m_vBCCRecvMail.clear();
            m_encryptMode = EncryptMode::NO_ENCRYPT;
            m_sendTimeout = DEFAULT_SEND_TIMEOUT;
            m_connectTimeout = DEFAULT_CONNECT_TIMEOUT;
            m_errorCode = SMTP_NO_ERROR;
            m_enableAuth = true;
            m_strSubject = "Subject: "; //初始化为空主题
        }

        CSmtpSendMail::~CSmtpSendMail()
        {
            for (auto iter : m_vAttachMentBuf)
            {
                if (nullptr != iter.file_buf)
                {
                    delete[] iter.file_buf;
                }
            }
        }

        void CSmtpSendMail::SetAuthEnable(bool enable_auth)
        {
            m_enableAuth = enable_auth;
        }

        void CSmtpSendMail::SetMailEncryptMode(EncryptMode encrypt_mode)
        {
            m_encryptMode = encrypt_mode;
        }

        void CSmtpSendMail::SetSmtpServer(const std::string & username, const std::string &password, const std::string & servername, unsigned int port)
        {
            m_strUserName = username;
            m_strPassword = password;
            m_strServerName = servername;
            m_uiPort = port;
        }

        bool CSmtpSendMail::SetSendName(const std::string & sendname)
        {
            std::string strTemp = "";
            strTemp += "=?";
            strTemp += m_strCharset;
            strTemp += "?B?";
            std::string sendNameEncoded;
            if (!CBase64::Base64Encode(sendNameEncoded, sendname))
            {
                printf("base64 encode send name failed:%s", sendname.c_str());
               // NET_LOG_ERROR() << "base64 encode send name failed:" << sendname.c_str();
                m_errorCode = SMTP_BASE64_ENCODE_ERROR;
                return false;
            }
            strTemp += sendNameEncoded;
            strTemp += "?=";
            m_strSendName = strTemp;
            return true;
        }

        void CSmtpSendMail::SetSendMail(const std::string & sendmail)
        {
            m_strSendMail = sendmail;
        }

        void CSmtpSendMail::AddRecvMail(const std::vector<std::string>& recvmailList)
        {
            for (auto recvmail : recvmailList)
            {
                m_vRecvMail.push_back(recvmail);
            }
        }

        void CSmtpSendMail::AddCCRecvMail(const std::vector<std::string>& recvmailList)
        {
            for (auto recvmail : recvmailList)
            {
                m_vCCRecvMail.push_back(recvmail);
            }
        }

        void CSmtpSendMail::AddBCCRecvMail(const std::vector<std::string>& recvmailList)
        {
            for (auto recvmail : recvmailList)
            {
                m_vBCCRecvMail.push_back(recvmail);
            }
        }

        bool CSmtpSendMail::SetSubject(const std::string & subject)
        {
            if (subject.empty())
            {
                printf("set an empty subject\n");
                m_strSubject = "Subject: "; //设置为空主题
                return true;
            }
            std::string strTemp = "";
            strTemp = "Subject: ";
            strTemp += "=?";
            strTemp += m_strCharset;
            strTemp += "?B?";
            std::string subjectEncoded;
            if (!CBase64::Base64Encode(subjectEncoded, subject))
            {
                printf("base64 encode subject failed:%s", subject.c_str());
                //NET_LOG_ERROR() << "base64 encode subject failed:" << subject.c_str();
                m_errorCode = SMTP_BASE64_ENCODE_ERROR;
                return false;
            }
            strTemp += subjectEncoded;
            strTemp += "?=";
            m_strSubject = strTemp;
            return true;
        }

        void CSmtpSendMail::SetBodyContent(const std::string & content)
        {
            m_strContent = content;
        }

        void CSmtpSendMail::AddAttachment(const std::string & filename)
        {
            if (filename.empty())
            {
                return;
            }
            m_vAttachMent.push_back(filename);
        }

        bool CSmtpSendMail::AddAttachment(const std::string &filename, const char* file_buf, unsigned int file_size)
        {
            if (nullptr == file_buf || file_size == 0)
            {
                printf("AddAttachment, file buf is null or file size is 0\n");
                //NET_LOG_ERROR() << "AddAttachment, file buf is null or file size is 0.";
                m_errorCode = SMTP_ATTACHMENT_ERROR;
                return false;
            }

            StruAttachMentInfo stru_attachment_info;
            if (filename.empty())
            {
                stru_attachment_info.file_name = "attachment";
            }
            else
            {
                stru_attachment_info.file_name = filename;
            }
            stru_attachment_info.file_buf = new char[file_size + 1];
            memset(stru_attachment_info.file_buf, 0, file_size + 1);
            memcpy(stru_attachment_info.file_buf, file_buf, file_size);
            stru_attachment_info.file_size = file_size;

            m_vAttachMentBuf.push_back(stru_attachment_info);
            return true;
        }

        void CSmtpSendMail::SetTimeout(int connect_timeout, int send_timeout)
        {
            m_connectTimeout = connect_timeout;
            m_sendTimeout = send_timeout;
        }

        CSmtpSendMail::SmtpErrorCode CSmtpSendMail::GetLastError()
        {
            return m_errorCode;
        }

        bool CSmtpSendMail::SendMail()
        {
            if (!CreatMessage())
            {
                printf("smtp send mail, create message failed.\n");
                //NET_LOG_ERROR() << "smtp send mail, create message failed.";
                m_errorCode = SMTP_BASE64_ENCODE_ERROR;
                return false;
            }

            bool ret = true;
            CURL *curl;
            CURLcode res = CURLE_OK;
            struct curl_slist *recipients = NULL;

            curl = curl_easy_init();
            if (curl)
            {
                /* Set username and password */
                res = curl_easy_setopt(curl, CURLOPT_USERNAME, m_strUserName.c_str());
                CHECK_CURL_RESULT(res);
                res = curl_easy_setopt(curl, CURLOPT_PASSWORD, m_strPassword.c_str());
                CHECK_CURL_RESULT(res);
                std::string tmp = "smtp://";
                if (m_encryptMode != EncryptMode::NO_ENCRYPT && m_uiPort == 465)
                {
                    tmp = "smtps://";
                }
                tmp += m_strServerName;
                char cPort[10];  
                memset(cPort, 0, 10);  
				sprintf_s(cPort, 9, ":%d", m_uiPort);
                tmp += cPort;  

                // 注意不能直接传入tmp，应该带上.c_str()，否则会导致下面的
                // curl_easy_perform调用返回CURLE_COULDNT_RESOLVE_HOST错误
                // 码
                res = curl_easy_setopt(curl, CURLOPT_URL, tmp.c_str());
                CHECK_CURL_RESULT(res);

                res = curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, m_connectTimeout);
                res = curl_easy_setopt(curl, CURLOPT_TIMEOUT, m_sendTimeout);
                /* If you want to connect to a site who isn't using a certificate that is
                * signed by one of the certs in the CA bundle you have, you can skip the
                * verification of the server's certificate. This makes the connection
                * A LOT LESS SECURE.
                *
                * If you have a CA cert for the server stored someplace else than in the
                * default bundle, then the CURLOPT_CAPATH option might come handy for
                * you. */
                switch (m_encryptMode)
                {
                case EncryptMode::NO_ENCRYPT:
                    break;
                case EncryptMode::SSL_ENCRYPT:
                    res = curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
                    CHECK_CURL_RESULT(res);
                    res = curl_easy_setopt(curl, CURLOPT_FTPSSLAUTH, CURLFTPAUTH_SSL);
                    CHECK_CURL_RESULT(res);
                    break;
                case EncryptMode::TLS_ENCRYPT:
                    res = curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
                    CHECK_CURL_RESULT(res);
                    res = curl_easy_setopt(curl, CURLOPT_FTPSSLAUTH, CURLFTPAUTH_TLS);
                    CHECK_CURL_RESULT(res);
                    break;
                default:
                    break;
                }
#ifdef SKIP_PEER_VERIFICATION  
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif  

                /* If the site you're connecting to uses a different host name that what
                * they have mentioned in their server certificate's commonName (or
                * subjectAltName) fields, libcurl will refuse to connect. You can skip
                * this check, but this will make the connection less secure. */
#ifdef SKIP_HOSTNAME_VERIFICATION  
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif  

                /* Note that this option isn't strictly required, omitting it will result
                * in libcurl sending the MAIL FROM command with empty sender data. All
                * autoresponses should have an empty reverse-path, and should be directed
                * to the address in the reverse-path which triggered them. Otherwise,
                * they could cause an endless loop. See RFC 5321 Section 4.5.5 for more
                * details.
                */
                //curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM);   
                res = curl_easy_setopt(curl, CURLOPT_MAIL_FROM, m_strSendMail.c_str());
                CHECK_CURL_RESULT(res);
                /* Add two recipients, in this particular case they correspond to the
                * To: and Cc: addressees in the header, but they could be any kind of
                * recipient. */
                for (std::size_t i = 0; i < m_vRecvMail.size(); i++)
                {
                    recipients = curl_slist_append(recipients, m_vRecvMail[i].c_str());
                }

                for (std::size_t i = 0; i < m_vCCRecvMail.size(); i++)
                {
                    recipients = curl_slist_append(recipients, m_vCCRecvMail[i].c_str());
                }

                for (std::size_t i = 0; i < m_vBCCRecvMail.size(); i++)
                {
                    recipients = curl_slist_append(recipients, m_vBCCRecvMail[i].c_str());
                }
                res = curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
                CHECK_CURL_RESULT(res);

                std::stringstream stream;
                stream.str(m_strMessage.c_str());
                stream.flush();
                /* We're using a callback function to specify the payload (the headers and
                * body of the message). You could just use the CURLOPT_READDATA option to
                * specify a FILE pointer to read from. */

                // 注意回调函数必须设置为static
                res = curl_easy_setopt(curl, CURLOPT_READFUNCTION, &CSmtpSendMail::payload_source);
                CHECK_CURL_RESULT(res);
                res = curl_easy_setopt(curl, CURLOPT_READDATA, (void *)&stream);
                CHECK_CURL_RESULT(res);
                res = curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
                CHECK_CURL_RESULT(res);

                /* Since the traffic will be encrypted, it is very useful to turn on debug
                * information within libcurl to see what is happening during the
                * transfer */
                res = curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
                CHECK_CURL_RESULT(res);

                /* Send the message */
                res = curl_easy_perform(curl);
                CURLINFO info = CURLINFO_OS_ERRNO;
                long oserrno = 0;
                curl_easy_getinfo(curl, info, oserrno);
                /* Check for errors */
                if (res != CURLE_OK)
                {
                    printf("send mail failed, err no:%d, curl info:%d, os error number:%d", res, info, oserrno);
                    //NET_LOG_ERROR() << "smtp send mail failed.err no:" << res << " curl info:" << info << " os error number:" << oserrno;
                    ret = false;
                }

                /* Free the list of recipients */
                curl_slist_free_all(recipients);

                /* Always cleanup */
                curl_easy_cleanup(curl);

            }
            else
            {
               // NET_LOG_ERROR() << "curl init failed";
                ret = false;
            }
            return ret;
        }

		std::size_t CSmtpSendMail::payload_source(void *ptr, std::size_t size, std::size_t nmemb, void *stream)
        {
			std::size_t num_bytes = size * nmemb;
            char* data = (char*)ptr;
            std::stringstream* strstream = (std::stringstream*)stream;

            strstream->read(data, num_bytes);

            return strstream->gcount();
        }

        bool CSmtpSendMail::CreatMessage()
        {
            if (m_strSendMail.empty() || m_vRecvMail.empty())
            {
                m_errorCode = SMTP_BASE64_ENCODE_ERROR;
                return false;
            }
            m_strMessage = "From: ";
            m_strMessage += m_strSendMail;
            m_strMessage += "\r\nReply-To: ";
            m_strMessage += m_strSendMail;
            m_strMessage += "\r\nTo: ";
            for (std::size_t i = 0; i < m_vRecvMail.size(); i++)
            {
                if (i > 0)
                {
                    m_strMessage += ",";
                }
                m_strMessage += m_vRecvMail[i];
            }
            m_strMessage += "\r\n";

            if (!m_vCCRecvMail.empty())
            {
                m_strMessage += "Cc: ";
                for (std::size_t i = 0; i < m_vCCRecvMail.size(); i++)
                {
                    if (i > 0)
                    {
                        m_strMessage += ",";
                    }
                    m_strMessage += m_vCCRecvMail[i];
                }
                m_strMessage += "\r\n";
            }
            
            if (!m_vBCCRecvMail.empty())
            {
                m_strMessage += "Bcc: ";
                for (std::size_t i = 0; i < m_vBCCRecvMail.size(); i++)
                {
                    if (i > 0)
                    {
                        m_strMessage += ",";
                    }
                    m_strMessage += m_vBCCRecvMail[i];
                }
                m_strMessage += "\r\n";
            }

            m_strMessage += m_strSubject;
            m_strMessage += "\r\nX-Mailer: The Bat! (v3.02) Professional";
            m_strMessage += "\r\nMime-Version: 1.0";
            m_strMessage += "\r\nContent-Type: multipart/mixed;";
            m_strMessage += "boundary=\"simple boundary\"";
            m_strMessage += "\r\nThis is a multi-part message in MIME format.";
            m_strMessage += "\r\n--simple boundary";
            //正文  
            m_strMessage += "\r\nContent-Type: text/plain;";
            m_strMessage += "charset=";
            m_strMessage += "\"";
            m_strMessage += m_strCharset;
            m_strMessage += "\"";
            m_strMessage += "\r\nContent-Transfer-Encoding: 7BIT";
            m_strMessage += "\r\n\r\n";
            m_strMessage += m_strContent;

            //附件  
            std::string filename = "";
            std::string filetype = "";
            for (std::size_t i = 0; i < m_vAttachMent.size(); i++)
            {
                m_strMessage += "\r\n--simple boundary";
                GetFileName(m_vAttachMent[i], filename);
                GetFileType(m_vAttachMent[i], filetype);
                SetContentType(filetype);
                SetFileName(filename);

                m_strMessage += "\r\nContent-Type: ";
                m_strMessage += m_strContentType;
                m_strMessage += "\tname=";
                m_strMessage += "\"";
                m_strMessage += m_strFileName;
                m_strMessage += "\"";
                m_strMessage += "\r\nContent-Disposition:attachment;filename=";
                m_strMessage += "\"";
                m_strMessage += m_strFileName;
                m_strMessage += "\"";
                m_strMessage += "\r\nContent-Transfer-Encoding:base64";
                m_strMessage += "\r\n\r\n";


                FILE *pt = NULL;
                std::string tmp_str = UTF8ToLocal(m_vAttachMent[i]);
                if ((pt = fopen(tmp_str.c_str(), "rb")) == NULL)
                {
                    printf("open attachment failed:%s\n", m_vAttachMent[i].c_str());
                    m_errorCode = SMTP_ATTACHMENT_ERROR;
                    //NET_LOG_ERROR() << "open attachment failed.";
                    return false;
                }
                fseek(pt, 0, SEEK_END);
                int len = ftell(pt);
                fseek(pt, 0, SEEK_SET);
                int rlen = 0;
                char buf[55];
                for (std::size_t j = 0; j < len / 54 + 1; j++)
                {
                    memset(buf, 0, 55);
                    rlen = fread(buf, sizeof(char), 54, pt);
                    std::string encodeTmp;
					CBase64::Base64Encode(encodeTmp, std::string(buf, rlen));
                    m_strMessage += encodeTmp;
                    m_strMessage += "\r\n";
                }
                fclose(pt);
                pt = NULL;

            }
            //附件  
            filename = "";
            filetype = "";
            for (std::size_t i = 0; i < m_vAttachMentBuf.size(); i++)
            {
                m_strMessage += "\r\n--simple boundary";
                GetFileName(m_vAttachMentBuf[i].file_name, filename);
                GetFileType(m_vAttachMentBuf[i].file_name, filetype);
                SetContentType(filetype);
                SetFileName(filename);

                m_strMessage += "\r\nContent-Type: ";
                m_strMessage += m_strContentType;
                m_strMessage += "\tname=";
                m_strMessage += "\"";
                m_strMessage += m_strFileName;
                m_strMessage += "\"";
                m_strMessage += "\r\nContent-Disposition:attachment;filename=";
                m_strMessage += "\"";
                m_strMessage += m_strFileName;
                m_strMessage += "\"";
                m_strMessage += "\r\nContent-Transfer-Encoding:base64";
                m_strMessage += "\r\n\r\n";

                int rlen = 0;
                char buf[55];
                for (std::size_t tmp = 0; tmp < m_vAttachMentBuf[i].file_size / 54 + 1; tmp++)
                {
                    memset(buf, 0, 55);
                    int cpy_len = 54;
                    if ((tmp + 1) * 54 > m_vAttachMentBuf[i].file_size)
                    {
                        cpy_len = m_vAttachMentBuf[i].file_size - tmp * 54;
                    }
                    memcpy(buf, m_vAttachMentBuf[i].file_buf + tmp * 54, cpy_len);
                    std::string encodeTmp;
					CBase64::Base64Encode(encodeTmp, std::string(buf, cpy_len));
                    m_strMessage += encodeTmp;
                    m_strMessage += "\r\n";
                }

            }

            if (m_vAttachMent.size() > 0 || m_vAttachMentBuf.size() > 0)
            {
                m_strMessage += "\r\n--simple boundary--\r\n";
            }
            return true;
        }


        int CSmtpSendMail::GetFileType(std::string const & stype)
        {
            if (stype == "txt")
            {
                return 0;
            }
            else if (stype == "xml")
            {
                return 1;
            }
            else if (stype == "html")
            {
                return 2;
            }
            else if (stype == "jpeg")
            {
                return 3;
            }
            else if (stype == "png")
            {
                return 4;
            }
            else if (stype == "gif")
            {
                return 5;
            }
            else if (stype == "exe")
            {
                return 6;
            }

            return -1;
        }

        bool CSmtpSendMail::SetFileName(const std::string & fileName)
        {
            std::string EncodedFileName = "=?";
            EncodedFileName += m_strCharset;
            EncodedFileName += "?B?";//修改  
            std::string base64Filename;
            if (!CBase64::Base64Encode(base64Filename, fileName))
            {
                printf("base64 encode fileName failed:%s", fileName.c_str());
                m_errorCode = SMTP_BASE64_ENCODE_ERROR;
               // NET_LOG_ERROR() << "base64 encode fileName failed:" << fileName.c_str();
                return false;
            }
            EncodedFileName += base64Filename;
            EncodedFileName += "?=";
            m_strFileName = EncodedFileName;
            return true;
        }

        void CSmtpSendMail::SetContentType(std::string const & stype)
        {
            int type = GetFileType(stype);
            switch (type)
            {//  
            case 0:
                m_strContentType = "plain/text;";
                break;

            case 1:
                m_strContentType = "text/xml;";
                break;

            case 2:
                m_strContentType = "text/html;";

            case 3:
                m_strContentType = "image/jpeg;";
                break;

            case 4:
                m_strContentType = "image/png;";
                break;

            case 5:
                m_strContentType = "image/gif;";
                break;

            case 6:
                m_strContentType = "application/x-msdownload;";
                break;

            default:
                m_strContentType = "application/octet-stream;";
                break;
            }
        }

        void CSmtpSendMail::GetFileName(const std::string& file, std::string& filename)
        {
            std::string::size_type p = file.find_last_of('/');
            if (p == std::string::npos)
            {
                p = file.find_last_of('\\');
            }

            if (p != std::string::npos)
            {
                p += 1; // get past folder delimeter  
                filename = file.substr(p, file.length() - p);
            }
            else
            {
                filename = file;
            }
        }

        void CSmtpSendMail::GetFileType(const std::string & file, std::string & stype)
        {
            std::string::size_type p = file.find_last_of('.');
            if (p != std::string::npos)
            {
                p += 1; // get past folder delimeter  
                stype = file.substr(p, file.length() - p);
            }
        }
    

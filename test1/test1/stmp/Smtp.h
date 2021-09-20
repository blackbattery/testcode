
#ifndef _SMTP_H_
#define _SMTP_H_

#include <string>  
#include <vector>  
//#include "../BeeNetExports.h"

 
#define SKIP_PEER_VERIFICATION  
#define SKIP_HOSTNAME_VERIFICATION  

        enum class EncryptMode
        {
            NO_ENCRYPT = 0,
            SSL_ENCRYPT,
            TLS_ENCRYPT
        };

        struct StruAttachMentInfo
        {
            std::string file_name;
            char* file_buf;
            unsigned int file_size;
            StruAttachMentInfo()
                :file_buf(nullptr)
                , file_size(0)
            {}
        };

        class  CSmtpSendMail
        {
        public:
            enum SmtpErrorCode
            {
                SMTP_NO_ERROR = 0,
                SMTP_BASE64_ENCODE_ERROR,
                SMTP_ATTACHMENT_ERROR,
                SMTP_NETWORK_FAIL_CONNECT,
                SMTP_AUTH_REQUIRED,
                SMTP_MAILBOX_UNAVAILABLE,
                SMTP_AUTH_FAILED,
                SMTP_UNKNOWN_ERROR,
            };

            CSmtpSendMail(const std::string & charset = "utf-8"); // 初始值为utf-8

            ~CSmtpSendMail();

            //设置stmp服务器、用户名、密码、端口（端口其实不用指定，libcurl默认25，但如果是smtps则默认是465）  
            void SetSmtpServer(const std::string &username, const std::string& password, const std::string& servername, unsigned int port = 25);

            //设置邮件加密模式
            void SetMailEncryptMode(EncryptMode encrypt_mode);

            //开启认证，true-开启，false-不开启，默认为true
            void SetAuthEnable(bool enable_auth);

            //发送者姓名，可以不用  

            bool SetSendName(const std::string& sendname);

            //发送者邮箱   
            void SetSendMail(const std::string& sendmail);

            //添加收件人  
            void AddRecvMail(const std::vector<std::string>& recvmailList);

            //添加抄送人 
            void AddCCRecvMail(const std::vector<std::string>& recvmailList);

            //添加暗送人 
            void AddBCCRecvMail(const std::vector<std::string>& recvmailList);

            //设置主题  
            bool SetSubject(const std::string &subject);

            //设置正文内容  
            void SetBodyContent(const std::string &content);

            //添加附件,传入附件路径
            void AddAttachment(const std::string &filename);

            //添加附件,二进制形式  
            bool AddAttachment(const std::string &filename, const char* file_buf, unsigned int file_size);

            //设置超时时间
            //connect_timeout:连接超时时间，单位 秒
            //send_timeout:发送超时时间，单位 秒
            void SetTimeout(int connect_timeout, int send_timeout);

            SmtpErrorCode GetLastError();

            //发送邮件  
            bool SendMail();
        private:

            //回调函数，将MIME协议的拼接的字符串由libcurl发出  
            static std::size_t payload_source(void *ptr, std::size_t size, std::size_t nmemb, void *stream);

            //创建邮件MIME内容  
            bool CreatMessage();

            //获取文件类型  
            int GetFileType(std::string const& stype);

            //设置文件名  
            bool SetFileName(const std::string& FileName);

            //设置文件的contenttype  
            void SetContentType(std::string const& stype);

            //得到文件名  
            void GetFileName(const std::string& file, std::string& filename);

            //得到文件类型  
            void GetFileType(const std::string& file, std::string& stype);

        private:
            std::string m_strCharset; //邮件编码  
            std::string m_strSubject; //邮件主题  
            std::string m_strContent; //邮件内容  
            std::string m_strFileName; //文件名  
            std::string m_strMessage;// 整个MIME协议字符串  
            std::string m_strUserName;//用户名  
            std::string m_strPassword;//密码  
            std::string m_strServerName;//smtp服务器  
            unsigned int m_uiPort;//端口  
            std::string m_strSendName;//发送者姓名  
            std::string m_strSendMail;//发送者邮箱  
            std::string m_strContentType;//附件contenttype  
            std::string m_strFileContent;//附件内容  
            EncryptMode m_encryptMode;
            bool m_enableAuth;  //是否开启认证

            std::vector<std::string> m_vRecvMail; //收件人容器  
            std::vector<std::string> m_vCCRecvMail; //抄送人容器 
            std::vector<std::string> m_vBCCRecvMail; //暗送人容器 
            std::vector<std::string> m_vAttachMent;//附件容器  

            std::vector<StruAttachMentInfo> m_vAttachMentBuf;//二进制形式附件容器
            int m_connectTimeout;
            int m_sendTimeout;
            SmtpErrorCode m_errorCode;
        };


#endif

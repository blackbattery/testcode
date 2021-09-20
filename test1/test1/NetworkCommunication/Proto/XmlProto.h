#pragma once
#include "XmlProtoData.h"


namespace NetworkCommunication
{
    class CXmlProtoImp;

    class  CXmlProto
	{
	public:
		CXmlProto();
		explicit CXmlProto(CXmlProto* other);
		virtual ~CXmlProto();

	public:
		bool set_str(std::string str);
		std::string get_str(int format = 0);

		int get_cnt(std::string url);
		std::string get_v(std::string url,std::string defv = "");
		int get_int_v(std::string url,int defv = -1);
		bool set_v(std::string url, std::string val);
		bool set_v(std::string url, int val);

		bool add_v(std::string url, std::string tag, std::string val);
		bool add_v(std::string url, std::string tag, int val);

		void* get_node(std::string url);
		std::string get_node_xml(const std::string url, std::string defv = "");
		void* add_v(void* node, std::string tag, std::string val);
		void* add_v(void* node, std::string tag, int val);
	public:
        CXmlProto* copy(int full = 0);
		bool clear();
	private:
        CXmlProtoImp* ptr_xml_imp_;
	};

}

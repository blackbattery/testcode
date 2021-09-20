#include "stdafx.h"
#include "XmlProto.h"
#include "Private/XmlProtoImp.h"

#include <sstream>

namespace NetworkCommunication
{
	using namespace std;

	CXmlProto::CXmlProto()
        : ptr_xml_imp_(new CXmlProtoImp)
	{

	}

	CXmlProto::CXmlProto( CXmlProto* other)
	{
        ptr_xml_imp_->set_str(other->get_str());
	}

	CXmlProto::~CXmlProto()
	{
        if (ptr_xml_imp_ != NULL)
        {
            delete ptr_xml_imp_;
        }
	}

	bool CXmlProto::set_str(std::string str)
	{
        return ptr_xml_imp_->set_str(str);
	}

	std::string CXmlProto::get_str(int format)
	{
        return ptr_xml_imp_->get_str(format);
	}

	int CXmlProto::get_cnt(std::string url)
	{
        return ptr_xml_imp_->get_cnt(url);
	}

	std::string CXmlProto::get_v(std::string url,std::string defv)
	{
        return ptr_xml_imp_->get_v(url, defv);
	}

	int CXmlProto::get_int_v(std::string url, int defv)
	{
        return ptr_xml_imp_->get_int_v(url, defv);
	}

	bool CXmlProto::set_v(std::string url, std::string val)
	{
        return ptr_xml_imp_->set_v(url, val);
	}

	bool CXmlProto::set_v(std::string url, int val)
	{

        return ptr_xml_imp_->set_v(url, val);
	}

	bool CXmlProto::add_v(std::string url, std::string tag, std::string val)
	{
        return ptr_xml_imp_->add_v(url, tag, val);
	}

	bool CXmlProto::add_v(std::string url, std::string tag, int val)
	{
        return ptr_xml_imp_->add_v(url, tag, val);
	}

	void* CXmlProto::get_node(std::string url)
	{
        return ptr_xml_imp_->get_node(url);
	}

	std::string CXmlProto::get_node_xml(const std::string url, std::string defv)
	{
		return ptr_xml_imp_->get_node_xml(url, defv);
	}

	void* CXmlProto::add_v(void* node, std::string tag, std::string val)
	{
        return ptr_xml_imp_->add_v(node, tag, val);
	}

	void* CXmlProto::add_v(void* node, std::string tag, int val)
	{
        return ptr_xml_imp_->add_v(node, tag, val);
	}

    CXmlProto* CXmlProto::copy(int full)
	{
		if (full)
		{
			return new CXmlProto(this);
		}
		return new CXmlProto();
	}
	
	bool CXmlProto::clear()
	{
        return ptr_xml_imp_->clear();
	}
}
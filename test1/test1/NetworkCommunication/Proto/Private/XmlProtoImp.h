#ifndef __NETWORKCOMM_XML_PROTO_IMP_H__
#define __NETWORKCOMM_XML_PROTO_IMP_H__

#include "NetworkCommunication/Proto/IProto.h"
#include "NetworkCommunication/Proto/XmlProtoData.h"

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

namespace NetworkCommunication
{
    class CXmlProtoImp : public IProto
    {
    public:
        CXmlProtoImp();
        explicit CXmlProtoImp(CXmlProtoImp* other);
        virtual ~CXmlProtoImp();

    public:
        bool set_str(std::string str);
        std::string get_str(int format = 0);

        int get_cnt(std::string url);
        std::string get_v(std::string url, std::string defv = "");
        int get_int_v(std::string url, int defv = -1);
        bool set_v(std::string url, std::string val);
        bool set_v(std::string url, int val);

        bool add_v(std::string url, std::string tag, std::string val);
        bool add_v(std::string url, std::string tag, int val);

        void* get_node(std::string url);
        void* add_v(void* node, std::string tag, std::string val);
        void* add_v(void* node, std::string tag, int val);

		std::string get_node_xml(const std::string url, std::string defv = "");

    public:
        IProto* copy(int full = 0);
        bool clear();
    private:
        xmlXPathObjectPtr getXPath(const xmlChar *szXpath);
        bool parse_url(std::string url, std::vector<urlNode>& nodeVec);
        int found_url(std::vector<urlNode>& nodeVec);
        bool build_url(std::vector<urlNode>& nodeVec, int index, std::string val);
    private:
        xmlDocPtr doc_ = NULL;
    };
}

#endif //__NETWORKCOMM_XML_PROTO_IMP_H__
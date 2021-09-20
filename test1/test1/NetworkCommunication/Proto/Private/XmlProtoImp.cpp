#include "stdafx.h"
#include "XmlProtoImp.h"

#include <sstream>

namespace NetworkCommunication
{
    using namespace std;

    string& replace_all_distinct(string&   str, const   string&   old_value, const   string&   new_value)
    {
        for (string::size_type pos(0); pos != string::npos; pos += new_value.length())   {
            if ((pos = str.find(old_value, pos)) != string::npos)
                str.replace(pos, old_value.length(), new_value);
            else   break;
        }
        return   str;
    }

    string& rep_xml_char(string&   str)
    {
        return replace_all_distinct(str, "&", "&amp;");
    }

    CXmlProtoImp::CXmlProtoImp()
    {
        doc_ = xmlNewDoc(BAD_CAST "1.0");
    }

    CXmlProtoImp::CXmlProtoImp(CXmlProtoImp* other)
    {
        doc_ = xmlNewDoc(BAD_CAST "1.0");
        set_str(other->get_str());
    }

    CXmlProtoImp::~CXmlProtoImp()
    {
        if (doc_)
        {
            xmlFreeDoc(doc_);
            doc_ = NULL;
        }
    }

    xmlXPathObjectPtr CXmlProtoImp::getXPath(const xmlChar *szXpath)
    {
        xmlXPathContextPtr context;
        xmlXPathObjectPtr xpath;

        context = xmlXPathNewContext(doc_);
        xpath = xmlXPathEvalExpression(szXpath, context);
        xmlXPathFreeContext(context);
        if (xmlXPathNodeSetIsEmpty(xpath->nodesetval))
        {
            xmlXPathFreeObject(xpath);
            return NULL;
        }

        return xpath;
    }

    bool CXmlProtoImp::set_str(std::string str)
    {
        bool result = true;
        if (doc_)
        {
            xmlFreeDoc(doc_);
            doc_ = NULL;
        }
        doc_ = xmlReadMemory(str.c_str(), str.length(), NULL, "UTF-8", XML_PARSE_NOBLANKS);
        if (doc_ == NULL)
        {
            doc_ = xmlNewDoc(BAD_CAST "1.0");
            result = false;
        }
        return result;
    }

    std::string CXmlProtoImp::get_str(int format)
    {
        string str;
        xmlChar *xmlbuff;
        int buffersize;
        //xmlDocDumpFormatMemory(doc_, &xmlbuff, &buffersize, format);
        xmlDocDumpFormatMemoryEnc(doc_, &xmlbuff, &buffersize, "UTF-8", format);
        str = (char *)xmlbuff;
        xmlFree(xmlbuff);

		//新的XML实现出来是<?xml version="1.0" encoding="UTF-8"?>\n<DeviceStatusRequest>。。。</DeviceStatusRequest >\n
		//多出了前面一段<?xml version="1.0" encoding="UTF-8"?>,但是这里的UTF-8可能会变。
		string str_final = str;
		string head = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
		if (str.find(head) != string::npos)
		{
			str_final = str.substr(head.size());
			str_final = str_final.substr(0, str_final.size() - 1);
		}
		return str_final;
    }

	std::string CXmlProtoImp::get_node_xml(const std::string url, std::string defv)
	{
		string result = defv;
		if (url.empty())
		{
			return result;
		}

		xmlXPathObjectPtr xpath = getXPath((const xmlChar*)url.c_str());
		if (xpath != NULL)
		{
			if (xpath->nodesetval->nodeNr == 0)
			{
				xmlXPathFreeObject(xpath);
				return result;
			}
			xmlNodePtr ptr_node = xpath->nodesetval->nodeTab[0];

			xmlBufferPtr nodeBuffer = xmlBufferCreate();
			if (xmlNodeDump(nodeBuffer, doc_, ptr_node->children, 0, 1) > 0)
			{
				result = (char*)nodeBuffer->content;
			}

			xmlBufferFree(nodeBuffer);

			xmlXPathFreeObject(xpath);
		}

		return result;
	}

    int CXmlProtoImp::get_cnt(std::string url)
    {
        int result = 0;
        xmlXPathObjectPtr xpath = getXPath((const xmlChar*)url.c_str());
        if (xpath != NULL)
        {
            result = xpath->nodesetval->nodeNr;
            xmlXPathFreeObject(xpath);
        }
        return result;
    }

    std::string CXmlProtoImp::get_v(std::string url, std::string defv)
    {
        string result = defv;
        xmlXPathObjectPtr xpath = getXPath((const xmlChar*)url.c_str());
        if (xpath != NULL)
        {
            xmlChar *key = xmlNodeListGetString(doc_, xpath->nodesetval->nodeTab[0]->xmlChildrenNode, 1);
            if (key)
            {
                if (strlen((char *)key))//有值
                {
                    result = (char *)key;
                }
                xmlFree(key);
            }
            xmlXPathFreeObject(xpath);
        }
        return result;
    }

    int CXmlProtoImp::get_int_v(std::string url, int defv)
    {
        string val = get_v(url);
        if (val.empty())
        {
            return defv;
        }
        return atoi(val.c_str());
    }

    bool CXmlProtoImp::set_v(std::string url, std::string val)
    {
        rep_xml_char(val);
        vector<urlNode> nodeList;
        if (parse_url(url, nodeList) == false)
        {
            return false;
        }
        int index = found_url(nodeList);
        if (index < 0)
        {
            return false;
        }
        return build_url(nodeList, index, val);
    }

    bool CXmlProtoImp::set_v(std::string url, int val)
    {
        stringstream ss;
        string str;
        ss << val;
        ss >> str;
        return set_v(url, str);
    }

    bool CXmlProtoImp::add_v(std::string url, std::string tag, std::string val)
    {
        rep_xml_char(val);
        bool result = false;
        xmlNodePtr node = NULL;
        if (doc_->children)//非空树
        {
            xmlXPathObjectPtr xpath = getXPath((const xmlChar*)url.c_str());
            if (xpath != NULL)
            {
                node = xpath->nodesetval->nodeTab[0];
                xmlXPathFreeObject(xpath);
            }

            if (node)//创建子节点
            {
                xmlNodePtr tmp_node = xmlNewNode(NULL, BAD_CAST tag.c_str());
                node = xmlAddChild(node, tmp_node);

                if (val.length() > 0)//对于空值情况只创建节点，形如：<a1/>
                {
                    xmlNodeSetContent(node, (const xmlChar *)val.c_str());//存在会覆盖原有子节点的问题
                    //以下代码有设置不能覆盖原有设置的问题，不采用
                    //xmlNodePtr txt = xmlNewText(BAD_CAST val.c_str());
                    //xmlAddChild(node, txt);
                }
                result = true;
            }
        }
        else
        {
            node = xmlNewNode(NULL, BAD_CAST tag.c_str());
            xmlDocSetRootElement(doc_, node);
            result = true;
        }


        return result;
    }

    bool CXmlProtoImp::add_v(std::string url, std::string tag, int val)
    {
        stringstream ss;
        string str;
        ss << val;
        ss >> str;
        return add_v(url, tag, str);
    }

    void* CXmlProtoImp::get_node(std::string url)
    {
        xmlNodePtr result = NULL;
        xmlXPathObjectPtr xpath = getXPath((const xmlChar*)url.c_str());
        if (xpath != NULL)
        {
            result = xpath->nodesetval->nodeTab[0];
            xmlXPathFreeObject(xpath);
        }
        return result;
    }

    void* CXmlProtoImp::add_v(void* node, std::string tag, std::string val)
    {
        rep_xml_char(val);
        xmlNodePtr pnode = (xmlNodePtr)(node);
        if (doc_->children)//非空树
        {
            if (pnode)//创建子节点
            {
                xmlNodePtr tmp_node = xmlNewNode(NULL, BAD_CAST tag.c_str());
                pnode = xmlAddChild(pnode, tmp_node);
                if (val.length() > 0)//对于空值情况只创建节点，形如：<a1/>
                {
                    xmlNodeSetContent(pnode, (const xmlChar *)val.c_str());//存在会覆盖原有子节点的问题
                }
            }
        }
        else
        {
            pnode = xmlNewNode(NULL, BAD_CAST tag.c_str());
            xmlDocSetRootElement(doc_, pnode);
        }

        return pnode;
    }

    void* CXmlProtoImp::add_v(void* node, std::string tag, int val)
    {
        stringstream ss;
        string str;
        ss << val;
        ss >> str;
        return add_v(node, tag, str);
    }

    bool CXmlProtoImp::parse_url(string url, vector<urlNode>& nodeVec)
    {
        if (url.length() == 0)//空url
        {
            return false;
        }

        stringstream ss(url);
        string sub_str;
        string sub_url = "";
        //分割
        while (getline(ss, sub_str, '/'))
        {
            if (sub_str.length() > 0)
            {
                string sub_url2 = sub_url;
                sub_url += "/" + sub_str;
                urlNode node(sub_str, sub_url, sub_url2, 1);
                nodeVec.push_back(node);
            }
        }
        //处理index下标
        for (int i = 0; i < (int)nodeVec.size(); i++)
        {
            string s = nodeVec[i].name;
            int pos1 = s.find("[");
            int pos2 = s.find("]");

            if ((pos1 < 0) && (pos2 < 0))//未找到
            {
                nodeVec[i].sub_url2 = nodeVec[i].sub_url;
                continue;
            }
            if ((pos1>0) && (pos2 > (pos1 + 1)))//找到
            {
                string name = s.substr(0, pos1);
                string index = s.substr(pos1 + 1, pos2 - pos1 - 1);
                nodeVec[i].name = name;
                nodeVec[i].sub_url2 += "/" + name;
                nodeVec[i].index = atoi(index.c_str());
                if (nodeVec[i].index <= 0)
                {
                    return false;
                }
                else if (nodeVec[i].index == 1)//去掉[1]
                {
                    nodeVec[i].sub_url = nodeVec[i].sub_url2;
                }
            }
            else//url有问题
            {
                return false;
            }
        }

        if (nodeVec[0].index > 1)//多个根
        {
            return false;
        }
        return true;
    }

    //找到的最大级数
    int CXmlProtoImp::found_url(vector<urlNode>& nodeVec)
    {
        if ((doc_->children) &&
            (strcmp((char *)doc_->children->name, nodeVec[0].name.c_str()) != 0))//多根
        {
            return -1;
        }

        int index = 0;
        int len = nodeVec.size();
        for (int i = len - 1; i >= 0; i--)
        {
            int cnt = get_cnt(nodeVec[i].sub_url2);
            if (cnt == 0)//
            {
                if (nodeVec[i].index == 1)
                {
                    continue;
                }
                else//越界
                {
                    return -1;
                }
            }
            else if (nodeVec[i].index > (cnt + 1))//越界
            {
                return -1;
            }
            else if (nodeVec[i].index == (cnt + 1))//找到，不存在
            {
                index = i;
                break;
            }
            else //找到，存在
            {
                index = i + 1;
                break;
            }
        }

        return index;
    }

    bool CXmlProtoImp::build_url(vector<urlNode>& nodeVec, int index, string val)
    {
        int len = nodeVec.size();
        xmlNodePtr node = NULL;

        for (int i = index; i <= len; i++)//i==len 时只设置值，不会创建节点
        {

            if (i == 0)//空树
            {
                node = xmlNewNode(NULL, BAD_CAST nodeVec[i].name.c_str());
                xmlDocSetRootElement(doc_, node);
            }
            else
            {
                if (node == NULL)
                {
                    string result = "";
                    xmlXPathObjectPtr xpath = getXPath((const xmlChar*)nodeVec[i - 1].sub_url.c_str());
                    if (xpath != NULL)
                    {
                        node = xpath->nodesetval->nodeTab[0];
                        xmlXPathFreeObject(xpath);
                    }
                }
                if (i < len)//创建子节点
                {
                    xmlNodePtr tmp_node = xmlNewNode(NULL, BAD_CAST nodeVec[i].name.c_str());

                    //node1 = xmlNewText(BAD_CAST"other way to create content");
                    node = xmlAddChild(node, tmp_node);
                }

            }
        }

        if (val.length() > 0)//对于空值情况只创建节点，形如：<a1/>
        {
            xmlNodeSetContent(node, (const xmlChar *)val.c_str());//存在会覆盖原有子节点的问题
            //以下代码有设置不能覆盖原有设置的问题，不采用
            //xmlNodePtr txt = xmlNewText(BAD_CAST val.c_str());
            //xmlAddChild(node, txt);
        }
        else
        {
            xmlNodeSetContent(node, NULL);
        }
        return true;
    }

    IProto* CXmlProtoImp::copy(int full)
    {
        if (full)
        {
            return new CXmlProtoImp(this);
        }
        return new CXmlProtoImp();
    }

    bool CXmlProtoImp::clear()
    {
        if (doc_)
        {
            xmlFreeDoc(doc_);
            doc_ = NULL;
        }
        doc_ = xmlNewDoc(BAD_CAST "1.0");
        return true;
    }
}
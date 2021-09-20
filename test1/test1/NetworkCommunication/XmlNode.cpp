/** @file    XmlNode.cpp
 *  @note    HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
 *  @brief   xml组装，基于xml的树形结构
 *  @author  gaoyong5
 *  @date    2017-06-21
 */
#include "stdafx.h"
#include "XmlNode.h"

using namespace std;
namespace NetworkCommunication
{
    namespace
    {
       // 特殊字符替换为符合Xml标准的转义字符
        string EscapeXML(string& str_ret)
        {
            size_t size = str_ret.length();
            for(size_t i = 0; i < size; ++i)
            {
                char ch = str_ret.at(i);
                if(ch == '<')
                {
                    str_ret.replace(i,1,"&lt;");
                    i += 3;
                    size += 3;
                }
                else if(ch == '>')
                {
                    str_ret.replace(i,1,"&gt;");
                    i += 3;
                    size += 3;
                }
                else if(ch == '&')
                {
                    str_ret.replace(i,1,"&amp;");
                    i += 4;
                    size += 4;
                }
                else if(ch == '"')
                {
                    str_ret.replace(i,1,"&quot;");
                    i += 5;
                    size += 5;
                }
                else if(ch == '\'')
                {
                    str_ret.replace(i,1,"&apos;");
                    i += 5;
                    size += 5;
                }
            }
            return str_ret;
        }
    }

    // 构造函数
    XmlNode::XmlNode(const std::string& name, const std::string& value)
    {
        name_ = name;
        value_ = value;
    }

    // 添加子节点
    void XmlNode::AddSub(shared_ptr<XmlNode> subnode)
    {
        sub_node_.push_back(subnode);
    }

    // 添加子节点并返回该子节点
    std::shared_ptr<XmlNode> XmlNode::AddSub(const std::string& name)
    {
        auto ptr_sub_node = std::make_shared<XmlNode>(name);
        sub_node_.push_back(ptr_sub_node);
        return ptr_sub_node;
    }

    // 转换成string
    string XmlNode::ToString(bool root)
    {
        string root_info;
        if(root)
        {
            root_info = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
        }

        string xml;
        if(sub_node_.size() > 0)
        {
            for(const auto& itor : sub_node_)
            {
                xml += itor->ToString(false);
            }
        }
        else
        {
            xml = EscapeXML(value_);
        }
        string str_attributes = "";
        for (const auto& data : attributes_)
        {
            str_attributes += " " + data.first + "=" + "\"" + data.second + "\"";
        }
        xml = root_info + "<" + name_  + str_attributes +  ">" + xml + "</" + name_ + ">";
        return xml;
    }
}
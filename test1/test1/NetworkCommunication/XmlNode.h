/** @file    XmlNode.h
 *  @note    HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
 *  @brief   xml组装，基于xml的树形结构
 *  @author  gaoyong5
 *  @date    2017-06-21
 */
#pragma once

#include <string>
#include <vector>
#include <memory>

namespace NetworkCommunication
{
	class  XmlNode : public std::enable_shared_from_this<XmlNode>
    {
    public:
        // 构造函数
        XmlNode(const std::string& name, const std::string& value = "");

        // 添加子节点， key-value类型的 
        template<typename T>
        void AddSub(const std::string& name, const T& value)
        {
            std::stringstream ss;
            ss << value;
            std::string val = ss.str();
            sub_node_.push_back(std::make_shared<XmlNode>(name, val));
        }

        // 添加子节点， key-value类型的，并返回当前节点
        template<typename T>
        std::shared_ptr<XmlNode> AddSubAndReturn(const std::string& name, const T& value)
        {
            std::stringstream ss;
            ss << value;
            std::string val = ss.str();
            std::shared_ptr<XmlNode> ptr_node = std::make_shared<XmlNode>(name, val);
            sub_node_.push_back(ptr_node);
            return ptr_node;
        }

        // 添加子节点，并返回该子节点
        std::shared_ptr<XmlNode> AddSub(const std::string& name);

        template<typename T>
        std::shared_ptr<XmlNode> AddAttributes(const std::string& attribute_name, const T& value)
        {
            std::stringstream ss;
            ss << value;
            std::string val = ss.str();
            attributes_.push_back(make_pair(attribute_name, val));
            // 依据this指针返回shared_ptr通常写法，直接返回std::shared_ptr<XmlNode>(this)会导致内存被释放多次
            return shared_from_this();
        }

        // 添加子节点， 节点类型的
        void AddSub(std::shared_ptr<XmlNode> subnode);

        // 添加子节点， 非智能指针节点类型的，不提供改接口，该接口使用不当会有问题
        //void AddSub(const XmlNode& subnode);

        // 转换为string
        std::string ToString(bool root = true);
    private:
        std::string name_;
        std::string value_;
        std::vector<std::pair<std::string, std::string>> attributes_;
        std::vector<std::shared_ptr<XmlNode>> sub_node_; 
    };
}

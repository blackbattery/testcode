/** @file    XmlNode.h
 *  @note    HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
 *  @brief   xml��װ������xml�����νṹ
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
        // ���캯��
        XmlNode(const std::string& name, const std::string& value = "");

        // ����ӽڵ㣬 key-value���͵� 
        template<typename T>
        void AddSub(const std::string& name, const T& value)
        {
            std::stringstream ss;
            ss << value;
            std::string val = ss.str();
            sub_node_.push_back(std::make_shared<XmlNode>(name, val));
        }

        // ����ӽڵ㣬 key-value���͵ģ������ص�ǰ�ڵ�
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

        // ����ӽڵ㣬�����ظ��ӽڵ�
        std::shared_ptr<XmlNode> AddSub(const std::string& name);

        template<typename T>
        std::shared_ptr<XmlNode> AddAttributes(const std::string& attribute_name, const T& value)
        {
            std::stringstream ss;
            ss << value;
            std::string val = ss.str();
            attributes_.push_back(make_pair(attribute_name, val));
            // ����thisָ�뷵��shared_ptrͨ��д����ֱ�ӷ���std::shared_ptr<XmlNode>(this)�ᵼ���ڴ汻�ͷŶ��
            return shared_from_this();
        }

        // ����ӽڵ㣬 �ڵ����͵�
        void AddSub(std::shared_ptr<XmlNode> subnode);

        // ����ӽڵ㣬 ������ָ��ڵ����͵ģ����ṩ�Ľӿڣ��ýӿ�ʹ�ò�����������
        //void AddSub(const XmlNode& subnode);

        // ת��Ϊstring
        std::string ToString(bool root = true);
    private:
        std::string name_;
        std::string value_;
        std::vector<std::pair<std::string, std::string>> attributes_;
        std::vector<std::shared_ptr<XmlNode>> sub_node_; 
    };
}

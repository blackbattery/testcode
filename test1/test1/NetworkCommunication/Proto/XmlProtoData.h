#ifndef __NETWORKCOMM_XML_PROTO_DATA_H__
#define __NETWORKCOMM_XML_PROTO_DATA_H__
#include<string>
namespace NetworkCommunication
{
	using namespace std;
    struct urlNode{
        std::string name;//����
        std::string sub_url;//ȫurl
        std::string sub_url2;//ȥ�±�url
        int index;//�±�
        urlNode(std::string n, std::string sub, std::string sub2, int i)
        {
            name = n;
            sub_url = sub;
            sub_url2 = sub2;
            index = i;
        }
    };
}



#endif
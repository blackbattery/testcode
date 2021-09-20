#pragma once
#include <string>


namespace NetworkCommunication
{
	using namespace std;
    class  IProto
	{
	public:
		virtual ~IProto(){};
	public:
		virtual bool set_str(std::string str) = 0;
		virtual std::string get_str(int format = 0) = 0;
		virtual int get_cnt(std::string url) = 0;
		virtual std::string get_v(std::string url,std::string defv = "") = 0;
		virtual int get_int_v(std::string url, int defv = -1) = 0;
		virtual bool set_v(std::string url, std::string val) = 0;
		virtual bool set_v(std::string url, int val) = 0;

		virtual bool add_v(std::string url,std::string tag, std::string val) = 0;
		virtual bool add_v(std::string url,std::string tag, int val) = 0;

		virtual void* get_node(std::string url) = 0;
		virtual void* add_v(void* node, std::string tag, std::string val) = 0;
		virtual void* add_v(void* node, std::string tag, int val) = 0;
	public:
		virtual IProto* copy(int full = 0) = 0;
		virtual bool clear() = 0;
	};
}

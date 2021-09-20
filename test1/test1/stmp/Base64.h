#ifndef _BASE64_H__
#define _BASE64_H__


#include <string>


class CBase64
{
public:
	// char�ַ�Base64����,��������\0���ַ���
	static int Base64Encode(const char *in_str, int in_len, char *out_str);

	// char�ַ�Base64����,��������\0���ַ���
	static int Base64Decode(const char *in_str, int in_len, char *out_str);

	
	// ��������\0���ַ���
	static bool Base64Encode(std::string& output, const std::string& input);

	
	// ��������\0���ַ���
	static bool Base64Decode(std::string& output, const std::string& input);

};


#endif 
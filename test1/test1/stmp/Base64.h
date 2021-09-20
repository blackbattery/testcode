#ifndef _BASE64_H__
#define _BASE64_H__


#include <string>


class CBase64
{
public:
	// char字符Base64加密,适用于有\0的字符串
	static int Base64Encode(const char *in_str, int in_len, char *out_str);

	// char字符Base64解密,适用于有\0的字符串
	static int Base64Decode(const char *in_str, int in_len, char *out_str);

	
	// 适用于无\0的字符串
	static bool Base64Encode(std::string& output, const std::string& input);

	
	// 适用于无\0的字符串
	static bool Base64Decode(std::string& output, const std::string& input);

};


#endif 
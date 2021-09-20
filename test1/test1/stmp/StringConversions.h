#ifndef __BASE_STRINGCONVERSIONS_H__
#define __BASE_STRINGCONVERSIONS_H__

#include <string>
// #include "../Global.h"


	
    // 将UTF8编码转换成本地编码，
    // 本地字符集与当前客户端所在的系统设置有关，转换后的类型有可能会导致
    // 当前系统无法显示
     bool UTF8ToLocal(const char* src, size_t src_len, std::string* output);
    
    // 将本地字符编码转换成UTF-8编码。
     bool LocalToUTF8(const char* src, size_t src_len, std::string* output);

    // 将UTF-8编码转换成UTF-16(unicode)
     bool UTF8ToWide(const char* src, size_t src_len, std::wstring* output);

    // 将UTF-16(unicode)编码转换成UTF-8
     bool WideToUTF8(const wchar_t* src, size_t src_len, std::string* output);

    // 将本地编码转换成UTF-16(unicode)编码
     bool LocalToWide(const char* src, size_t src_len, std::wstring* output);

    // 将UTF-16(unicode)编码转换成本地编码
     bool WideToLocal(const wchar_t* src, size_t src_len, std::string* output);

    // Convience functions
     std::string UTF8ToLocal(const std::string& src);
     std::string LocalToUTF8(const std::string& src);
     std::string WideToUTF8(const std::wstring& src);
     std::wstring UTF8ToWide(const std::string& src);
     std::wstring LocalToWide(const std::string& src);
     std::string WideToLocal(const std::wstring& src);

#endif 

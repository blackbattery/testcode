#ifndef __BASE_STRINGCONVERSIONS_H__
#define __BASE_STRINGCONVERSIONS_H__

#include <string>
// #include "../Global.h"


	
    // ��UTF8����ת���ɱ��ر��룬
    // �����ַ����뵱ǰ�ͻ������ڵ�ϵͳ�����йأ�ת����������п��ܻᵼ��
    // ��ǰϵͳ�޷���ʾ
     bool UTF8ToLocal(const char* src, size_t src_len, std::string* output);
    
    // �������ַ�����ת����UTF-8���롣
     bool LocalToUTF8(const char* src, size_t src_len, std::string* output);

    // ��UTF-8����ת����UTF-16(unicode)
     bool UTF8ToWide(const char* src, size_t src_len, std::wstring* output);

    // ��UTF-16(unicode)����ת����UTF-8
     bool WideToUTF8(const wchar_t* src, size_t src_len, std::string* output);

    // �����ر���ת����UTF-16(unicode)����
     bool LocalToWide(const char* src, size_t src_len, std::wstring* output);

    // ��UTF-16(unicode)����ת���ɱ��ر���
     bool WideToLocal(const wchar_t* src, size_t src_len, std::string* output);

    // Convience functions
     std::string UTF8ToLocal(const std::string& src);
     std::string LocalToUTF8(const std::string& src);
     std::string WideToUTF8(const std::wstring& src);
     std::wstring UTF8ToWide(const std::string& src);
     std::wstring LocalToWide(const std::string& src);
     std::string WideToLocal(const std::wstring& src);

#endif 

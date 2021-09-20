#include "../stdafx.h"
#include "StringConversions.h"
#include <Windows.h>
#include <assert.h>


     bool UTF8ToLocal( const char* src, size_t src_len, std::string* output )
    {
        assert(output != NULL && src != NULL);
        if (output == NULL || src == NULL)
        {
            return false;
        }

        int iWideCharLength = MultiByteToWideChar(CP_UTF8, NULL, src, src_len, NULL, NULL);
        wchar_t* pWideCharBuffer = new(std::nothrow) wchar_t[iWideCharLength];
        if (NULL == pWideCharBuffer)
        {
            return false;
        }
        MultiByteToWideChar(CP_UTF8, NULL, src, src_len, pWideCharBuffer, iWideCharLength);

        int len = WideCharToMultiByte(CP_ACP, 0, pWideCharBuffer, iWideCharLength, NULL, NULL, NULL, NULL); 
        char* pLocalCharBuffer = (char*)new(std::nothrow) char[len];
        if (NULL == pLocalCharBuffer)
        {
            delete [] pWideCharBuffer;
            return false;
        }
        WideCharToMultiByte(CP_ACP, 0, pWideCharBuffer, iWideCharLength, pLocalCharBuffer, len, NULL, NULL);

        output->assign(pLocalCharBuffer, len);

        delete [] pWideCharBuffer;
        delete [] pLocalCharBuffer;

        return true;
    }

     bool LocalToUTF8( const char* src, size_t src_len, std::string* output )
    {
        assert(output != NULL && src != NULL);
        if (output == NULL || src == NULL)
        {
            return false;
        }

        int iWideCharLength = MultiByteToWideChar(CP_ACP, NULL, src, src_len, NULL, NULL);
        wchar_t* pWideCharBuffer = new(std::nothrow) wchar_t[iWideCharLength];
        if (NULL == pWideCharBuffer)
        {
            return false;
        }
        MultiByteToWideChar(CP_ACP, NULL, src, src_len, pWideCharBuffer, iWideCharLength);

        int iUtf8CharLength = WideCharToMultiByte(CP_UTF8, 0, pWideCharBuffer, iWideCharLength, NULL, NULL, NULL, NULL); 
        char* pUtf8CharBuffer = (char*)new(std::nothrow) char[iUtf8CharLength];
        if (NULL == pUtf8CharBuffer)
        {
            delete [] pWideCharBuffer;
            return false;
        }
        WideCharToMultiByte(CP_UTF8, 0, pWideCharBuffer, iWideCharLength, pUtf8CharBuffer, iUtf8CharLength, NULL, NULL);

        output->assign(pUtf8CharBuffer, iUtf8CharLength);

        delete [] pWideCharBuffer;
        delete [] pUtf8CharBuffer;

        return true;
    }

     bool UTF8ToWide( const char* src, size_t src_len, std::wstring* output )
    {
        assert(output != NULL && src != NULL);
        if (output == NULL || src == NULL)
        {
            return false;
        }

        int iWideCharLength = MultiByteToWideChar(CP_UTF8, NULL, src, src_len, NULL, NULL);
        wchar_t* pWideCharBuffer = new(std::nothrow) wchar_t[iWideCharLength];
        if (NULL == pWideCharBuffer)
        {
            return false;
        }
        MultiByteToWideChar(CP_UTF8, NULL, src, src_len, pWideCharBuffer, iWideCharLength);

        output->assign(pWideCharBuffer, iWideCharLength);

        delete [] pWideCharBuffer;
        return true;
    }

     bool WideToUTF8( const wchar_t* src, size_t src_len, std::string* output )
    {
        assert(output != NULL && src != NULL);
        if (output == NULL || src == NULL)
        {
            return false;
        }

        int iWideCharLength = WideCharToMultiByte(CP_UTF8, 0, src, src_len, NULL, NULL, NULL, NULL); 
        char* pUtf8CharBuffer = (char*)new(std::nothrow) char[iWideCharLength];
        if (NULL == pUtf8CharBuffer)
        {
            return false;
        }
        WideCharToMultiByte(CP_UTF8, 0, src, src_len, pUtf8CharBuffer, iWideCharLength, NULL, NULL);

        output->assign(pUtf8CharBuffer, iWideCharLength);

        delete [] pUtf8CharBuffer;

        return true;
    }

     bool LocalToWide( const char* src, size_t src_len, std::wstring* output )
    {
        assert(output != NULL && src != NULL);
        if (output == NULL || src == NULL)
        {
            return false;
        }

        int iWideCharLength = MultiByteToWideChar(CP_ACP, NULL, src, src_len, NULL, NULL);
        wchar_t* pWideCharBuffer = new(std::nothrow) wchar_t[iWideCharLength];
        if (NULL == pWideCharBuffer)
        {
            return false;
        }
        MultiByteToWideChar(CP_ACP, NULL, src, src_len, pWideCharBuffer, iWideCharLength);

        output->assign(pWideCharBuffer, iWideCharLength);

        delete [] pWideCharBuffer;

        return true;
    }

     bool WideToLocal( const wchar_t* src, size_t src_len, std::string* output )
    {
        assert(output != NULL && src != NULL);
        if (output == NULL || src == NULL)
        {
            return false;
        }

        int iLocalCharLength = WideCharToMultiByte(CP_ACP, 0, src, src_len, NULL, NULL, NULL, NULL); 
        char* pLocalCharBuffer = (char*)new(std::nothrow) char[iLocalCharLength];
        if (NULL == pLocalCharBuffer)
        {
            return false;
        }
        WideCharToMultiByte(CP_ACP, 0, src, src_len, pLocalCharBuffer, iLocalCharLength, NULL, NULL);

        output->assign(pLocalCharBuffer, iLocalCharLength);
        
        delete [] pLocalCharBuffer;
        return true;
    }

     std::string UTF8ToLocal( const std::string& src )
    {
        std::string output;
        UTF8ToLocal(src.c_str(), src.size(), &output);

        return output;
    }

     std::string LocalToUTF8( const std::string& src )
    {
        std::string output;
        LocalToUTF8(src.c_str(), src.size(), &output);

        return output;
    }

     std::string WideToUTF8( const std::wstring& src )
    {
        std::string output;
        WideToUTF8(src.c_str(), src.size(), &output);

        return output;
    }

     std::wstring UTF8ToWide( const std::string& src )
    {
        std::wstring output;
        UTF8ToWide(src.c_str(), src.size(), &output);

        return output;
    }

     std::wstring LocalToWide( const std::string& src )
    {
        std::wstring output;
        LocalToWide(src.c_str(), src.size(), &output);

        return output;
    }

     std::string WideToLocal( const std::wstring& src )
    {
        std::string output;
        WideToLocal(src.c_str(), src.size(), &output);

        return output;
    }

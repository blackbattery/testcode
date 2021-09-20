
#include "Base64.h"
#include "../modp_base64/modp_b64.h"


/*******************************************************************************
*         Name: base64_encode 
*  Description: BASE64�����㷨ʵ��  
* 
*       Mode   Type         Name         Description
* -----------------------------------------------------------------------------
*        in:   char *      in_str      ԭʼ��������
*              int         in_len      �������ݳ��� 
*    in-out:   �� 
*       out:   char *      out_str     ���ܽ�����
*    return:   int                     ���ܽ������ 
* -----------------------------------------------------------------------------

******************************************************************************/
int CBase64::Base64Encode(const char *in_str, int in_len, char *out_str)
{
	int output_size = modp_b64_encode(out_str, in_str, in_len);

	return output_size;
}


/*******************************************************************************
*         Name: base64_decode 
*  Description: BASE64�����㷨
* 
*       Mode   Type         Name         Description
* -----------------------------------------------------------------------------
*        in:   char *      in_str      ����ԭʼ����
*              int         in_len      �������ݳ��� 
*    in-out:   �� 
*       out:   char *      out_str     �����������
*    return:   int                     ����������ݳ���
* -----------------------------------------------------------------------------
******************************************************************************/
int CBase64::Base64Decode(const char *in_str, int in_len, char *out_str)
{
	int output_size = modp_b64_decode(out_str, in_str, in_len);

	return output_size;
}


bool CBase64::Base64Encode(std::string& output, const std::string& input)
{
	std::string temp;
	temp.resize(modp_b64_encode_len(input.length()));  

													  
	int input_size = static_cast<int>(input.length());
	int output_size = modp_b64_encode(&(temp[0]), input.data(), input_size);
	if (output_size < 0)
	{
		return false;
	}

	temp.resize(output_size);  
	output.swap(temp);
	return true;
}


bool CBase64::Base64Decode(std::string& output, const std::string& input)
{
	std::string temp;
	temp.resize(modp_b64_decode_len(input.size()));

	int input_size = static_cast<int>(input.size());
	int output_size = modp_b64_decode(&(temp[0]), input.data(), input_size);
	if (output_size < 0)
	{
		return false;
	}

	temp.resize(output_size);
	output.swap(temp);
	return true;

}
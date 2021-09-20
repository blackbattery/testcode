// test1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include<iostream>
#include<sstream>
#include<string>
#include<time.h>  
#include <list>  
#include <vector>
#include <map>
#include <numeric>   
#include <algorithm> 
#include<windows.h>
#include <time.h>
#include<ratio>
#include <chrono>
#include <deque>
#include <fstream>
#include <cstring>
#include <utility>
#include <string>
#include <vector>
#include <set>
#include<io.h>
#include<stdlib.h>
#include <regex> 
#include <sstream>
#include <iomanip>
#include"stmp/Smtp.h"
#include "ThreadPool.hpp"



#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"  
#include "rapidjson/stringbuffer.h" 


using namespace std;



struct Student
{
	int No;
	string name;
	string phoneNumber;
	int score;
};
void testDate();
void testnumber();
void listTest();
void StringTest();
void RunTimeTest();
void FooVector(std::initializer_list<int> list);
void vectortoArray();
 int str2num(string s)
 {
	 int num;
     stringstream ss(s);
	  ss >> num;
	 return num;
 }
void forTest();
bool AllisNum(string str);
void timetrans();
void multiMapTest();
bool checkTextChineseOrNumberOrLetter(string str);
void testoperate();
void testURL();
long long GetFileSize(string& strPath);
string CreateFileNameBySource(string& str_source_name, string& str_suffix);
string GetFileCurrentDir(string &filePath, string& findstring);

struct UpgradeDevice
{
	UpgradeDevice()
	{}

	UpgradeDevice(int site_ID, string SerialNumber)
		: siteID(site_ID),
		SerialNo(SerialNumber)

	{
	}

	UpgradeDevice& operator = (const UpgradeDevice& Upgradeinfo)
	{
		siteID = Upgradeinfo.siteID;
		SerialNo = Upgradeinfo.SerialNo;
		return *this;
	}

	int siteID = -1; //远端站点ID
	std::string SerialNo;   //设备序列号

};
vector<pair<int, string>> fun(const string& str)
{
	vector<string> subs;
	vector<pair<int, string>> res;
	int len = str.size();
	for (int i = 0; i < len; i++)
	{
		subs.push_back(str.substr(i));
	}

	int length;
	string sub;

	//i为子串的长度
	for (int i = 1; i <= len / 2; i++)
	{
		for (int j = 0; 2 * i + j <= len; j++)
		{

			if (subs[j + i].substr(0, i) == subs[j].substr(0, i))
			{
				length = 2 * i;
				sub = subs[j].substr(0, i) + subs[j].substr(0, i);
				res.clear();
				res.push_back(make_pair(length, sub));
			}
			//res.push_back(make_pair(length, sub));
		}
	}

	return res;
}

void trim(string &s)
{
   if (!s.empty())
	{
		 s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	}
}




void getFiles(string path, string path2, vector<string>& files)
{
	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	string p, p2;
	if ((hFile = _findfirst(p.assign(path).append(path2).append("*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			//如果不是,加入列表  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("\\"), p2.assign(fileinfo.name).append("\\"), files);
			}
			else
			{
				files.push_back(p.assign(path2).append(fileinfo.name));  //这一行可以给出相对路径
				//files.push_back(p.assign(fileinfo.name) );  //这一行可以给出文件名 
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}
int f(int n)
{
	if (n == 1) return 0;
	if (n == 2) return 1;
	return f(n - 2) + 1;
}
void test12()
{
	string strPath = "C:/Users/xiedeju/Desktop/原始视频.mp4";
	long  long fileSize = GetFileSize(strPath);
	
	
	// 	string directory;
	// 	const size_t last_slash_idx = strPath.rfind('/');
	// 	if (std::string::npos != last_slash_idx)
	// 	{
	// 		directory = strPath.substr(0, last_slash_idx);
	// 	}
	string findString = "/";
	string directory = GetFileCurrentDir(strPath, findString);


	SYSTEMTIME sys;
	GetLocalTime(&sys);
	char chTime[128];
	sprintf(chTime, "%4d/%02d/%02d %02d:%02d:%02d.%03d", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
	std::string Strtime = std::string(chTime);
	cout << Strtime.c_str() << endl;
	std::ostringstream oss;
	oss << sys.wYear << sys.wMonth << sys.wDay << sys.wHour << sys.wMinute << sys.wSecond << sys.wMilliseconds;
	cout << oss.str() << endl;
}

string GetFileCurrentDir(string &filePath, string& findstring)
{
	string directory;
	const size_t last_slash_idx = filePath.rfind(findstring);
	if (std::string::npos != last_slash_idx)
	{
		directory = filePath.substr(0, last_slash_idx);
	}
	return directory;
}
void  testFile()
{
	vector<string> files;
	//char   buffer[MAX_PATH];
	//_getcwd(buffer, MAX_PATH);
	//string filePath;
	//filePath.assign(buffer).append("\\");
	////获取该路径下的所有文件  
	string filePath = "D:\\Person\\";
	getFiles(filePath, "", files);
	char str[30];
	int size = files.size();
	for (int i = 0; i < size; i++)
	{
		cout << files[i].c_str() << endl;
	}
	string fileName_;
	for (auto it : files)
	{
		size_t it_find = it.find("\\");
		if (it_find != it.npos)
		{

		}
		else
		{
			fileName_ = it;
		}
	}
	bool isExcel = false;
	if (!fileName_.empty())
	{
		size_t it_find = fileName_.find(".xls");
		if (it_find != fileName_.npos)
		{
			isExcel = true;
		}
	}
	string fullPath;
	if (isExcel)
	{
		fullPath = filePath + fileName_;
	}
}

typedef pair<int, int> pii;

map<int, int> mp;
map<int, int>::iterator mit;

vector<pii> vc;
vector<pii>::iterator vit;
bool cmp(pii a, pii b) {
	return a.second < b.second;
}


vector<string> split(const string& str, const string& delim) {
	vector<string> res;
	if ("" == str) return res;
	//先将要切割的字符串从string类型转换为char*类型  
	char * strs = new char[str.length() + 1]; //不要忘了  
	strcpy(strs, str.c_str());

	char * d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char *p = strtok(strs, d);
	while (p) {
		string s = p; //分割得到的字符串转换为string类型  
		res.push_back(s); //存入结果数组  
		p = strtok(NULL, d);
	}

	return res;
}

string  round(float src, int bits)
{
	ostringstream  ss;
	ss << setiosflags(ios::fixed) << setprecision(bits) << src;
	return ss.str();
}
struct  CameraWithspeaker
{
	CameraWithspeaker()
	{
	}
	int cameraid;
	int speakerid;
	string strName;
	int areaid;

};

class map_value_finder
{
public:
	map_value_finder(const std::string &cmp_string) :m_s_cmp_string(cmp_string) {}
	bool operator ()(const std::map<int, CameraWithspeaker>::value_type &pair)
	{
		return pair.second.strName == m_s_cmp_string;
	}
private:
	const std::string &m_s_cmp_string;
};


int GetSecsFromHMS(const std::string& strTime)
{
	int h, m, s;
	if (sscanf(strTime.c_str(), "%02d:%02d:%02d", &h, &m, &s) <= 0)
	{
		return 0;
	}
	return h * 3600 + m * 60 + s;
}


int GetSecsOfDay()
{
	time_t timeStampUTC = time(0);
	struct tm tmLocal;
#if (defined(_WIN32) || defined(_WIN32_WCE) || defined(_WIN64))
	_tzset();	// 矫正时区等参数
	if (localtime_s(&tmLocal, &timeStampUTC) == 0)
	{
		return _mkgmtime(&tmLocal) % (3600 * 24);
	}
	return 0;
#elif (defined(__linux__))		
	tzset();	// 矫正时区等参数
	localtime_r(&timeStampUTC, &tmLocal);
	return timegm(&tmLocal) % (3600 * 24);
#endif
}

// 将一个时间转化为time_t，！！！ 其中夏令时设置为-1
time_t GetTime(
	int n_year,
	int n_month,
	int n_day,
	int n_hour,
	int n_min,
	int n_sec)
{
	if (n_year < 0 ||
		n_month < 0 || n_month > 12 ||
		n_day < 0 || n_day > 31 ||
		n_hour < 0 || n_hour > 24 ||
		n_min < 0 || n_min > 60 ||
		n_sec < 0 || n_sec > 60)
	{
		
		return 0;
	}

	tm local_tm;

	local_tm.tm_year = n_year - 1900;
	local_tm.tm_mon = n_month - 1;
	local_tm.tm_mday = n_day;
	local_tm.tm_hour = n_hour;
	local_tm.tm_min = n_min;
	local_tm.tm_sec = n_sec;
	local_tm.tm_isdst = -1;

	return mktime(&local_tm);
}


// 获取一个忽略秒的时间
time_t GetTimeIgnoreSecond(time_t time_source)
{
	if (time_source <= 0)
	{
		return 0;
	}

	tm source_tm = *localtime(&time_source);
	source_tm.tm_sec = 0;
	source_tm.tm_isdst = -1;

	return mktime(&source_tm);
}

// 获取一个忽略分及以后的时间
time_t GetTimeIgnoreMin(time_t time_source)
{
	if (time_source <= 0)
	{
		return 0;
	}

	tm source_tm = *localtime(&time_source);
	source_tm.tm_sec = 0;
	source_tm.tm_min = 0;
	source_tm.tm_isdst = -1;

	return mktime(&source_tm);
}

// 获取一个忽略时及以后的时间
time_t GetTimeIgnoreHour(time_t time_source)
{
	if (time_source <= 0)
	{
		return 0;
	}

	tm source_tm = *localtime(&time_source);
	source_tm.tm_sec = 0;
	source_tm.tm_min = 0;
	source_tm.tm_hour = 0;

	source_tm.tm_isdst = -1;

	return mktime(&source_tm);
}

// 获取一个忽略天及以后的时间
time_t GetTimeIgnoreDay(time_t time_source)
{
	if (time_source <= 0)
	{
		return 0;
	}

	tm source_tm = *localtime(&time_source);
	source_tm.tm_sec = 0;
	source_tm.tm_min = 0;
	source_tm.tm_hour = 0;
	source_tm.tm_mday = 1;

	source_tm.tm_isdst = -1;

	return mktime(&source_tm);
}
bool testSendMail();
bool testSendMail()
{
	//EncryptMode enumEncryptMode = EncryptMode::NO_ENCRYPT;

	//std::string strUserName = "xiedeju";
	//std::string strPassword = "black?857126";
	//std::string strSMTPAddress = "hikml17.hikvision.com.cn";

	//std::string strSenderEmailAddress = "xiedeju@hikvision.com.cn";
	//
	//std::vector<std::string> recvmailList;
	//recvmailList.push_back(strSenderEmailAddress);

	//std::vector<string> vsCCRecipient;    // 邮件抄送者
	//std::vector<string> vsBCCRecipient;   // 邮件暗送者

	//vsCCRecipient.push_back("xiedeju@vip.163.com");

	//int  iSMTPPort = 25;
	//CSmtpSendMail CCSmtpSendMailObject("utf-8");
	//CCSmtpSendMailObject.SetAuthEnable(false);
	//CCSmtpSendMailObject.SetMailEncryptMode(enumEncryptMode);
	//CCSmtpSendMailObject.SetSmtpServer(strUserName, strPassword, strSMTPAddress, iSMTPPort);
	//CCSmtpSendMailObject.SetSendMail(strSenderEmailAddress);
	//CCSmtpSendMailObject.SetSubject("test demo");
	//CCSmtpSendMailObject.SetBodyContent("This is from demo, send by xiedeju "+to_string(time(NULL)));
	//CCSmtpSendMailObject.AddRecvMail(recvmailList);
	//CCSmtpSendMailObject.AddCCRecvMail(vsCCRecipient);
	//CCSmtpSendMailObject.AddBCCRecvMail(vsBCCRecipient);

	//int iConnectTimeOut =1000;
	//int iSendTimeOut = 1000;
	//CCSmtpSendMailObject.SetTimeout(iConnectTimeOut, iSendTimeOut);


	//bool bSendMail = CCSmtpSendMailObject.SendMail();
	//if (!bSendMail)
	//{
	//	CSmtpSendMail::SmtpErrorCode enumSmtpErrorCode = CSmtpSendMail::SmtpErrorCode::SMTP_NO_ERROR;
	//	enumSmtpErrorCode = CCSmtpSendMailObject.GetLastError();
	//	switch (enumSmtpErrorCode)
	//	{
	//	case CCSmtpSendMailObject.SMTP_NETWORK_FAIL_CONNECT://发送邮件服务器连接失败
	//		//Error::SetLastError(Error::ErrorCode(ERROR_CODE(CMS_MODULE_CMS), ERROR_CODE(CMS_EMAIL_SEND_SMTP_SERVER_ERROR)));
	//		break;
	//	case  CCSmtpSendMailObject.SMTP_AUTH_REQUIRED://需要身份认证的服务器，未做身份认证
	//		//Error::SetLastError(Error::ErrorCode(ERROR_CODE(CMS_MODULE_CMS), ERROR_CODE(CMS_EMAIL_SEND_SMTP_NEED_AUTH)));
	//		break;
	//	case  CCSmtpSendMailObject.SMTP_MAILBOX_UNAVAILABLE://要求的邮件操作未完成，邮箱不可用（例如，邮箱未找到，或不可访问）
	//		//Error::SetLastError(Error::ErrorCode(ERROR_CODE(CMS_MODULE_CMS), ERROR_CODE(CMS_EMAIL_SEND_ADDR_NOT_EXIST)));
	//		break;
	//	case  CCSmtpSendMailObject.SMTP_AUTH_FAILED://用户验证失败
	//		//Error::SetLastError(Error::ErrorCode(ERROR_CODE(CMS_MODULE_CMS), ERROR_CODE(CMS_EMAIL_SEND_USER_OR_PWD_ERROR)));
	//		break;
	//	default:
	//		//Error::SetLastError(Error::ErrorCode(ERROR_CODE(CMS_MODULE_CMS), ERROR_CODE(CMS_EMAIL_SEND_FAILED)));
	//		break;
	//	}
	//}

	//return bSendMail;
	return true;


}



std::string&   replace_all(string&   str, const  string&  old_value, const  string&  new_value)
{
	while (true)
	{
		string::size_type   pos(0);
		if ((pos = str.find(old_value)) != string::npos)
		{
			str.replace(pos, old_value.length(), new_value);
		}
		else { break; }
	}
	return   str;
}


std::string str_fmt(const char * _Format, ...)
{

	std::string _str;
	va_list marker = NULL;
	va_start(marker, _Format);

	size_t num_of_chars = _vscprintf(_Format, marker);
	_str.resize(num_of_chars);
	vsprintf_s((char *)_str.c_str(), num_of_chars + 1, _Format, marker);
	va_end(marker);
	return _str;

}


bool ParseRecordStorage(bool& is_enable, int & max_save_time, const string& ret_body)
{
	bool ret = false;

	do
	{
		rapidjson::Document doc;
		if (doc.Parse(ret_body.c_str()).HasParseError())
		{
			break;
		}

		if (doc.HasMember("RecordStorage"))
		{
			rapidjson::Value& record_storage_node = doc["RecordStorage"];

			rapidjson::Value& enable_node = record_storage_node["enable"];
			if (enable_node.IsBool())
			{
				is_enable = enable_node.GetBool();
			}
			else
			{
				break;
			}

			rapidjson::Value& max_save_time_node = record_storage_node["maxSaveTime"];
			if (max_save_time_node.IsInt())
			{
				max_save_time = max_save_time_node.GetInt();
			}
		}
		else
		{
			break;
		}

		ret = true;
	} while (false);

	if (!ret)
	{
		
	}

	return ret;
}

void TestThreadPool();
int _tmain(int argc, _TCHAR* argv[])
{


	
	rapidjson::Document json_doc;
	rapidjson::Document::AllocatorType &allocator = json_doc.GetAllocator(); //获取分配器																	//将当前的Document设置为一个object
	json_doc.SetObject();
	rapidjson::Value ay_time_range(rapidjson::kArrayType);

	rapidjson::Value obj_time_range(rapidjson::kObjectType);
	std::string str_begin_time = "2019-02-03";
	obj_time_range.AddMember("startTime", rapidjson::Value(str_begin_time.c_str(), allocator), allocator);

	obj_time_range.AddMember("enable", true, allocator);
	int number = 30;
	obj_time_range.AddMember("maxSaveTime", number, allocator);
	

	json_doc.AddMember("RecordStorage", obj_time_range, allocator);
	//将JSON格式数据序列化
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	json_doc.Accept(writer);

	string out_str_json = buffer.GetString();
	
	bool is_enable = false;
	int  max_save_time = 0;
	string ret_body;
	bool bParse = ParseRecordStorage(is_enable, max_save_time, out_str_json);
	float temperature = -1;
	

	string str_person_temperature;
	float f_temperature = atof(str_person_temperature.c_str());

	std::string testTemp = str_fmt("%.1f", f_temperature);


	TestThreadPool();




//testSendMail();
	do 
	{
		std::string str_test = R"(
		{
			"employees": [
		{
			"firstName": "Bill",
			"lastName" : "Gates"
		},
		{
			"firstName": "George",
			"lastName" : "Bush"
		},
		{
			"firstName": "Thomas",
			"lastName" : "Carter"
		}
		]})";
		std::cout << str_test << std::endl;
	} while (0);


	do 
	{
		vector<int> m_privateIDVec{1,2,6,5,9,8,9,4};
		
		int *privacyIdArray = new int[m_privateIDVec.size()];
		memcpy(privacyIdArray, &m_privateIDVec[0], m_privateIDVec.size() * sizeof(int));
		
		for (int i = 0; i < m_privateIDVec.size(); i++)
		{
			int num = privacyIdArray[i];
			printf("int* convert to int:%d \n", num);
			std::cout <<"test num: "<< num << std::endl;
		}

	} while (0);

	do 
	{
		map<int, map<int, string>> person_groups;
		int test = 0;
		for (auto &it : person_groups)
		{
			test = it.first;
		}
	} while (0);
	do 
	{
		std::set<int> testSet;
		testSet.insert(1);
		testSet.insert(1);
		testSet.insert(1);
		testSet.insert(1);
		testSet.insert(1);

	} while (0);

	do 
	{
		vector<int> vec;
		vec = { 1, 2, 3, 4, 8, 9, 3, 2, 1, 0, 4, 8 };
		set<int> st(vec.begin(), vec.end());
		vec.assign(st.begin(), st.end());

	} while (0);

	do 
	{
		vector<int> vec;
		vec = { 1, 2, 3, 4, 8, 9, 3, 2, 1, 0, 4, 8 };
		set<int> st(vec.begin(), vec.end());
		vec.assign(st.begin(), st.end());

		
	} while (0);
	map<int, CameraWithspeaker > m_mapCamera;

	for (int i = 0; i < 10; i++)
	{
		CameraWithspeaker cameraWithspeaker;
		cameraWithspeaker.cameraid = i + 1;
		cameraWithspeaker.speakerid = i + 2;
		cameraWithspeaker.strName = to_string(i+3);
		cameraWithspeaker.areaid = i + 4;
		
		m_mapCamera.insert(make_pair(i,cameraWithspeaker));
	}

	string str45 = "100";
	auto it_find = std::find_if(m_mapCamera.begin(), m_mapCamera.end(), map_value_finder(str45));
	if (it_find == m_mapCamera.end())
	{
		printf("not found\n");
	}
		
	else
	{
		printf("found key:%d value:%s\n", it_find->first, it_find->second.strName.c_str());
		m_mapCamera.erase(it_find);
	}
		


	do 
	{
		std::string str_time_ = "14:40:00";
		//int SecsOfDayTime = GetSecsOfDay();
		int SecsFromHMSTime = GetSecsFromHMS(str_time_);
		//int iDiff = GetSecsOfDay() - GetSecsFromHMS(str_time_);	// 当前时间与定时时间相差的秒数
		//int iDiff = SecsOfDayTime - SecsFromHMSTime;
		time_t timeNow = time(nullptr);
		//time_t IgnoreDayTime = GetTimeIgnoreDay(time(nullptr));
		time_t IgnoreHourTime = GetTimeIgnoreHour(time(nullptr));
		//time_t IgnoreMinTime = GetTimeIgnoreMin(time(nullptr));
		//time_t IgnoreSecondTime = GetTimeIgnoreSecond(time(nullptr));
		const int timeIntval = 10 * 60;
		int startImportTime = IgnoreHourTime + SecsFromHMSTime - timeIntval;
		bool bstart = (abs(timeNow - startImportTime) <= 1000) ? true : false;

		if (abs(timeNow - startImportTime) < 20)
		{
			printf("start import person\n");
		}
		

	} while (0);



// 	vector<int> vs_test;
// 	string strTest = round(12.33333333, 1);
	int a = -1; 
	int b = -9999;
	bool isMore = (a >= b) ? true : false;
	string str_test = "2019-06-15";
	string strtest = str_test.substr(2,str_test.length());
 	do 
	{
		string str = "123";
		//string roomno = str.substr(str.length() - 2, str.length());
		//string floorno = str.substr(0, str.length() - 2);

		string room_no_ = (str.substr(str.length() - 2));
	} while (0);
	do 
	{
		for (int i = 0; i < 10; i++)
		{
			if (i == 5)
			{
				break;
				//continue;
			}
			int t = 5;
		}
	} while (0);
	do 
	{
		string str1 = "M135buffer.buffer,|M135data.data1,4.22424|M135data.data6,-0.0714111|M135data.data7,-0.53833|M135data.data11,145.569|M135data.data16,5.0354|M135data.data17,-112.906|M135data.data21,4.34509|M135data.data22,-0.115356|M135data.data23,0|M135data.data24,-14|M135data.data25,49521|M135data.data_lng,0|M135data.data26,-14|M135data.data27,49521|M135data.data_lati,0";
		string str2 = "|This is a test!\n";

		string str = str1 + str2;

		printf(str.c_str());

		vector<string> AllStr = split(str, "|");
		for (int i = 0; i < AllStr.size(); i++)
		{
			vector<string> tempStr = split(AllStr[i], ",");
			if (tempStr.size() >= 2)
			{
				printf("Name:%s  Value:%s\n", tempStr[0].c_str(), tempStr[1].c_str());
			}
		}
	} while (0);
	do 
	{
		string str = "xiaoming_test";
		string name = "10.19.167.101";

		std::string::size_type  npos = str.find("_");
		if (npos != std::string::npos)
		{
			str = str.substr(npos, str.length());
			//str = name + "_" + str;
		}
		
	} while (0);
	do 
    {
		mp[1] = 11;
		mp[2] = 2222;
		mp[3] = 333;
		mp[4] = 4;
		mp[0] = 888888;
		for (mit = mp.begin(); mit != mp.end(); mit++) {
			vc.push_back(pii(mit->first, mit->second));
			cout << mit->first << "  " << mit->second << endl;
		}
		puts("-------------------------------------");
		sort(vc.begin(), vc.end(), cmp);
		map<int, int> cameraDelayTime;
		for (vit = vc.begin(); vit != vc.end(); vit++) {
			cout << vit->first << "  " << vit->second << endl;
			cameraDelayTime.insert(pair<int, int>(vit->first,vit->second));
		}
		cameraDelayTime.clear();
		map<int, int> bet;
		bet[1] = 11;
		bet[2] = 2222;
		bet[3] = 333;
		bet[4] = 4;
		bet[0] = 888888;
		bet[5] = 333;
		vector<pair<int, int>> a(bet.begin(), bet.end());
		sort(a.begin(), a.end(), [](const pair<int, int>& lhs, const pair<int, int>& rhs){return lhs.second > rhs.second; });
		vector<pair<int,int>>::iterator vit;
		for (vit = a.begin(); vit != a.end(); vit++) {
			cout << vit->first << "  " << vit->second << endl;
			cameraDelayTime.insert(pair<int, int>(vit->first, vit->second));
			
		}

    } while (0);
	std::regex reg_pin_code("^[0-9a-zA-Z]+$");
	std::string str_card_no = "0000000805D30120";
	bool is_rex = std::regex_match(str_card_no.c_str(), reg_pin_code);
	if (!std::regex_match(str_card_no.c_str(), reg_pin_code))
	{
		return 0;
	}

	string strTime_ = "2020-01-17T13:02:18+08:00";
	strTime_ = strTime_.substr(0, strTime_.length() - 6);


	//2022-01-08T11:52:01+08:00
	string strsql = "SELECT * FROM evidence WHERE ";
	std::vector<int> element_ids{ 1, 2, 3, 4, 5 };

	if (element_ids.size() > 0)
	{ 
		std::ostringstream oss;
		//oss << "select ID, ResourceID from " << "test" << " where id in( ";
		for (int i = 0, vs_size = static_cast<int>(element_ids.size()); i < vs_size; ++i)
		{
			/*if (i == vs_size - 1)
			{
				oss << "@>" << "array[" << element_ids[i] << "]";
			}
			else
			{
				oss << element_ids[i] << "or";
			}*/

			oss << " camera_ids @>" << " array[" << element_ids[i] << "] " << "or";
		}

		//oss << " and isdeleted = 0";

		std::string exec_sql = oss.str();
		exec_sql = exec_sql.substr(0, exec_sql.length()-2);
		strsql += exec_sql;
	}


	do
	{
		
		std::ostringstream oss;
		oss << "camera_id in( ";
		for (int i = 0, vs_size = static_cast<int>(element_ids.size()); i < vs_size; ++i)
		{
			if (i == vs_size - 1)
			{
				//oss << element_ids[i] << ")";
				oss << " @>" << " array[" << element_ids[i] << "]"  ;
			}
			else
			{
				//oss << element_ids[i] << ",";
				
				oss  << " or";
			}
		}
		std::string exec_sql = oss.str();

	} while (0);
	
	string evidenceCamera = "1,2,3";
	const char * camera_ = "{%s}";
	printf("{%s}\n", evidenceCamera.c_str());
	//std::string camera_info = str_fmt(camera_, evidenceCamera.c_str());


	do{
		string evidenceCamera = "{}";
		evidenceCamera.erase(0, evidenceCamera.find_first_not_of("{ "));
		evidenceCamera.erase(evidenceCamera.find_last_not_of(" }") + 1);


	} while(0);

//2022-01-08 11:52:01
	string strTime = "2022-01-08 11:52:01";
	strTime = strTime.substr(0, 10) + "T" + strTime.substr(11,8);
	time_t  time_now = time(0);


	
}

void TestThreadPool()
{
	ThreadPool threadpool;
	std::thread threads([&threadpool] {
		for (int i = 0; i < 10; i++)
		{
			auto threadid = this_thread::get_id();
			threadpool.AddTask([threadid] {
				std::cout << "thread one id =" << threadid << std::endl;
				}
			);
		}
		});


	std::thread thread2([&threadpool] {
		for (int i = 0; i < 10; i++)
		{
			auto threadid2 = this_thread::get_id();
			threadpool.AddTask([threadid2] {
				std::cout << "thread two id =" << threadid2 << std::endl;
				}
			);
		}
		});


	this_thread::sleep_for(std::chrono::seconds(2));
	getchar();
	threadpool.Stop();
	threads.join();
	thread2.join();

}

string CreateFileNameBySource( string& str_source_name, string& str_suffix)
{	
	size_t pos_suffix = str_source_name.find_last_of(".");
	return str_source_name.replace(pos_suffix, string::npos, str_suffix);
}

void testoperate()
{
	std::deque<string> words{ "one", "two", "nine", "nine", "one", "three", "four", "five", "six" };
	std::sort(std::begin(words), std::end(words), [](const string& s1, const string& s2) { return s1.front() > s2.front(); });
	std::copy(std::begin(words), std::end(words),
		std::ostream_iterator<string> {std::cout, " "}); // six four two one nine nine one three five

	std::cout << std::endl;
	std::vector<int> numbers{ 22, 7, 93, 45, 19, 56, 88, 12, 8, 7, 15, 10 };
	size_t count{ 5 }; // Number of elements to be sorted
	std::vector<int> result(count); // Destination for the results 一 count elements
	std::partial_sort_copy(std::begin(numbers), std::end(numbers), std::begin(result), std::end(result)); std::copy(std::begin(numbers), std::end(numbers), std::ostream_iterator<int>{std::cout, " " });
	std::cout << std::endl;
	std::copy(std::begin(result), std::end(result), std::ostream_iterator <int> {std::cout, " "}); std::cout << std::endl;

	std::vector<double> data{ 1.5, 2.5, 3.5, 4.5 };
	std::cout << "numbers is " << (std::is_sorted(std::begin(numbers), std::end(numbers)) ? "" : "not ") << "in ascending sequence.\n";
	std::cout << "data is " << (std::is_sorted(std::begin(data), std::end(data)) ? "" : "not ") << "in ascending sequence." << std::endl;
	

	std::vector<string> pets{ "cat", "chicken", "dog", "pig", "llama", "coati", "goat" };
	std::cout << "The pets in ascending sequence are:\n";
	std::copy(std::begin(pets), std::is_sorted_until(std::begin(pets), std::end(pets)), std::ostream_iterator<string>{ std::cout, " "});


	size_t count1{};
	int five{ 5 };
	auto start_iter = std::begin(numbers);
	auto end_iter = std::end(numbers);
	while ((start_iter = std::find(start_iter, end_iter, five)) != end_iter)
	{
		++count1;
		++start_iter;
	}
	std::cout << five << " was found " << count1 << " times." << std::endl; // 3 times


	string text{ "The world of searching" };
	string vowels{ "aeiou" };
	auto iter = std::find_first_of(std::begin(text), std::end(text), std::begin(vowels), std::end(vowels));
	if (iter != std::end(text))
		std::cout << "We found '" << *iter << "'." << std::endl; //We found 'e'.



	string found{}; // Records characters that are found
	for (auto iter = std::begin(text); (iter = std::find_first_of(iter, std::end(text), std::begin(vowels), std::end(vowels))) != std::end(text);)
		found += *(iter++);
	std::cout << "The characters \"" << found << "\" were found in text." << std::endl;

	string saying{ "Children should be seen and not heard." };
	auto iter1 = std::adjacent_find(std::begin(saying), std::end(saying));
	if (iter1 != std::end(saying))
		std::cout << "In the following text: \n\"" << saying << "\"\n'" << *iter1 << "' is repeated starting at index position " << std::distance(std::begin(saying), iter1) << std::endl;

	do 
	{

		string text{ "Smith, where Jones had had \"had\", had had \"had had\"." " \"Had had\" had had the examiners\' approval." };
		std::cout << text << std::endl;
		string phrase{ "had had" };
		auto iter = std::find_end(std::begin(text), std::end(text), std::begin(phrase), std::end(phrase));
		if (iter != std::end(text))
			std::cout << "The last \"" << phrase << "\" was found at index " << std::distance(std::begin(text), iter) << std::endl;

	} while (0);

}
bool checkTextChineseOrNumberOrLetter(string str)
{
	int badNum = 0;

	int size = str.length();
	if (size <= 0)
		return false;

	char* pStr = new char[size];

	strcpy(pStr, str.c_str());
	for (int i = 0; i < size; i++)
	{
		if (!(pStr[i] >= 0 && pStr[i] <= 127))
			continue;
		if (ispunct(pStr[i]))
		{
			badNum++;
		}
	}
	delete[] pStr;
	bool res = true;
	if (badNum > 0)
	{
		res = false;
	}
	return res;
}

void multiMapTest()
{
	    vector<UpgradeDevice> vecUpgradeDevice;
		UpgradeDevice upgradeDevice1;
		upgradeDevice1.siteID = 52;
		upgradeDevice1.SerialNo = "DS-1632";
		UpgradeDevice upgradeDevice2;
		upgradeDevice2.siteID = 52;
		upgradeDevice2.SerialNo = "DS-2632";
		UpgradeDevice upgradeDevice3;
		upgradeDevice3.siteID = 53;
		upgradeDevice3.SerialNo = "DS-3632";
		UpgradeDevice upgradeDevice4;
		upgradeDevice4.siteID = 54;
		upgradeDevice4.SerialNo = "DS-4632";
		vecUpgradeDevice.push_back(upgradeDevice1);
		vecUpgradeDevice.push_back(upgradeDevice2);
		vecUpgradeDevice.push_back(upgradeDevice3);
		vecUpgradeDevice.push_back(upgradeDevice4);
		vector<string> strTest;
		strTest.push_back(upgradeDevice1.SerialNo);
		strTest.push_back(upgradeDevice2.SerialNo);
		strTest.push_back(upgradeDevice3.SerialNo);
		strTest.push_back(upgradeDevice4.SerialNo);
		std::map<int, vector<string>> mapTest;
		
		vector<int> siteIDVec;
		siteIDVec.push_back(upgradeDevice1.siteID);
		siteIDVec.push_back(upgradeDevice2.siteID);
		siteIDVec.push_back(upgradeDevice3.siteID);
		siteIDVec.push_back(upgradeDevice4.siteID);
		for (auto &it : siteIDVec)
		{
			auto it_find = mapTest.find(it);
			if (it_find != mapTest.end())
			{
				mapTest.insert(make_pair(it, strTest));
			}
			else
			{
				mapTest[it] = strTest;
			}
		}

		for (auto &it : mapTest)
		{
			int siteID = it.first;
			vector<string> strVec = it.second;
		}
}



void testURL()
{
	std::string strHomePath = "C://Users//";
	std::string::size_type it_find22 = strHomePath.find("C:");
	strHomePath = strHomePath.replace(it_find22, it_find22 +2, "D:");

	//新接口查出来的URL，无法播放
	string strDirectPlayUrl = "rtsp://admin:aGlrMTIzNDUr_encry@127.0.0.1:554/file/cvr://10.41.21.121:8000/0/tracks/?range=20190121T200000-20190121T204348";
	//定制接口查询出来的URL，可以播放
	string strOldDirectPlayUrl = "rtsp://10.19.82.202:554/file/cvr://10.41.21.121:8000/0/tracks/a4243911b0aa4f41a25a77f46317f1e2?range=20190122T121738-20190122T131739";
	string guid = "a4243911b0aa4f41a25a77f46317f1e2";
	string streamIP = "10.19.82.202";
	std::string::size_type itfindRtsp = strDirectPlayUrl.find("rtsp://");
	std::string::size_type itfind1 = strDirectPlayUrl.find("@");

	strDirectPlayUrl = strDirectPlayUrl.replace(itfindRtsp, itfind1 + 1 - itfindRtsp, "rtsp://");
	std::string::size_type it_find = strDirectPlayUrl.find_first_of("127.0.0.1");
	string localHostIp = "127.0.0.1";
	int localHostSize = localHostIp.size();
	strDirectPlayUrl = strDirectPlayUrl.replace(it_find, localHostIp.size(), streamIP);
	std::string::size_type itfind_ = strDirectPlayUrl.find("tracks/");
	std::string::size_type itfind2 = strDirectPlayUrl.find("?");
	string range_ = strDirectPlayUrl.substr(itfind2, strDirectPlayUrl.size() - itfind2);
	string str_ = strDirectPlayUrl.substr(itfind_ + 7, range_.size());
	strDirectPlayUrl = strDirectPlayUrl.replace(itfind_, guid.size() + str_.size(), "tracks/" + guid);
	strDirectPlayUrl.append(str_);
}
void vectortoArray()
{
	vector<int> faceIDVect;
	for (int i = 2; i < 20; i++)
	{
		faceIDVect.push_back(i);
	}
	int *buffer = new int[faceIDVect.size()*sizeof(int)];
	if (!faceIDVect.empty())
	{
		memcpy(buffer, &faceIDVect[0], faceIDVect.size()*sizeof(int));
	}
	for (int i = 0; i < faceIDVect.size(); i++)
	{
		int j = buffer[i];
		cout <<j << endl;
	}

	if (buffer != nullptr)
	{
		delete[] buffer;
	}

}
void timetrans()
{
	struct tm t;   //tm结构指针
	time_t now;  //声明time_t类型变量
	time(&now);      //获取系统日期和时间
	localtime_s(&t, &now);   //获取当地日期和时间

	//格式化输出本地时间
	printf("年：%d\n", t.tm_year + 1900);
	printf("月：%d\n", t.tm_mon + 1);
	printf("日：%d\n", t.tm_mday);
	printf("周：%d\n", t.tm_wday);
	printf("一年中：%d\n", t.tm_yday);
	printf("时：%d\n", t.tm_hour);
	printf("分：%d\n", t.tm_min);
	printf("秒：%d\n", t.tm_sec);
	printf("夏令时：%d\n", t.tm_isdst);
	system("pause");

}
void forTest()
{
	vector<Student> stuVector;
	Student A = {1,"xiaoming","1896666666",60};
	Student B = { 2, "zhang", "1216666666", 80 };
	Student C = { 3, "zhangTeeee", "12366666666", 90 };
// 	stuVector.push_back(A);
// 	stuVector.push_back(B);
// 	stuVector.push_back(C);
	stuVector.emplace_back(A);
	stuVector.emplace_back(B);
	stuVector.emplace_back(C);

	std::map<int, Student> map_stu = { { 1, A }, { 2, B }, { 3, C } };
	 

	for (auto &it : map_stu)
	{
		cout << it.first << "\n" << it.second.name << endl;
	}


	for (const auto &it : stuVector)
	{
		cout << it.No << "\n" << it.name << "\n" << it.phoneNumber << "\n" << it.score << endl;
	}

	for ( auto &it : stuVector)
	{
		it.No = 2;
		cout << it.No << "\n" << it.name << "\n" << it.phoneNumber << "\n" << it.score << endl;
	}
	
}
void FooVector(std::initializer_list<int> list)
{
	vector<int> Vector;
	for (auto it = list.begin(); it != list.end(); ++it)
	{
		Vector.push_back(*it);
	}
}
void RunTimeTest()
{
	DWORD start_time = GetTickCount();
	{
		StringTest();
		testDate();
	}
	DWORD end_time = GetTickCount();
	cout << "The run time is:" << (end_time - start_time)*1.00 / 1000 << "s!" << endl;//输出运行时间
}
void StringTest()
{
	string date = "07/08/2018 12:13:14";
	int length = date.size();
	int iSize = date.length();
	size_t index = date.find("/");
	if (index != string::npos)
	{
		cout << "find it" <<endl;
	}
	else
	{
		cout << " not find it" << endl;
	}
	size_t index_ = date.find_first_of("_");
	if (index_ != string::npos)
	{
		cout << "find it" << endl;
	}
	else
	{
		cout << " not find it" << endl;
	}

}
void listTest()
{

	//创建一个list容器的实例LISTINT   
	typedef list<int> LISTINT;
	//创建一个list容器的实例LISTCHAR   
	typedef list<int> LISTCHAR;
		//用list容器处理整型数据    
		//用LISTINT创建一个名为listOne的list对象   
		LISTINT listOne;
		//声明i为迭代器   
		LISTINT::iterator i;

		//从前面向listOne容器中添加数据   
		listOne.push_front(2);
		listOne.push_front(1);

		//从后面向listOne容器中添加数据   
		listOne.push_back(3);
		listOne.push_back(4);
		listOne.push_back(1236);
		//从前向后显示listOne中的数据   
		cout << "listOne.begin()--- listOne.end():" << endl;
		for (i = listOne.begin(); i != listOne.end(); ++i)
			cout << *i << " ";
		cout << endl;
		vector<int>  countVector;
		for (auto &it : listOne)
		{
			countVector.push_back(it);
			cout << it << " ";
			cout << endl;
		}

		

		for (auto & it : listOne)
		{
			cout << it << " ";
			cout << endl;
		}

		//从后向后显示listOne中的数据   
		LISTINT::reverse_iterator ir;
		cout << "listOne.rbegin()---listOne.rend():" << endl;
		for (ir = listOne.rbegin(); ir != listOne.rend(); ir++) {
			cout << *ir << " ";
		}
		cout << endl;

		//使用STL的accumulate(累加)算法   
		int result = accumulate(listOne.begin(), listOne.end(), 0);
		cout << "Sum=" << result << endl;
		cout << "------------------" << endl;

		//--------------------------   
		//用list容器处理字符型数据   
		//--------------------------   

		//用LISTCHAR创建一个名为listOne的list对象   
		LISTCHAR listTwo;
		//声明i为迭代器   
		LISTCHAR::iterator j;

		//从前面向listTwo容器中添加数据   
		listTwo.push_front('A');
		listTwo.push_front('B');

		//从后面向listTwo容器中添加数据   
		listTwo.push_back('x');
		listTwo.push_back('y');

		//从前向后显示listTwo中的数据   
		cout << "listTwo.begin()---listTwo.end():" << endl;
		for (j = listTwo.begin(); j != listTwo.end(); ++j)
			cout << char(*j) << " ";
		cout << endl;

		for (auto &it : listTwo)
		{
			cout << it << " ";
			cout << endl;
		}
		//使用STL的max_element算法求listTwo中的最大元素并显示   
		j = max_element(listTwo.begin(), listTwo.end());
		cout << "The maximum element in listTwo is: " << char(*j) << endl;
	
}



void testnumber()
{
	string str_name = "000001_xiaocao";
	string name = str_name.substr(0,6);
}


void testDate()
{
	string date = "07/08/2018 12:13:14";

	string date2 = date.substr(0, 2);
	string date3 = date.substr(3, 2);
	string date4 = date.substr(6, 4);
	string date_ = date.substr(6, 4) + "-" + date.substr(3, 2) + "-" + date.substr(0, 2) + " " + date.substr(11, date.length() - 1);
	int length = date.length();
	string date1 = date.substr(0, 3);
	size_t  index = date1.find_first_of("/");
	if (index != string::npos)
	{
		printf("date format is correct");
	}
	else
	{
		printf("date format is correct");

	}


	date2 = date_.substr(0, 4);
	date3 = date_.substr(5, 2);
	date4 = date_.substr(8, 2);

	string backdate = date_.substr(8, 2) + "/" + date_.substr(5, 2) + "/" + date_.substr(0, 4) + " " + date_.substr(11, date_.length() - 1);

	string dateTime = "2018-08-21 12:13:14";
	//"07/08/2018 12:13:14";
	string dateTime_ = dateTime.substr(8, 2) + "/" + dateTime.substr(5, 2) + "/" + dateTime.substr(0, 4) + " " + dateTime.substr(11, dateTime.length() - 1);

	string strResp;
	ostringstream alarmReleasess;
	alarmReleasess << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	alarmReleasess << "<Msg>";
	alarmReleasess << "<Data>";
	alarmReleasess << "<AlarmRelease>";
	alarmReleasess << "<AlarmGuid>" << "123" << "</AlarmGuid>"
		<< "<AlarmState>" << 33 << "</AlarmState>"
		<< "<AlarmStopTime>" << "2018-08-19-12:12:15" << "</AlarmStopTime>";

	alarmReleasess << "</AlarmRelease>";
	alarmReleasess << "</Data>";
	alarmReleasess << "</Msg>";
	strResp = alarmReleasess.str();
	printf("test:%s", strResp.c_str());
}








//判断字符串中是否具有数字
bool AllisNum(string str)
{
	for (int i = 0; i < str.size(); i++)
	{
		int tmp = (int)str[i];
		if (tmp >= 48 && tmp <= 57)
		{
			continue;
		}
		else
		{
			return false;
		}
	}
	return true;
}


long long GetFileSize(string& strPath)
{
	std::ifstream  read_file_(strPath.c_str(), ios::binary);
	long long fileSize = 0;
	if (read_file_.is_open())
	{
		read_file_.seekg(0, ios_base::end);//移动到文件尾
		istream::pos_type file_size = read_file_.tellg();//取得当前位置的指针长度->即文件长度
		read_file_.seekg(0, ios_base::beg);//移动到原来的位置
		return  (long long )file_size;
	}
	return 0;
}


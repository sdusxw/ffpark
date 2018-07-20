#include "common.h"
#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <fstream>

#include <iconv.h>

extern "C"
{
#include <jpeglib.h>
}

std::fstream g_log_file;

std::string get_time_us()
{
	char t[256];
	struct timeval tv;
	struct timezone tz;
	struct tm *p;
	gettimeofday(&tv, &tz);
	p = localtime(&tv.tv_sec);

	sprintf(t, "%04d%02d%02d-%02d:%02d:%02d.%06d", 1900 + p->tm_year,
			1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min,
			p->tm_sec, (int) tv.tv_usec);
	std::string str = t;
	return str;
}

std::string get_time_ms()
{
	char t[256];
	struct timeval tv;
	struct timezone tz;
	struct tm *p;
	gettimeofday(&tv, &tz);
	p = localtime(&tv.tv_sec);

	sprintf(t, "%04d%02d%02d-%02d%02d%02d.%03d", 1900 + p->tm_year,
			1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min,
			p->tm_sec, (int) tv.tv_usec/1000);
	std::string str = t;
	return str;
}

std::string get_time_sec()
{
	char t[256];
	struct timeval tv;
	struct timezone tz;
	struct tm *p;
	gettimeofday(&tv, &tz);
	p = localtime(&tv.tv_sec);

	sprintf(t, "%04d-%02d-%02d %02d:%02d:%02d", 1900 + p->tm_year,
			1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min,
			p->tm_sec);
	std::string str = t;
	return str;
}

std::string get_time_date()
{
	char t[256];
	struct timeval tv;
	struct timezone tz;
	struct tm *p;
	gettimeofday(&tv, &tz);
	p = localtime(&tv.tv_sec);
	sprintf(t, "%04d%02d%02d", 1900 + p->tm_year, 1 + p->tm_mon,
			p->tm_mday);
	std::string str = t;
	return str;
}

void msg_print(std::string msg)
{
	std::cout << "[" << get_time_us() << "]\t" << msg << std::endl;
}

void log_init()
{
	//创建log文件夹
	create_dir("../log");
	std::string log_file_name = "../log/bipc_" + get_time_date() + ".log";
	g_log_file.open(log_file_name.c_str(), std::ios::out | std::ios::app);
	if (g_log_file.good())
		msg_print("日志系统初始化完成。");
	else
		msg_print("日志系统初始化失败，程序将在无日志状态下运行。");
}

void log_output(std::string msg)
{
	if (g_log_file.good())
	{
		std::string log_msg = "[" + get_time_us() + "]" + msg;
		g_log_file << log_msg << std::endl;
	}
}

bool get_local_ip(std::string dev, std::string & str_ip)
{
	bool result = false;
	struct ifaddrs * ifAddrStruct = NULL;
	void * tmpAddrPtr = NULL;

	getifaddrs(&ifAddrStruct);

	while (ifAddrStruct != NULL)
	{
		if (ifAddrStruct->ifa_addr->sa_family == AF_INET) // check it is IP4
		{
			tmpAddrPtr =
					&((struct sockaddr_in *) ifAddrStruct->ifa_addr)->sin_addr;
			char addressBuffer[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer,
			INET_ADDRSTRLEN);
			if (dev == ifAddrStruct->ifa_name)
			{
				str_ip = addressBuffer;
				result = true;
				break;
			}
		}
		ifAddrStruct = ifAddrStruct->ifa_next;
	}
	return result;
}

std::string get_local_ip()
{
	std::string ip_addr="127.0.0.1";
	for(int i = 0;i<10;i++)
	{
		char dev[10]="";
		sprintf(dev, "eth%d", i);
		if(get_local_ip(dev, ip_addr))
		{
			return ip_addr;
		}
	}
	return ip_addr;
}

int create_dir(const char *s_path_name)
{
	char DirName[256];
	strcpy(DirName, s_path_name);
	int i, len = strlen(DirName);
	if (DirName[len - 1] != '/')
		strcat(DirName, "/");

	len = strlen(DirName);

	for (i = 1; i < len; i++)
	{
		if (DirName[i] == '/')
		{
			DirName[i] = 0;
			if (access(DirName, F_OK) != 0)
			{
				if (mkdir(DirName, 0755) == -1)
				{
					perror("mkdir   error");
					return -1;
				}
			}
			DirName[i] = '/';
		}
	}

	return 0;
}

/*********************进程互斥（用文件）***********************************/
bool is_have_instance()
{
	int file_id = open("./bipc.tmp", O_RDWR | O_CREAT, 0640);
	if (file_id < 0)
	{
		return true;
	}

	if (flock(file_id, LOCK_EX | LOCK_NB) < 0)
	{
		return true;
	}

	return false;
}
/*********************end**********************************************/
void JpegInitSource(j_decompress_ptr cinfo)
{
}
boolean JpegFillInputBuffer(j_decompress_ptr cinfo)
{
	return TRUE;
}
void JpegSkipInputData(j_decompress_ptr cinfo, long num_bytes)
{
}
void JpegTermSource(j_decompress_ptr cinfo)
{
}
//JPG解压函数
bool jpeg_uncompress(const char * jpeg_data, int jpeg_size, char *rgb_data,
		int rgb_size, int w, int h, int c)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	struct jpeg_source_mgr jpegSrcManager;
	int ret;
	JSAMPROW rowPointer[1];
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	jpegSrcManager.init_source = JpegInitSource;
	jpegSrcManager.fill_input_buffer = JpegFillInputBuffer;
	jpegSrcManager.skip_input_data = JpegSkipInputData;
	jpegSrcManager.resync_to_restart = jpeg_resync_to_restart;
	jpegSrcManager.term_source = JpegTermSource;
	jpegSrcManager.next_input_byte = (unsigned char*) jpeg_data;
	jpegSrcManager.bytes_in_buffer = jpeg_size;
	cinfo.src = &jpegSrcManager;

	jpeg_read_header(&cinfo, TRUE);
	if (c == 3)
		cinfo.out_color_space = JCS_EXT_BGR;
	else if (c == 1)
		cinfo.out_color_space = JCS_GRAYSCALE;
	jpeg_start_decompress(&cinfo);
	if (cinfo.output_width != (unsigned int) w
			&& cinfo.output_height != (unsigned int) h)
	{
		jpeg_destroy_decompress(&cinfo);
		return false;
	}
	for (int dy = 0; cinfo.output_scanline < cinfo.output_height; dy++)
	{
		rowPointer[0] = (unsigned char *) (rgb_data + w * dy * c);
		ret = jpeg_read_scanlines(&cinfo, rowPointer, 1);
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	return true;
}
std::vector<std::string> string_split(std::string str,std::string pattern)
{
    std::string::size_type pos;
    std::vector<std::string> result;
    str+=pattern;
    int size=str.size();

    for(int i=0; i<size; i++)
    {
        pos=str.find(pattern,i);
        if(pos<size)
        {
            std::string s=str.substr(i,pos-i);
            result.push_back(s);
            i=pos+pattern.size()-1;
        }
    }
    return result;
}

/****************************************************************************************************************************************
 函数简介     UTF-8与GB2312转换
 
 ****************************************************************************************************************************************/

int code_convert(char *from_charset, char *to_charset, char *inbuf,
                 size_t inlen, char *outbuf, size_t outlen)
{
    iconv_t cd;
    int rc;
    char **pin = &inbuf;
    char **pout = &outbuf;
    
    cd = iconv_open(to_charset, from_charset);
    if (cd == 0)
        return -1;
    memset(outbuf, 0, outlen);
    if (iconv(cd, pin, &inlen, pout, &outlen) == -1)
        return -1;
    iconv_close(cd);
    return 0;
}
int utf8togb2312(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
    return code_convert("utf-8", "gb2312", inbuf, inlen, outbuf, outlen);
}

int gb2312toutf8(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
    return code_convert("gb2312", "utf-8", inbuf, inlen, outbuf, outlen);
}

bool utf8togb2312(std::string utf8, std::string &gb2312)
{
    char gbk[1024];
    int len = 1024;
    int ret = utf8togb2312((char*) utf8.c_str(), utf8.length(), gbk, len);
    gb2312 = gbk;
    if (0 == ret)
        return true;
    else
        return false;
}

bool gbk2utf8(std::string gbk, std::string &utf8)
{
    char utf[4096];
    int len = 4096;
    int ret = gb2312toutf8((char*) gbk.c_str(), gbk.length(), utf, len);
    utf8 = utf;
    if (0 == ret)
        return true;
    else
        return false;
}

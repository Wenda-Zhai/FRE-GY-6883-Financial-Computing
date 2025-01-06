#include <cstring>
#include <string> 
#include <iostream>
#include <sstream>  
#include <vector>
#include <locale>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include "curl/curl.h"

using namespace std;
const char* cIWB3000SymbolFile = "Russell_3000_component_stocks.csv";

void populateSymbolVector(vector<string>& symbols)
{
	ifstream fin;
	fin.open(cIWB3000SymbolFile, ios::in);
	string line, name, symbol;
	while (!fin.eof())
	{
		getline(fin, line);
		stringstream sin(line);
		getline(sin, symbol, ',');
		getline(sin, name);
		symbols.push_back(symbol);
	}
}

int write_data(void* ptr, int size, int nmemb, FILE* stream)
{
	size_t written;
	written = fwrite(ptr, size, nmemb, stream);
	return written;
}

struct MemoryStruct {
	char* memory;
	size_t size;
};

void* myrealloc(void* ptr, size_t size)
{
	if (ptr)
		return realloc(ptr, size);
	else
		return malloc(size);
}

int write_data2(void* ptr, size_t size, size_t nmemb, void* data)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct* mem = (struct MemoryStruct*)data;
	mem->memory = (char*)myrealloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory) {
		memcpy(&(mem->memory[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;
	}
	return realsize;
}

int main(void)
{
	vector<string> symbolList;
	populateSymbolVector(symbolList);

	// file pointer to associate with the file Results.txt for storing the data
	FILE* fp;

	const char resultfilename[FILENAME_MAX] = "Results.txt";

	// declare a pointer of CURL
	CURL* handle;

	CURLcode status;

	// set up the program environment that libcurl needs
	curl_global_init(CURL_GLOBAL_ALL);

	// curl_easy_init() returns a CURL easy handle
	handle = curl_easy_init();

	// if everything's all right with the easy handle
	if (handle)
	{
		string url_common = "https://eodhistoricaldata.com/api/eod/";
		string start_date = "2024-11-15";
		string end_date = "2024-11-30";
		string api_token = "67474cf3ba4e10.64365179";  // You must replace this API token with yours

		// vector<string>::iterator itr = symbolList.begin();
		string my_symbol="ACGL";
		vector<string>::iterator itr=find(symbolList.begin(),symbolList.end(),my_symbol);
		if(itr!=symbolList.end())
		{
			struct MemoryStruct data;
			data.memory = NULL;
			data.size = 0;

			string symbol = *itr;
			string url_request = url_common + symbol + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d";
			curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());
			cout<<url_request<<endl;
			//adding a user agent
			curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
			fp = fopen(resultfilename, "ab");
			fprintf(fp, "%s\n", symbol.c_str());
			
			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, fp);
			
			// retrieve data
			status = curl_easy_perform(handle);
			
			fprintf(fp, "%c", '\n');
			fclose(fp);
			// check for errors
			if (status != CURLE_OK) {
				cout << "curl_easy_perform() failed: " << curl_easy_strerror(status) << endl;
				return -1;
			}

			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);

			// retrieve data
			status = curl_easy_perform(handle);

			if (status != CURLE_OK)
			{
				cout << "curl_easy_perform() failed: " << curl_easy_strerror(status) << endl;
				return -1;
			}
			
			stringstream sData;
			sData.str(data.memory);
			string sValue, sDate,sOpenPrice,sClosePrice;
			double dValue = 0,open_Price,close_Price,dailyRtn;
			string line;
			cout << symbol << endl;
			cout<<"Date\t\tOpen\tClose\tDailyRtn\n";
			while (getline(sData, line)) {
				size_t found = line.find('-');
				if (found != std::string::npos) // "std::string::npos" means not found
				{
					// cout << line << endl;
					sDate = line.substr(0, line.find_first_of(',')); //find date
					line.erase(line.find_last_of(',')); //erase last
					line.erase(0,line.find_first_of(',')+1);
					
					string day;
					day=sDate.erase(0,day.find_first_of('-')+1);
					day=day.erase(0,day.find_first_of('-')+1);
					day=day.erase(0,day.find_first_of('-')+1);
					double dday=strtod(day.c_str(),NULL);
					if(dday>=25 && dday<=27)
					{
					sOpenPrice=line.substr(0, line.find_first_of(','));
					open_Price=strtod(sOpenPrice.c_str(),NULL);
					line.erase(0,line.find_first_of(',')+1);
					//get the high prices
					line.erase(0,line.find_first_of(',')+1);
					//get the low price

					line.erase(0,line.find_first_of(',')+1);
					//get the close price

					sClosePrice=line.substr(0,line.find_first_of(','));
					close_Price=strtod(sClosePrice.c_str(),NULL);

					dailyRtn=100*(close_Price-open_Price)/close_Price;
					
					cout<<fixed<<setprecision(2)<<sDate<<'\t'<<open_Price<<'\t'<<close_Price<<'\t'<<dailyRtn<<endl;
					}
				}

			}
			
			free(data.memory);
			data.size = 0;
		}

	}
	else
	{
		cout << "Curl init failed!" << endl;
		return -1;
	}

	// cleanup what is created by curl_easy_init
	curl_easy_cleanup(handle);

	// release resources acquired by curl_global_init()
	curl_global_cleanup();

	return 0;
}

/*
Symbol:ACGL
Date            Open    Close   DailyRtn
2024-11-25      99.91   100.06  0.15
2024-11-26      100.10  100.41  0.31
2024-11-27      100.79  100.74  -0.05
*/
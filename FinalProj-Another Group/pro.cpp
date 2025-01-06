#include "curl/curl.h"
#include <cstring>
#include <fstream>
#include <locale>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>  
#include <stdio.h>
#include <string> 
#include <thread>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
using namespace std;

typedef vector<double> Vector;
typedef vector<Vector> Matrix;
typedef map<string, double> Stock_data;
typedef map<string, Stock_data> Popul_data;
typedef map<string, Matrix> Matrix_;

// const int thread_num = int(thread::hardware_concurrency());
const int thread_num = min(10, int(thread::hardware_concurrency()));

struct EAInfo{
	string date;
	string period_ending;
	double estimate;
	double reported;
	double surprise;
	double surprise_percent;
};

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


int write_data(void* ptr, int size, int nmemb, FILE* stream)
{
	size_t written;
	written = fwrite(ptr, size, nmemb, stream);
	return written;
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

class EODMarketData{
	private:
		unsigned N;
		const char* cIWB3000SymbolFile;
		const char* EAFile;
		const char* MarketCalendarFile;
		string default_start_date;
		
		vector<string> symbol_vector; // stock symbol frome EAFile
		vector<string> EA_date_vector; // EA_date from EAFile
		vector<string> market_calendar_vector; // US trading day
		
		map<string, string> start_date_map; // for every EA_date, calculate start date
		map<string, string> end_date_map; // for every EA_date, calculate end date
		map<string, EAInfo> ea_info_map; // key: stock, value: EAData of stock from EAFile;
		map<string, Stock_data> data_map; // key: stock, value: {date: price} of stock
		
	public:
		EODMarketData();
		
		void populate_ea_info_map(vector<string>& symbols, vector<string>& dates);
		void populate_market_calendar(vector<string>& dates);
		void populate_start_end_date(map<string, string>& start_date_map, map<string, string>& end_date_map);
		
		void populate_IWV_from_web(map<string, Stock_data>& data_map);
		static void populate_stock_data_map_from_web(EODMarketData* eod_ptr , vector<string> symbol_vector, map<string, Stock_data>& data_map);
		Stock_data download_stock_data_from_web(CURL* handle, string symbol, string date, string start_date, string end_date);
		
		int GetN(){return N;}
		vector<string> get_market_calendar_vector(){return market_calendar_vector;}
		map<string, string> get_start_date_map(){return start_date_map;}
		map<string, string> get_end_date_map(){return end_date_map;}
		map<string, EAInfo> get_ea_info_map(){return ea_info_map;}
		map<string, Stock_data> get_data_map(){return data_map;}
		~EODMarketData(){}
};

EODMarketData::EODMarketData()
{
// enter N to retrieve data
// download stock price data from website and store them in data_map
// read csv EAFile and store them in ea_info_map
	cout<<"Please enter N to retrieve 2N+1 days of historical price data for all stocks."<<endl;
	cin>>N;
	while (N<60)
	{
		cout<<"N should not be less than 60, Please enter again."<<endl;
		cin>>N;
	}
	
	cIWB3000SymbolFile = "Russell_3000_component_stocks.csv";
	EAFile = "Russell3000EarningsAnnouncements.csv";
	MarketCalendarFile = "market_calendar.csv";
	default_start_date = "2020-01-01";
	
	populate_ea_info_map(symbol_vector, EA_date_vector);
	populate_market_calendar(market_calendar_vector);
	populate_start_end_date(start_date_map, end_date_map);
	
	populate_IWV_from_web(data_map);

	int size_sub_vector = symbol_vector.size()+1;
	size_sub_vector /= thread_num;
	
	vector<thread> threads;
    for (int i = 0; i < thread_num; i++)
    {
    	vector<string> sub_vector = {symbol_vector.begin()+i*size_sub_vector, symbol_vector.begin()+min((i+1)*size_sub_vector-1, (int)symbol_vector.size())};
        threads.push_back(thread(&populate_stock_data_map_from_web, this, sub_vector, ref(data_map)));
    }
    for (auto &i:threads)
    {
        i.join();
    }
    
    cout<<"download completeed"<<endl;
}

void EODMarketData::populate_ea_info_map(vector<string>& symbols, vector<string>& dates)
{
// read EAFile and change the format of dates
// symbols: store stock symbol in this vector 
// dates: store EA date in this vector
    map<string, string> month_map;
    month_map["JAN"]="01";
    month_map["FEB"]="02";
    month_map["MAR"]="03";
    month_map["APR"]="04";
    month_map["MAY"]="05";
    month_map["JUN"]="06";
    month_map["JUL"]="07";
    month_map["AUG"]="08";
    month_map["SEP"]="09";
    month_map["OCT"]="10";
    month_map["NOV"]="11";
    month_map["DEC"]="12";
    
	ifstream fin;
	fin.open(EAFile, ios::in);
	string line, raw_date, date, symbol, period_ending;
	double estimate, reported, surprise, surprise_percent;
	getline(fin, line); // skip the first line
	//get stock symbol, EA date, surprise%
	while (getline(fin, line))
	{
		symbol = line.substr(0, line.find_first_of(','));
        line.erase(0, line.find_first_of(',')+1);
        
        raw_date = line.substr(0, line.find_first_of(','));
        date = "20"+raw_date.substr(7)+"-"+month_map[raw_date.substr(3,3)]+"-"+raw_date.substr(0,2);
        line.erase(0, line.find_first_of(',')+1);
        
        period_ending = line.substr(0, line.find_first_of(','));
        line.erase(0, line.find_first_of(',')+1);
        
        estimate = strtod(line.substr(0, line.find_first_of(',') + 1).c_str(), NULL);
        line.erase(0, line.find_first_of(',')+1);
        
        reported = strtod(line.substr(0, line.find_first_of(',') + 1).c_str(), NULL);
        line.erase(0, line.find_first_of(',')+1);
        
        surprise = strtod(line.substr(0, line.find_first_of(',') + 1).c_str(), NULL);
        line.erase(0, line.find_first_of(',')+1);
        
        surprise_percent = strtod(line.c_str(), NULL);
        
        EAInfo ea_info = {date, period_ending, estimate, reported, surprise, surprise_percent};
        ea_info_map[symbol] = ea_info;
        // cout<<symbol<<" "<<date<<" "<<period_ending<<" "<<estimate<<" "<<reported<<" "<<surprise<<" "<<surprise_percent<<endl;
        
		symbols.push_back(symbol);
		dates.push_back(date);
	}
	fin.close();
}

void EODMarketData::populate_market_calendar(vector<string>& dates)
{
// read trading dates 
// dates: store trading dates in this vector
	ifstream fin;
	fin.open(MarketCalendarFile, ios::in);
	string date;
	while (getline(fin, date)) {
		dates.push_back(date.substr(0,10));
	}
	fin.close();
}

void EODMarketData::populate_start_end_date(map<string, string>& start_date_map, map<string, string>& end_date_map)
{
// for every EA date, calculate the start and end date to download data using url
// start_date_map, end_date_map: key: EA date, value: start and end date
	set<string> st(EA_date_vector.begin(), EA_date_vector.end());
	set<string>::iterator itr = st.begin();
	for(; itr!=st.end(); itr++)
	{	
		unsigned count = 0;
		for(vector<string>::iterator itr1 = market_calendar_vector.begin(); itr1 != market_calendar_vector.end(); itr1++)
		{
			if ((*itr1).compare(*itr)>=0) 
			{
				
				count++;
				if (count == N+1 || itr1 == market_calendar_vector.end()-1)
				{
					end_date_map[*itr] = *itr1;
					break;
				}
			} 
		}
		
		count = 0;
		for(vector<string>::reverse_iterator itr2 = market_calendar_vector.rbegin(); itr2 != market_calendar_vector.rend(); itr2++)
		{
			if ((*itr2).compare(*itr)<0) 
			{
				count++;
				if (count == N || itr2 == market_calendar_vector.rend()-1)
				{
					start_date_map[*itr] = *itr2;
					break;
				}
			}
		}
	}
}

void EODMarketData::populate_IWV_from_web(map<string, Stock_data>& data_map)
{
// download IWV prices
// data_map: key: symbol, value: Stock_data(which is a map, key: date, value: price)
	CURL* handle;
	curl_global_init(CURL_GLOBAL_ALL);// set up the program environment that libcurl needs 
	handle = curl_easy_init();// curl_easy_init() returns a CURL easy handle 
	if (handle)
	{
		string symbol = "IWV";
		string start_date;
		string end_date;
		string date;
		
		Stock_data stock_date_price = download_stock_data_from_web(handle, symbol, date, start_date, end_date);
		data_map[symbol] = stock_date_price;
	}
	else
	{
		fprintf(stderr, "Curl init failed!\n");
	}
	curl_easy_cleanup(handle); // cleanup since you've used curl_easy_init  
	curl_global_cleanup(); // release resources acquired by curl_global_init()
}

void EODMarketData::populate_stock_data_map_from_web(EODMarketData* eod_ptr , vector<string> symbol_vector, map<string, Stock_data>& data_map)
{
// for every stock in symbol_vector, download data and store stock price data in data_map
// symbol_vector: stocks in EA file
// data_map: key: symbol, value: Stock_data(which is a map, key: date, value: price)
	CURL* handle;
	curl_global_init(CURL_GLOBAL_ALL);// set up the program environment that libcurl needs 
	handle = curl_easy_init();// curl_easy_init() returns a CURL easy handle 

	// if everything's all right with the easy handle... 
	if (handle)
	{
		for(auto itr = symbol_vector.begin(); itr != symbol_vector.end(); itr++)
		{
			string symbol = *itr;
			string date = eod_ptr->get_ea_info_map()[*itr].date;
			string start_date = eod_ptr->get_start_date_map()[date];
			string end_date = eod_ptr->get_end_date_map()[date];
			
			Stock_data stock_date_price = eod_ptr->download_stock_data_from_web(handle, symbol, date, start_date, end_date);
			if (stock_date_price.size() == (unsigned)(2*eod_ptr->N+1)) data_map[symbol] = stock_date_price;
		}
	}
	else
	{
		fprintf(stderr, "Curl init failed!\n");
	}
	curl_easy_cleanup(handle); // cleanup since you've used curl_easy_init  
	curl_global_cleanup(); // release resources acquired by curl_global_init()
}

Stock_data EODMarketData::download_stock_data_from_web(CURL* handle, string symbol, string date, string start_date, string end_date)
{
// given stock symbol, EA date, start date, end date, download stock prices from start date to end date
// if lack prices in some trading days, re download historical data from default start date and select 2n+1 dates 
	string url_common = "https://eodhistoricaldata.com/api/eod/";
	string api_token = "67474cf3ba4e10.64365179";  // You must replace this API token with yours
	string url_request;
	if (start_date.empty() && end_date.empty())
	{
		url_request = url_common + symbol + ".US?" + "from=" + default_start_date + "&api_token=" + api_token + "&period=d";
	}
	else
	{
		url_request = url_common + symbol + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d";
	}
	Stock_data stock_date_price;
			
	struct MemoryStruct data;
	data.memory = NULL;
	data.size = 0;
	
	curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());
	curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
	curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);

	CURLcode result;
	result = curl_easy_perform(handle);// perform, then store the expected code in result
	if (result != CURLE_OK)
	{
		// if errors have occured, tell us what is wrong with result
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
	}
	
	stringstream sData;
	sData.str(data.memory);
	string sValue, sDate;
	double dValue = 0;
	string line;
	
	while (getline(sData, line)) {
		size_t found = line.find('-');
		if (found != std::string::npos)
		{
			sDate = line.substr(0, line.find_first_of(','));
			line.erase(line.find_last_of(','));
			sValue = line.substr(line.find_last_of(',') + 1);
			dValue = strtod(sValue.c_str(), NULL);
			stock_date_price[sDate] = dValue;
		}
	}
	
	free(data.memory);
	data.size = 0;
	if (stock_date_price.size() != 2*N+1 && !start_date.empty()) 
	{
		start_date = "";
		end_date = "";
		stock_date_price = download_stock_data_from_web(handle, symbol, date, start_date, end_date);
		
		Stock_data new_stock_date_price;
		unsigned count = 0;
		for(Stock_data::iterator itr = stock_date_price.begin(); itr != stock_date_price.end(); itr++)
		{
			if ((itr->first).compare(date)>=0) 
			{
				count++;
				new_stock_date_price[itr->first] = itr->second;
				if (count == N+1 || itr == --stock_date_price.end()) break;
			} 
		}
		
		count = 0;
		for(Stock_data::reverse_iterator itr = stock_date_price.rbegin(); itr != stock_date_price.rend(); itr++)
		{
			if ((itr->first).compare(date)<0) 
			{
				count++;
				new_stock_date_price[itr->first] = itr->second;
				if (count == N || itr == --stock_date_price.rend()) break;
			}
		}
		if (new_stock_date_price.size() < 2*N+1) cout <<"Downloading Reminder: There are only " << new_stock_date_price.size() << " historical prices for " + symbol << endl;
		return new_stock_date_price;
	}
	return stock_date_price;
}

class SortingGroup{
    private:
        vector<string> beat; // beat group
        vector<string> miss; // miss group
        vector<string> meet; // meet group
        map<string,string> symbol_group; // key -> symbol; value -> group
    
    protected:
        EODMarketData market_data; // EOD class object
        vector<string> sorting_symbol;
    
    public:
        SortingGroup(){};
        void sort_group(); // function of how to sort the three groups
        vector<string> get_beat(){return beat;}
        vector<string> get_miss(){return miss;}
        vector<string> get_meet(){return meet;}
        map<string,string> get_symbol_group(){return symbol_group;} // key -> symbol; value -> group
        EODMarketData get_market_data(){return market_data;}
        virtual ~SortingGroup(){};
};


class Sample{
    private:
        vector<string> population; // population: beat_group; meet_group; miss_group
    
    public:
        Sample(){}
        Sample(vector<string> const &population_):population(population_){}
        vector<string> get_sample(int i);// get samples; i:random number seed
        void SetPopulation(const vector<string> &population_){population = population_;}
        ~Sample(){}
};



void SortingGroup::sort_group(){
    /*
    sort the stocks into 3 groups, pushed in 3 vectors
    */
    
    vector<double> vector_surprise_percent; // vector for surprise_percent
    string symbol;
    
    map<string, Stock_data> map_string = market_data.get_data_map();
    map<string, EAInfo> map_surprise = market_data.get_ea_info_map();
    for (map<string, Stock_data>::iterator itr = map_string.begin(); itr != map_string.end(); itr++){
        sorting_symbol.push_back(itr -> first);
        vector_surprise_percent.push_back(map_surprise[(itr -> first)].surprise_percent);
    }
    
    // for (vector<string>::iterator itr = sorting_symbol.begin(); itr != sorting_symbol.end(); itr++){
    //     vector_surprise_percent.push_back(map_surprise[(*itr)].surprise_percent);
    // }
    
    vector<double> sorting_surprise_percent = vector_surprise_percent; // copy vector for sorting surprise_percent
    
    // sorting according to surprise value
    sort(sorting_surprise_percent.begin(), sorting_surprise_percent.end());
    
    // Get thresholds
    int third = sorting_surprise_percent.size()/3;
    double threshold1 = sorting_surprise_percent[third]; // get group threshold1
    double threshold2 = sorting_surprise_percent[third*2]; // get group threshold2 which is greater than 1
    
    for (unsigned int i = 0; i < vector_surprise_percent.size(); i++){ // sorting the stocks into 3 groups and fill the symbol map
        symbol = sorting_symbol[i]; 
        double surp = vector_surprise_percent[i];
        if (surp < threshold1)
            {miss.push_back(symbol);symbol_group[symbol] = "miss";}
            else if (surp > threshold2)
            {beat.push_back(symbol);symbol_group[symbol] = "beat";}
            else
            {meet.push_back(symbol);symbol_group[symbol] = "meet";}
    }
}


vector<string> Sample::get_sample(int i){
    /*
    int i: random number seed
    the output is the sampling result which is a vector
    */
    unsigned int M = 80;
    int N = population.size(); // sample 80 from population[0,N-1]
    vector<string> sample;
    set<int> s; // we use set to make sure all the numbers are different
    
    srand(i);
    while( s.size() < M )
    {
       int random_value = (rand() % (N)); // get random value [0,N-1]
       s.insert(random_value);
    }
    
    for (std::set<int>::iterator it=s.begin(); it!=s.end(); it++)
        sample.push_back(population[*it]); // push back the sampling result
    
    return sample;
}

class ReturnCalculation: public SortingGroup{
    private:
        Stock_data IWVReturn;
        Popul_data stockdata = market_data.get_data_map();
        unsigned int N = market_data.GetN();
        Matrix_ Final_Marix;
        
    
    public:
        ReturnCalculation(){};
        
        Popul_data GetStockdata(){return stockdata;}
        Stock_data IWVReturnCal(); // get IWV return
        Popul_data StockreturnCal(vector<string> symbolvector_); // get stock return
        
        Matrix ARCal(Popul_data Stockreturn_, Stock_data IWVReturn_); //Calculate Abnormal Return
        Vector AARCal(Matrix AR_); //Calculate Average Abnormal Return
        Vector CAARCal(Vector AAR_); //Calculate Cumulative Average Abnormal Return
        Popul_data CRCal(); // cumulative stock return
        void AveCal(vector<string> beat_group_, vector<string> meet_group_, vector<string> miss_group_); //Calculate Average AAR, CAAR, AAR_STD, CAAR_STD and fill them into Final_marix
        
        void filling_final_matrix(string groupname, vector<string> group_); //Repeate function AveCal 3 times(with different group) to get Final_marix(a map) with group names as keys and matrices as values(Each matrix contains four vectors, respectively AAAR, AAR_STD, ACAAR, CAAR_STD.)
        Matrix_ GetMatrix_(){return Final_Marix;} //Get Final_Marix
        unsigned int GetN() {return N;} //Get number of N
        ~ReturnCalculation(){}
};


// Operators overloading session
Vector operator+(const Vector& V, const Vector& W)
{
    int d = (int)V.size();
    Vector U(d);
    
    for (int j = 0; j < d; j++) U[j] = V[j] + W[j];
    
    return U;
}
    
    
Vector operator*(const double& a, const Vector& V)
{
    int d = (int)V.size();
    Vector U(d);
    
    for (int j = 0; j < d; j++) U[j] = a * V[j];
    
    return U;
}


Vector operator*(const Vector& V, const Vector& W)
{
    int d = (int)V.size();
    Vector U(d);
    
    for (int j = 0; j < d; j++) U[j] = V[j] * W[j];
    
    return U;
}

Vector operator^(const Vector& C, const double& a)
{
    int m = (int)C.size();
    Vector W(m);
    
    for (int i = 0; i < m; i++)
    {
        W[i] = pow(C[i], a);
    }
    
    return W;
}


// Calculation session
Stock_data ReturnCalculation::IWVReturnCal()
{
/*
curr -> second points to the current stock price which will be replaced with stock return. 
cur starts with the second location.
prev -> second points to the price of the previous day. 
erase the frist day from the dataset

*/
    Stock_data IWVReturn = stockdata["IWV"];
    Stock_data IWVReturn_copy = IWVReturn;
    Stock_data :: iterator prev = IWVReturn_copy.begin();
    Stock_data :: iterator curr = std::next(IWVReturn.begin(), 1);
    
    for (; curr != IWVReturn.end(); curr ++)
    {
        curr->second = (curr->second / prev->second) - 1;
        prev ++;
    } // IWV return
    
    IWVReturn.erase(IWVReturn.begin());
    
    return IWVReturn;
};


Popul_data ReturnCalculation::StockreturnCal(vector<string> symbolvector_)
{
/*
The first iteration gives us the price data for each stock.
The second iteration is used to calculate every stock’s returns
for every stock, d is the data of the prices
similar methodology with IWVReturnCal
*/
    Popul_data Stockreturn;
    vector<string>::iterator itor;
    
    for (itor = symbolvector_.begin(); itor != symbolvector_.end();)
    {
        if (*itor == "IWV") 
        {
            itor = symbolvector_.erase(itor);
        }
        else
        {
            itor++;
        }
    } // remove IWV from the data
    
    vector<string>::iterator itr;
    for (itr = symbolvector_.begin(); itr != symbolvector_.end(); itr ++)
    {   
        Stock_data d = stockdata[*itr]; // *itr1: symbol
        Stock_data d_copy = d;
        Stock_data :: iterator prev = d_copy.begin();
        Stock_data :: iterator curr = std::next(d.begin(), 1);
        
        for (; curr != d.end(); curr ++)
        {
            curr->second = curr->second / prev->second - 1;
            prev ++;
        }
        
        d.erase(d.begin());
        Stockreturn[*itr] = d; // stock return
    }
    
    return Stockreturn;
}


Matrix ReturnCalculation :: ARCal(Popul_data Stockreturn_, Stock_data IWVReturn_)
{
/*
1. Matching the trading days
2. Subtracting (using formula)
3. Converting map into matrix
the first if function help us to exit the iteration once all the stock trading days have been matched with IWV trading days
*/
    map<string,Stock_data> :: iterator itr1;
    map<string,Vector> return_map;
    for (itr1 = Stockreturn_.begin(); itr1 != Stockreturn_.end(); itr1++)
    {
        Vector a;
        Stock_data :: iterator itr2;
        Stock_data d = Stockreturn_[itr1->first];
        Stock_data :: iterator itr3 = d.begin();
        
        for (itr2 = IWVReturn_.begin(); itr2 != IWVReturn_.end(); itr2++)
        {
            if (a.size() == 2 * N)
            {
                break;
            }
            else if (itr2->first == itr3->first)
            {
                double return_day = itr3->second-itr2->second;
                a.push_back(return_day);
                itr3++;
            }
        }
        return_map[itr1->first] = a;
    }
    
    Matrix AbnormalReturn(return_map.size());
    map<string, Vector> :: iterator itor;
    int ii;
    for (itor = return_map.begin(), ii = 0; itor != return_map.end(); itor++, ii++)
    {
        AbnormalReturn[ii].resize(2 * N);
        AbnormalReturn[ii] = itor->second;
    }
    
    return AbnormalReturn;
}


Vector ReturnCalculation::AARCal(Matrix AR_)
{
    /*
    Calculate Average abnormal return;
    param AR_: Matrix of abnormal return of stocks;
    return 100 * AAR: AAR is the vector of average abonrmal return, multiply it by 100 to make its elements percentage numbers;
    */
    int d = 2 * N;
    Vector AAR;
    AAR.resize(d);
    
    for (int i = 0; i < d; i++) AAR[i] = 0.0;
    
    for (unsigned int i = 0; i < AR_.size(); i++)
    {
        AAR = (1.0 / (i + 1.0)) * (i * AAR + AR_[i]);
    }
    
    return 100 * AAR;
}


Vector ReturnCalculation::CAARCal(Vector AAR_)
{       
    /*
    Calculate Cumulative Average abnormal return;
    param AAR_: Vector of Average abnormal return;
    return CAAR: CAAR is the vector of Cumulative Average abnormal return;
    */
    int d = AAR_.size();
    Vector CAAR;
    CAAR.resize(d);
    
    for (int i = 0; i < d; i++) CAAR[i] = 0.0;
    
    double COUNT = 0;
    for (int j = 0; j < d; j++)
    {
        COUNT = COUNT + AAR_[j];
        CAAR[j] = COUNT;
    }
    
    return CAAR;
}


void ReturnCalculation::AveCal(vector<string> beat_group_, vector<string> meet_group_, vector<string> miss_group_)
{
    /*
    Filling final matrix(a map) with group names as keys and matrices as values(Each matrix contains four vectors, respectively AAAR, AAR_STD, ACAAR, CAAR_STD.)
    param vector<string> beat_group_: A vector contains names of 80 sample stocks of a group.
    */
    filling_final_matrix("Beat_group", beat_group_);
    filling_final_matrix("Meet_group", meet_group_);
    filling_final_matrix("Miss_group", miss_group_);
}


void ReturnCalculation::filling_final_matrix(string groupname, vector<string> group_)
{
    /*
    Repeate sampling for 40 times. Calculate average AAR, CAAR, AAR_STD, CAAR_STD of 40 repetition and fill them into Final_Marix.
    param groupname: The name of the group, used as key in the Final_marix.
    param vector<string> group_: A vector contains names of 80 sample stocks of a group.
    */
    int d = 2 * N;
    Stock_data IWVReturn = IWVReturnCal();
    
    Vector AAAR_group;
    AAAR_group.resize(d);
    Vector ACAAR_group;
    ACAAR_group.resize(d);
    Sample sample_(group_);
    Vector AAR_group_sq;
    AAR_group_sq.resize(d);
    Vector CAAR_group_sq;
    CAAR_group_sq.resize(d);
    Vector AAR_std_group;
    AAR_std_group.resize(d);
    Vector CAAR_std_group;
    CAAR_std_group.resize(d); 
    
    for (int m = 0; m < 40; m++)
    {
        vector<string> sample = sample_.get_sample(m);
        Popul_data Stockreturn = StockreturnCal(sample);
        Matrix AR = ARCal(Stockreturn, IWVReturn);
        Vector AAR = AARCal(AR);
        Vector CAAR = CAARCal(AAR);
        AAAR_group = (1.0/(m+1.0)) * (m * AAAR_group + AAR);
        ACAAR_group = (1.0/(m+1.0)) * (m * ACAAR_group + CAAR);
        AAR_group_sq = (1.0/(m+1.0)) * (m * AAR_group_sq + AAR * AAR);
        CAAR_group_sq = (1.0/(m+1.0)) * (m * CAAR_group_sq + CAAR * CAAR);
    }
    
    AAR_std_group = (AAR_group_sq + (-1.0) * AAAR_group * AAAR_group) ^ 0.5;
    CAAR_std_group = (CAAR_group_sq + (-1.0) * ACAAR_group * ACAAR_group) ^ 0.5;
    Matrix Matrix_group;
    Matrix_group.resize(4);
    
    for(int i=0;i<4;i++) Matrix_group[i].resize(2*N);
    
    Matrix_group[0] = AAAR_group;
    Matrix_group[1] = AAR_std_group;
    Matrix_group[2] = ACAAR_group;
    Matrix_group[3] = CAAR_std_group;
    Final_Marix[groupname] = Matrix_group;
}


Popul_data ReturnCalculation::CRCal()
{
    /*
    The first iteration gives us the price data for each stock.
    The second iteration is used to calculate every stock’s returns
    Use variable count to keep track of the cumulative return
    */
    Popul_data CR = StockreturnCal(sorting_symbol);
    Popul_data :: iterator itr1 = CR.begin();
    
    for (; itr1 != CR.end(); itr1 ++)
    {
        Stock_data tem = itr1->second;
        double count = 0.0;
        Stock_data :: iterator itr2 = tem.begin();
        
        for (; itr2 != tem.end(); itr2 ++)
        {
            count = count + 100*itr2->second;    
            itr2->second = count;
        }
        
        CR[itr1->first] = tem;
    }
    return CR; // Cumulative Stock Return
}



class gnuplot{
    private:
        vector<double> yData1;
        vector<double> yData2;
        vector<double> yData3;
        int dataSize;
        
    public:
        gnuplot(const vector<double> &yData1_, const vector<double> &yData2_, const vector<double> &yData3_)
        {
            yData1 = yData1_;
            yData2 = yData2_;
            yData3 = yData3_;
            dataSize = yData1_.size();
        }
        ~gnuplot(){}
        vector<double> GetData1(){return yData1;}
        vector<double> GetData2(){return yData2;}
        vector<double> GetData3(){return yData3;}
        void plotResults() const;
};

void gnuplot::plotResults() const
{   
    // Create X axis array
    vector<int> xData;
    int N = (dataSize)/2;
    
    for (int i = -N+1; i <= N; i++) {
        xData.push_back(i);
    }
    
    // Configure gnuplot
    FILE *gnuplotPipe,*tempDataFile;
    const char *tempDataFileName1 = "Beat_Group";
    const char *tempDataFileName2 = "Meet_Group";
    const char *tempDataFileName3 = "Miss_Group";
    
    const char *title = "CAAR Outcome";
    const char *xlabel = "Date";
    const char *ylabel = "CAAR";
    
    gnuplotPipe = popen("gnuplot -persist", "w");
    fprintf(gnuplotPipe, "set grid\n");
    fprintf(gnuplotPipe, "set title '%s'\n", title);
    // Vertical line at x=0
    fprintf(gnuplotPipe, "set arrow from 0,graph(0,0) to 0,graph(1,1) nohead lc rgb 'red'\n");
    fprintf(gnuplotPipe, "set xlabel '%s'\n set ylabel '%s'\n", xlabel, ylabel);
    
    // Plot
    if (gnuplotPipe) {
        fprintf(gnuplotPipe,"plot \"%s\" with lines, \"%s\" with lines, \"%s\" with lines\n", tempDataFileName1, tempDataFileName2, tempDataFileName3);
        fflush(gnuplotPipe);
        
        // Figure 1
        tempDataFile = fopen(tempDataFileName1,"w");
        for (int i=0; i < dataSize; i++) {
            if (isnan(yData1[i])){
                fprintf(tempDataFile,"%d %lf\n",xData[i],0.0);
            }
            else{
                fprintf(tempDataFile,"%d %lf\n",xData[i],yData1[i]);
            }
        }
        fclose(tempDataFile);
        
        // Figure 2
        tempDataFile = fopen(tempDataFileName2,"w");
        for (int i=0; i < dataSize; i++) {
            if (isnan(yData2[i])){
                fprintf(tempDataFile,"%d %lf\n",xData[i],0.0);
            }
            else{
                fprintf(tempDataFile,"%d %lf\n",xData[i],yData2[i]);
            }
        }
        fclose(tempDataFile);
        
        // Figure 3
        tempDataFile = fopen(tempDataFileName3,"w");
        for (int i=0; i < dataSize; i++) {
            if (isnan(yData3[i])){
                fprintf(tempDataFile,"%d %lf\n",xData[i],0.0);
            }
            else{
                fprintf(tempDataFile,"%d %lf\n",xData[i],yData3[i]);
            }
        }
        fclose(tempDataFile);
        
        // Exit
        printf("Plotting, press enter to continue...");
        getchar();
        
        // Clear
        // remove(tempDataFileName1);
        // remove(tempDataFileName2);
        // remove(tempDataFileName3);
        
        // fprintf(gnuplotPipe,"Plot Exited \n");
    } 
    else 
    {        
        printf("gnuplot not found...");    
    }
}

int main() 
{
    cout << "----------------------------------------------------------------------------" << endl << endl;
    cout << "[Initialize the number N (<=80) and (>=40)]" << endl << endl;
    
    ReturnCalculation *RC_ptr = new ReturnCalculation();
    RC_ptr->sort_group();
    
    int N = RC_ptr->GetN();
    vector<string> beat_group;
    vector<string> meet_group;
    vector<string> miss_group;
    beat_group = RC_ptr->get_beat();
    meet_group = RC_ptr->get_meet();
    miss_group = RC_ptr->get_miss();
    
    RC_ptr->AveCal(beat_group, meet_group, miss_group);


    // Menu session
    bool running = true; 
    string soption;
    string sgroup;
    string ticker;
    
    while (running) {
        cout << endl;   
        cout << "-------------------------------------MENU-------------------------------------" << endl << endl;
        cout << "1. Reset the number N" << endl;
        cout << "2. Pull info of a single stock" << endl;
        cout << "3. Show AAR, AAR-STD, CAAR and CAAR-STD for one group" << endl;
        cout << "4. Show the gnuplot graph with CAAR for all 3 groups" << endl;
        cout << "5. Exit" << endl << endl;
        cout << "Please Enter Number 1-5: ";
        cin >> soption;
        getchar();
        cout << endl;
        cout << "-----------------------------------FUNCTION-----------------------------------" << endl << endl;

        while ((soption.at(0) > '5' || soption.at(0) <= '0')) {
            cout << "------------------------------------REMINDER----------------------------------" << endl << endl;
            cout << "Invalid option. Please input a number from 1 to 5" << endl;
            cout << "1. Reset the number N" << endl;
            cout << "2. Pull info of a single stock" << endl;
            cout << "3. Show AAR, AAR-STD, CAAR and CAAR-STD for one group" << endl;
            cout << "4. Show the gnuplot graph with CAAR for all 3 groups" << endl;
            cout << "5. Exit" << endl << endl;
            cout << "Please Enter Number 1-5: ";
            cin >> soption;
            getchar();
            cout << endl;
            cout << "-----------------------------------FUNCTION-----------------------------------" << endl << endl;
        }


        int option = stoi(soption);
        switch (option) {
            
            case 1:{
                cout << "[Reset the number N]" << endl << endl;
                delete RC_ptr;
                RC_ptr = new ReturnCalculation();
                
                N = RC_ptr->GetN();
                
                RC_ptr->sort_group();
                beat_group = RC_ptr->get_beat();
                meet_group = RC_ptr->get_meet();
                miss_group = RC_ptr->get_miss();
                RC_ptr->AveCal(beat_group, meet_group, miss_group);
                
                break;
            }

            case 2: {
                cout << "[Pull info of one stock]" << endl << endl;
                cout << "Please enter stock ticker (print '0' to get back):";
                cin >> ticker;
                getchar();
                cout << endl;
                
                // Get back to the main menu
                if (ticker.at(0) == '0') break;
                
                // 1. Stock dates and daily prices (121)
                Stock_data info_map = RC_ptr->GetStockdata()[ticker];
                
                cout << "1. " << ticker << " - " << "2N+1 Dates and respective prices are: " << endl << endl;
                cout << left << setw(14) << "Dates"
                    << setw(12) << "Prices"
                    << endl;
                    
                for (Stock_data::iterator itr=info_map.begin();itr != info_map.end();itr++) {
                    cout << left << setw(14) << itr->first
                        << setw(12) << itr->second
                        << endl << endl;
                }

                // 2. Cumulative return
                Sample symbol_group;
                map<string,string> group_map = RC_ptr->get_symbol_group();
        
                Popul_data stock_cum_return = RC_ptr->CRCal();
                Stock_data target_stock_cum_return = stock_cum_return[ticker];
                
                cout << "2. " << ticker << " - " << "2N+1 days' cumulative returns are: " << endl << endl;
                cout << left << setw(14) << "Dates"
                    << setw(12) << "Cum Returns" 
                    << endl << endl;
                    
                Stock_data::iterator itr=target_stock_cum_return.begin();    
                for (int i = 0; i < 2*N; i++) {
                    cout << left << setw(14) << itr->first
                        << setw(12) << itr->second
                        << endl << endl;
                    itr++;
                }

                // 3. Group name
                cout << "3. " << ticker << " - " << "Group Name is: " << group_map[ticker] << endl << endl;
                
                // 4. Other Info
                // EODMarketData market_data = RC_ptr->get_market_data();
                EAInfo ea_info_struct = RC_ptr->get_market_data().get_ea_info_map()[ticker];
                
                cout << "4. " << ticker << " - " << "Other related information: " << endl;
                cout << "- Earning Announcement Date: " << ea_info_struct.date << endl;
                cout << "- Period Ending: " << ea_info_struct.period_ending << endl;
                cout << "- Estimated: " << ea_info_struct.estimate << endl;
                cout << "- Reported Earnings: " << ea_info_struct.reported << endl;
                cout << "- Surprise: " << ea_info_struct.surprise << endl;
                cout << "- Surprise %: " << ea_info_struct.surprise_percent << endl;
                cout << endl;
                
                break;
            }

            case 3: {
                cout << "[Show AAR, AAR-STD, CAAR and CAAR-STD for one group]" << endl << endl;
                cout << "Please enter the group selection (print '0' to get back): " << endl
                    << "1. Beat group" << endl
                    << "2. Meet group" << endl
                    << "3. Miss group" << endl
                    << endl;
                    
                cout << "Please Enter Number 1-3: ";
                cin >> sgroup;
                getchar();
                cout << endl;
                
                if (sgroup.at(0) == '0') break;

                while ((sgroup.at(0) > '3' || sgroup.at(0) <= '0')) {
                    cout << "Input a Number from 1 to 5 only" << endl << endl;
                    cout << "Please enter the group selection: " << endl
                        << "1. Beat group" << endl
                        << "2. Meet group" << endl
                        << "3. Miss group" << endl
                        << endl;
                    cout << "Please Enter Number 1-3: ";
                    cin >> sgroup;
                    getchar();
                    cout << endl;
                }
                
                
                Matrix_ metrics_matrix = RC_ptr->GetMatrix_();
                Vector AAR, AAR_STD, CAAR, CAAR_STD;
                
                int groupnum = stoi(sgroup);
                string groupname;
                
                if (groupnum == 1) { groupname = "Beat_group"; }
                else if(groupnum == 2) { groupname = "Meet_group"; }
                else { groupname = "Miss_group"; }
                
                cout << "Detailed Information for " << groupname << ":" << endl << endl;
                cout << left << setw(10) << "Dates"
                    << setw(12) << "AAR"
                    << setw(12) << "AAR STD"
                    << setw(12) << "CAAR"
                    << setw(12) << "CAAR STD"
                    << endl << endl;
                    
                for (int i = 0; i < 2*N; i++) {
                    cout << left << setw(10) << i-int(N)+1
                        << setw(12) << metrics_matrix[groupname][0][i]
                        << setw(12) << metrics_matrix[groupname][1][i]
                        << setw(12) << metrics_matrix[groupname][2][i]
                        << setw(12) << metrics_matrix[groupname][3][i]
                        << endl << endl;
                }
                
                break; 
            }

            case 4: {
                cout << "[Show the gnuplot graph with CAAR for all 3 groups]" << endl << endl;
                Matrix_ metrics_matrix = RC_ptr->GetMatrix_();
                Vector beat_ACAAR, meet_ACAAR, miss_ACAAR;
                beat_ACAAR = metrics_matrix["Beat_group"][2];
                meet_ACAAR = metrics_matrix["Meet_group"][2];
                miss_ACAAR = metrics_matrix["Miss_group"][2];

                gnuplot CAAR_plot(beat_ACAAR, meet_ACAAR, miss_ACAAR);
                CAAR_plot.plotResults();
                    
                break; 
            }

            case 5: {
                running = false;
                cout << "[Program Exited]" << endl << endl;
                delete RC_ptr;
                RC_ptr = NULL;
                
                break;
            }
            return 0;
        };                 
    };
    return 0;
}
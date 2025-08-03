#include "filereader.h"
#include <iostream>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <vector>
#include <ctime>
#include <sstream>
#include <map>
#include <string>
#include <limits.h>


void Print_Output(const bool& prnt_act,const bool& out_act, std::string& line_from_file, std::ofstream &output_file)
{
    if (prnt_act)
    {
        std::cout << line_from_file << "\n";
    }                
    if(out_act)
    {
        output_file << line_from_file << "\n";
    }
}

int MonthToVal(std::string mnth)
{
    std::map <std::string, int> months;
    months["Jan"] = 1;
    months["Feb"] = 2;
    months["Mar"] = 3;
    months["Apr"] = 4;
    months["May"] = 5;
    months["Jun"] = 6;
    months["Jul"] = 7;
    months["Aug"] = 8;
    months["Sep"] = 9;
    months["Oct"] = 10;
    months["Nov"] = 11;
    months["Dec"] = 12;

    return (months[mnth]-1);
}

std::time_t Time_in_Sec(std::string time)
{
    std::time_t tstam = -1;
    std::tm Date_Strct = {};
    for (int j = 0; j < time.size(); j++)
    {
        if (time[j] == '[' || time[j] == '/' || time[j] == ':')
        {
            time[j] = ' ';
        }
    }

    std::istringstream divide_time(time);
    std::string tmp_time;
    std::vector<std::string> dates;

    while(divide_time >> tmp_time)
    {
        dates.push_back(tmp_time);
    }

    Date_Strct.tm_sec = ((stoi(dates[5])));       //Sec

    Date_Strct.tm_min = ((stoi(dates[4])));       //Minute

    Date_Strct.tm_hour = ((stoi(dates[3])-1));    //Hours

    Date_Strct.tm_mday = ((stoi(dates[0])));      //Day

    Date_Strct.tm_mon = ((MonthToVal(dates[1]))); //Month

    Date_Strct.tm_year = ((stoi(dates[2]))-1900); //Year

    //приводим к виду timestamp
    tstam = std::mktime(&Date_Strct);
    if (tstam >= 0)
    {
        return tstam;
    }
    else
    {
        std::cout << "Error in time convertation!";
        return 1;
    }
}

std::pair<std::time_t, std::time_t> MaxWindow_Def(const std::vector<std::time_t>& all_timestmp, std::time_t time) 
{    
    std::pair<std::time_t, std::time_t> WindowMX = {all_timestmp[0], all_timestmp[0] + time};
    long int lt = 0, rt = 0;
    int cntMax = 0;
    

    while (rt < all_timestmp.size()) 
    {
        while (rt < all_timestmp.size() && all_timestmp[rt] - all_timestmp[lt] < time)
        {
            ++rt;
        }

        int tmp_cnt = rt - lt;

        if (tmp_cnt > cntMax) {
            cntMax = tmp_cnt;
            WindowMX = {all_timestmp[lt], all_timestmp[lt] + time};
        }
        ++lt;
    }

    return WindowMX;
}

bool Error5XX(std::string& line_req)
{
    int i = line_req.size()-1;

    while(line_req[i] != ' ' and i > 0)
    {
        i--;
    }
    if(i > 3)
    {
        i-=3;
        if(line_req[i] == '5')
        {
            return true;
        }
    }
    return false;
}

int AnalizerDefLogs(const std::string& o_pth, const std::string& i_pht,const bool& out_act,const bool& prnt_act, const bool& sts_act, int n,const bool& wndw_act, int t, int time_F, int time_E)
{
    std::fstream input_file(i_pht);
    std::ofstream output_file(o_pth);

    
    if (!input_file)// Проверяем что полученный аргумент является файлом
    {
        std::cout << "Error! File is not open! Please check your input";
        return 1;
    }

    std::string line_tmp;
    std::string tmp_str;
    std::map<std::string, int> status_top;
    std::vector<time_t> all_timestmp;
    int cnt = 0;

    while(std::getline(input_file, line_tmp))//Получаем строку
    {
        std::istringstream divide_log(line_tmp);
        std::vector<std::string> log_args;

        while(divide_log >> tmp_str)
        {
            log_args.push_back(tmp_str);
        }
        if (log_args.size() > 5)
        {
            time_t time_of_log = Time_in_Sec(log_args[3]);
            if (time_of_log <= time_E and time_of_log >= time_F)
            {    
                if (Error5XX(line_tmp))
                {
                    Print_Output(prnt_act, out_act, line_tmp, output_file);
                    if(sts_act)
                    {
                        status_top[log_args[5] + " " + log_args[6] + " " + log_args[7]]++;
                    }
                    if (wndw_act)
                    {
                        all_timestmp.push_back(time_of_log);
                    }

                }
            }
        }
    }
    if(sts_act)
    {
        std::string header = "\n --==Top Requests==--", top_pair = "";
        Print_Output(prnt_act, out_act, header, output_file);

        //Состовляем топ запросов, сортировкой полученных пар из map
        std::vector<std::pair<std::string, int>> generate_top(status_top.begin(), status_top.end());

        std::sort(generate_top.begin(), generate_top.end(), [](const auto &argument_fst, const auto &argument_snd) 
        {
            return argument_fst.second > argument_snd.second; //Условие при сортировке, ищем наибольшую пару
        }
        );
        if (n > generate_top.size()) 
        {
            n = generate_top.size();
        }
        for (int i = 0; i < n; i++) {
            top_pair = std::to_string(i+1) + ". " + generate_top[i].first + " ||| times: " + std::to_string(generate_top[i].second);
            Print_Output(prnt_act, out_act, top_pair, output_file);
        }
    }
    if (wndw_act)
    {
        std::pair wind_inter = MaxWindow_Def(all_timestmp, t);
        std::string result = " Begin: " + std::to_string(wind_inter.first) + " End: " + std::to_string(wind_inter.second), header = "\n --==The Biggest Window==--";
        Print_Output(prnt_act, out_act, header, output_file);
        Print_Output(prnt_act, out_act, result, output_file);
    }
    output_file.close();
    return 0;
}

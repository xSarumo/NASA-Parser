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
#include "../lib/filereader.cpp"

int main(int argc, char* argv[])
{   
    if (argc < 2)
    {
        std::cout << "Error! Press your log file";// вывод ошибки 
        return 1;
    }
    
    char commands[][12] = 
    {
        "-o", "--output", "-p", "--print",
        "-s", "--stats", "-w", "--window", 
        "-f", "--from", "-e", "--to"
    };

    std::string
                full_command_line = "",
                time_e = "End",
                time_f = "Start",
                i_pth, o_pth, value = "";

    bool prnt_act = false,
         wndw_act = false, 
         sts_act = false,
         out_act = false,
         value_beg = false;

    int i, n = 10, t = 0;
    int time_E = INT_MAX, time_F = INT_MIN;

    //Приведение аргументов к одному виду
    for (i = 1; i < argc; ++i)
    {
        full_command_line += argv[i];
        full_command_line += ' ';
    }
    full_command_line += " .";

    std::istringstream divide_string(full_command_line);
    std::string word_of_line, tmp_str;
    std::vector <std::string> full_args;

    while(divide_string >> word_of_line)
    {
        full_args.push_back(word_of_line);
    }

    //Считываем аргументы
    for (i = 0; i < full_args.size(); i++)
    {   
        if (full_args[i] == commands[0] || full_args[i] == commands[1]) // -o || --output
        {
            if (i + 1 < full_args.size())
            {
                o_pth = full_args[i+1];
                if(i+1 == full_args.size()-1)
                {
                    break;
                }
                else
                {
                    ++i;
                }
                out_act = true;
            }
        }
        else if((full_args[i].compare(0, 9, "--output=") == 0) || (full_args[i].compare(0, 3, "-o=") == 0)) // -o= || --output=
        {
            value = "";
            value_beg = false;
            for (int j = 0; j < full_args[i].size(); j++)
            {
                if(value_beg)
                {
                    value += full_args[i][j];
                }
                if(full_args[i][j] == '=')
                {
                    value_beg = true;
                }

            }
            o_pth = value;
            out_act = true;
        }
        else if (full_args[i] == commands[2] || full_args[i] == commands[3]) // -p || --print
        {
            prnt_act = true;
        }
        else if (full_args[i] == commands[4] || full_args[i] == commands[5]) // -s || --stats
        {
            if (i + 1 < full_args.size())
            {
                if (full_args[i+1][0] != '-')
                {
                    n = stoi(full_args[i+1]);
                    i++;
                    sts_act = true;
                }
            }
        }
        else if ((full_args[i].compare(0, 9, "--stats=") == 0) || (full_args[i].compare(0, 3, "-s=") == 0)) // -s= || --status=
        {
            value = "";
            value_beg = false;
            for (int j = 0; j < full_args[i].size(); j++)
            {
                if(value_beg)
                {
                    value += full_args[i][j];
                }
                if(full_args[i][j] == '=')
                {
                    value_beg = true;
                }

            }
            n = stoi(value);
            sts_act = true;
        }
        else if (full_args[i] == commands[6] || full_args[i] == commands[7]) // -w || --window
        {
            if (i + 1 < full_args.size())
            {
                if (full_args[i+1][0] != '-')
                {
                    t = stoi(full_args[i+1]);
                    wndw_act = true;
                    if(i+1 == full_args.size()-1)
                    {
                        break;
                    }
                    else
                    {
                        ++i;
                    }
                }
            }
        }
        else if ((full_args[i].compare(0, 9, "--window=") == 0) || (full_args[i].compare(0, 3, "-w=") == 0)) // -w= || --window=
        {
            value = "";
            value_beg = false;
            for (int j = 0; j < full_args[i].size(); j++)
            {
                if(value_beg)
                {
                    value += full_args[i][j];
                }
                if(full_args[i][j] == '=')
                {
                    value_beg = true;
                }
            }
            t = stoi(value);
            wndw_act = true;
        
        }
        else if (full_args[i] == commands[8] || full_args[i] == commands[9]) // -f || --from
        {
            if (i + 1 < full_args.size())
            {
                if (full_args[i+1][0] != '-')
                {
                    time_F = stoi(full_args[i+1]);
                    if(i+1 == full_args.size()-1)
                    {
                        break;
                    }
                    else
                    {
                        ++i;
                    }
                    time_f = full_args[i];
                }
            }
            
        }
        else if ((full_args[i].compare(0, 7, "--from=") == 0) || (full_args[i].compare(0, 3, "-f=") == 0))
        {
            value = "";
            value_beg = false;
            for (int j = 0; j < full_args[i].size(); j++)
            {
                if(value_beg)
                {
                    value += full_args[i][j];
                }
                if(full_args[i][j] == '=')
                {
                    value_beg = true;
                }

            }
            time_F = stoi(value);
            time_f = value;
        }
        else if (full_args[i] == commands[10] || full_args[i] == commands[11]) // -e || --to
        {
            if (i + 1 < full_args.size())
            {
                if (full_args[i+1][0] != '-')
                {
                    time_E = stoi(full_args[i+1]);
                    if(i+1 == full_args.size()-1)
                    {
                        break;
                    }
                    else
                    {
                        ++i;
                    }
                    time_e = full_args[i];
                }
            }
            
        }
        else if((full_args[i].compare(0, 5, "--to=") == 0) || (full_args[i].compare(0, 3, "-e=") == 0))
        {
            value = "";
            value_beg = false;
            for (int j = 0; j < full_args[i].size(); j++)
            {
                if(value_beg)
                {
                    value += full_args[i][j];
                }
                if(full_args[i][j] == '=')
                {
                    value_beg = true;
                }

            }
            time_E = stoi(value);
            time_e = value;
        }
        else
        {
            if(i_pth == "")
            {i_pth = full_args[i];}
        }
    }
    //Вывод для пользователья
        std::cout << "--==Your input parametrs==--:\n";
        std::cout << "Input Path: /" << i_pth << "\n";
        std::cout << "Output Path: /" << o_pth << "\n"; 
        std::cout << "Stats: "<< n << "\n";
        std::cout << "Window: "<< t << "\n";
        std::cout << "Analize from: " << time_f << " sec" << "\n";
        std::cout << "Analize to: " << time_e << " sec" << "\n" << "\n";
    AnalizerDefLogs(o_pth, i_pth, out_act, prnt_act, sts_act, n, wndw_act, t, time_F, time_E);
    return 0;
}
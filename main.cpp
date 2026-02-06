#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <atomic>
#include <fstream>
#include <cstdlib>
#include <filesystem>
#include <print>


bool download_price(const std::string& coin_id,
                    const std::string& outfile = "temp.json")
{
    std::string url = "https://api.coingecko.com/api/v3/simple/price"
                      "?ids=" + coin_id +
                      "&vs_currencies=usd";

    std::string cmd = "curl -s -o \"" + outfile + "\" \"" + url + "\"";
    int ret = std::system(cmd.c_str());

    if(ret != 0)
    {
        std::cerr << "Failed curl " << ret << '\n' ;
        return false;
    }

    if(std::filesystem::exists(outfile) &&
       std::filesystem::file_size(outfile) > 10)
    {
        std::cout << "Success: " << outfile << " created. "
            << std::filesystem::file_size(outfile) << " [bytes] \n\n";
        return true;
    }

    std::cerr << "File " << outfile << " does not exists" << '\n' ;
    return false;
}

double parse_bitcoin_usd_price(const std::string& filename = "temp.json")
{
    std::ifstream file(filename);
    if(!file.is_open())
    {
        std::cerr << "Cannot open "<< filename << '\n';
    }
    std::string json((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
    file.close();
    size_t pos = json.find("\"usd\":");
    if(pos == std::string::npos)
    {
        std::cerr << "Could not find \"usd\": in JASON\n";
        return -1.0;
    }

    const std::string key = "\"usd\":";
    pos += key.size();

    while(pos < json.size() && std::isspace(json[pos])) ++pos;
    size_t end = pos;
    while(end < json.size() && (std::isdigit(json[end]) || json[end] == '.' )) ++end;

    if(end == pos)
    {
        std::cerr << "No number founded after \"usd\":\n";
        return -1.0;
    }

    std::string price_str = json.substr(pos, end - pos);
    try
    {
        double price = std::stod(price_str);
        return price;
    }catch(...)
    {
        std::cerr << "Failed to convert " << price_str << " to double" << '\n';
    }
    return 0;
}

int main()
{
    std::cout << "concurrent fetcher and arbitrage checker" << '\n';


    std::stop_source stop_source;
    std::stop_token stop_token = stop_source.get_token();


    std::jthread fetch_thread([&](std::stop_token st)
            {
                std::cout << "[Fetcher] Thread started successfully!\n";
                constexpr auto INTERVAL = std::chrono::seconds(5);

                int cycle{};

                while(!st.stop_requested())
                {
                    cycle++;

                    if(download_price("bitcoin"))
                    {
                        double price = parse_bitcoin_usd_price();

                        if(price > 0.0)
                        {
                            std::cout << "BTC/UCS : " << price << '\n';
                        }else
                        {
                            std::cout << "Parse failed." << '\n';
                        }
                    }else
                    {
                        std::cout << "Download failed." << '\n';
                    }

                    std::cout << "------------------------------" << '\n';

                    for(int i = 0 ; i < 5 && !st.stop_requested(); ++i)
                    {
                        std::this_thread::sleep_for(std::chrono::seconds(2));
                    }


                    std::cout << "[fetcher thread]  stop requested." << '\n';
                }
            }, stop_token);


    for (int i = 0; i < 8; ++i) {
        std::cout << "[Main] Still alive... (" << (i+1) << "/8)\n";
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }


    stop_source.request_stop();

    std::cout << "Fetcher thread has finished.\n";


    return 0;

}




















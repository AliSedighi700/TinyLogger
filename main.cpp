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
    std::cout << "Fetching " << coin_id << "...\n";
    std::cout << "===============================" << '\n';
    std::cout << "Command " <<  cmd << '\n';

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

    double price = parse_bitcoin_usd_price();

    if(price > 0.0)
    {
        std::cout << "market data is downloaded" << '\n';
        std::cout << "Current bitcon price: $" << price << '\n';
    }else{
        std::cout << "Parsing failed" << '\n';
    }



    return 0;

}

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
        std::cout << "Success " << outfile << " created (" << std::filesystem::file_size(outfile) << ")[bytes] \n\n";
        return true;
    }

    std::cerr << "File " << outfile << " does not exists" << '\n' ;
    return false;
}

int main()
{
    std::cout << "concurrent fetcher and arbitrage checker" << '\n';

    bool ok = download_price("bitcoin");

    if(ok)
    {
        std::cout << "market data is downloaded" << '\n';
    }else{
        std::cout << "download failed" << '\n';
    }



    return 0;

}

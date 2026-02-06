#include "price_fetcher.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

namespace fs = std::filesystem;

PriceFetcher::PriceFetcher(SharedPrices& store, std::string coin_id)
    : priceStore_(store)
    , coin_id_(std::move(coin_id))
{
}

void PriceFetcher::start(std::stop_token st)
{
    thread_ = std::jthread([this, st](std::stop_token token) {
        std::cout << "[Fetcher " << coin_id_ << "] Thread started\n";

        int cycle = 0;
        while (!token.stop_requested()) {
            ++cycle;
            std::cout << "[Fetcher " << coin_id_ << "] Cycle " << cycle << "   ";

            if (downloadPrice()) {
                double price = parsePrice();
                if (price > 0.0) {
                    priceStore_.updatePrice(coin_id_, price);
                    std::cout << coin_id_ << "/USD: $" << price << "  → updated shared store\n";
                } else {
                    std::cout << "Parse failed\n";
                }
            } else {
                std::cout << "Download failed\n";
            }

            std::cout << "----------------------------------------\n";

            for (int i = 0; i < 5 && !token.stop_requested(); ++i) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }

        std::cout << "[Fetcher " << coin_id_ << "] Stop requested → exiting\n";
    }, st);
}

bool PriceFetcher::downloadPrice(const std::string& outfile) const
{
    std::string url = "https://api.coingecko.com/api/v3/simple/price"
                      "?ids=" + coin_id_ +
                      "&vs_currencies=usd";

    std::string cmd = "curl -s -o \"" + outfile + "\" \"" + url + "\"";

    int ret = std::system(cmd.c_str());
    if (ret != 0) {
        std::cerr << "[Fetcher " << coin_id_ << "] curl failed (code " << ret << ")\n";
        return false;
    }

    if (fs::exists(outfile) && fs::file_size(outfile) > 10) {
        return true;
    }

    std::cerr << "[Fetcher " << coin_id_ << "] File missing or empty.\n";
    return false;
}

double PriceFetcher::parsePrice(const std::string& filename) const
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[Parser " << coin_id_ << "] Cannot open " << filename << "\n";
        return -1.0;
    }

    std::string json((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());

    file.close();

    size_t pos = json.find("\"usd\":");
    if (pos == std::string::npos) {
        std::cerr << "[Parser " << coin_id_ << "] Could not find \"usd\": in JSON\n";
        return -1.0;
    }

    pos += 7;
    while (pos < json.size() && std::isspace(json[pos])) ++pos;

    size_t end = pos;
    while (end < json.size() && (std::isdigit(json[end]) || json[end] == '.')) ++end;

    if (end == pos) {
        std::cerr << "[Parser " << coin_id_ << "] No number found after \"usd\":\n";
        return -1.0;
    }

    std::string price_str = json.substr(pos, end - pos);

    try {
        return std::stod(price_str);
    } catch (...) {
        std::cerr << "[Parser " << coin_id_ << "] Failed to convert \"" << price_str << "\" to double\n";
        return -1.0;
    }
}

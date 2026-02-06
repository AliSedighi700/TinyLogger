#include "shared_prices.h"
#include "price_fetcher.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <stop_token>

int main()
{
    std::cout << std::unitbuf;

    SharedPrices price_store;

    PriceFetcher fetcher(price_store, "bitcoin");

    std::stop_source stop_source;
    std::stop_token stop_token = stop_source.get_token();

    std::cout << "[Main] Starting Bitcoin price fetcher...\n";
    fetcher.start(stop_token);

    std::cout << "[Main] Monitoring shared prices (every 2 seconds)...\n";

    for (int i = 0; i < 12; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(2));

        double btc_price = price_store.getPrice("bitcoin");

        if (btc_price > 0.0) {
            std::cout << "[Main] Latest BTC/USD: $" << btc_price
                      << "   (snapshot at second " << (i * 2) << ")\n";
        } else {
            std::cout << "[Main] No BTC price available yet...\n";
        }
    }
    std::cout << "[Main] Requesting fetcher to stop...\n";
    stop_source.request_stop();

    std::cout << "[Main] Shutdown complete.\n\n";

    return 0;
}


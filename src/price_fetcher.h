#ifndef PRICE_FETCHER_H
#define PRICE_FETCHER_H

#include "shared_prices.h" 
#include <thread>
#include <string_view>
#include <stop_token>



class PriceFetcher
{
private: 
    SharedPrices& priceStore_; 
    std::string coin_id_{}; 
    std::jthread thread_; 

    bool downloadPrice(const std::string& outfile = "temp.json" ) const;
    double parsePrice(const std::string& filename = "temp.json") const;


public: 

    PriceFetcher(SharedPrices& store, std::string coin_id);

    void start(std::stop_token st);

};


#endif 

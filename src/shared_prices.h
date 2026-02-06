#ifndef SHARED_PRICE_H
#define SHARED_PRICE_H

#include <map>
#include <mutex>
#include <string>
#include <string_view>

class SharedPrices
{
private:
    mutable std::mutex mtx_{}; 
    std::map<std::string, double> prices_{}; 


public: 

    SharedPrices() = default; 

    void updatePrice(std::string_view coin, double price);

    double getPrice(std::string_view coin ) const; 

};



#endif

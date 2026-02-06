#include "shared_prices.h" 


void SharedPrices::updatePrice(std::string_view coin, double price)
{

    std::lock_guard lock(mtx_);
    prices_[std::string{coin} ]= price;

}


double SharedPrices::getPrice(std::string_view coin) const
{
    std::lock_guard lock(mtx_);
    auto it = prices_.find(std::string{coin}); 
    return (it != prices_.end() ) ? it->second : -1.0;
}

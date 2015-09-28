#ifndef STOCK_H
#define STOCK_H

#include <iostream>
#include <vector>
#include <queue>

struct order{
    int timestamp, timestamp2, clientID, equityID, priceLimit, quantity, index_num, num_bought, num_sold, profit;
    std::string intent;
};

class Sellers {
public:
    bool operator()(order bidder, order bidder2){
            if (bidder.priceLimit > bidder2.priceLimit){
                return true;
            }
            if (bidder.priceLimit == bidder2.priceLimit && bidder.index_num > bidder2.index_num){
                return true;
            }
        return false;
    }
};

class Buyers {
public:
    bool operator()(order bidder, order bidder2){
            if (bidder.priceLimit < bidder2.priceLimit){
                return true;
            }
            if (bidder.priceLimit == bidder2.priceLimit && bidder.index_num < bidder2.index_num){
                return true;
            }
        return false;
    }
};

struct equity{
    int equityID, medprice, counter, timestamp, index_num;
    bool time_change;
};

struct equities{
    order client;
    std::priority_queue<order, std::vector<order>, Buyers> buyers;
    std::priority_queue<order, std::vector<order>, Sellers> sellers;
};

void verbose(order buyer, order seller, int difference, int price);

void median(int medprice, int timestamp, int equityID);

void client_info(std::vector<order> market);

void time_travelers(std::vector<equities> equities, int num_equities);

int medianprice(std::vector<order> prices, int equity_ID, int counter);

void insertionSort_median(std::vector<order>& clients);

void insertionSort_profit(std::vector<order>& clients);

void insertionSort_equities(std::vector<equity>& equities);

#endif //STOCK_H


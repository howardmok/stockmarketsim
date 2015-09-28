#include "stock.h"
using namespace std;

void verbose(order buyer, order seller, int difference, int price){
    cout << "Client " << buyer.clientID << " purchased " << difference << " shares of Equity " << buyer.equityID << " from Client " << seller.clientID << " for $" << price << "/share\n";
}

void median(int medprice, int timestamp, int equityID){
    cout << "Median match price of Equity " << equityID << " at time " << timestamp << " is $" << medprice << '\n';
}

void client_info(vector<order> market){
    cout << "---Client Info---\n";
    for (unsigned int i = 0; i < market.size(); i++){
        cout << "Client " << i << " bought " << market[i].num_bought << " and sold " << market[i].num_sold << " for a net transfer of $" << market[i].profit << '\n';
    }
}

void time_travelers(vector<equities> equities, int num_equities){
    cout << "---Time Travelers---\n";
    order client;
    vector<order> profits;
    for (int m = 0; m < num_equities; m++){ // go through all the equities
        int timestamp1 = 0, timestamp2 = 0;
        struct equities temp = equities[m];
        if (equities[m].sellers.empty()){ // no valid sellers
            if (timestamp1 != -1 && timestamp2 != -1){
                timestamp1 = -1;
                timestamp2 = -1;
                cout << "A time traveler would buy shares of Equity " << m << " at time: " << timestamp1 << " and sell these shares at time: " << timestamp2 << '\n';
            }
        }
        if (equities[m].buyers.empty()){ // no valid buyers
            if (timestamp1 != -1 && timestamp2 != -1){
                timestamp1 = -1;
                timestamp2 = -1;
                cout << "A time traveler would buy shares of Equity " << m << " at time: " << timestamp1 << " and sell these shares at time: " << timestamp2 << '\n';
            }
        }
        for (unsigned int n = 0; n < equities[m].sellers.size(); n++){
            for (unsigned int j = 0; j < equities[m].buyers.size(); j++){
                if (!temp.buyers.empty()){
                    if (temp.sellers.top().index_num < temp.buyers.top().index_num && temp.sellers.top().priceLimit < temp.buyers.top().priceLimit){
                        client.profit = temp.buyers.top().priceLimit - temp.sellers.top().priceLimit;
                        client.timestamp = temp.sellers.top().timestamp;
                        client.timestamp2 = temp.buyers.top().timestamp;
                        if (client.profit > 0){
                            profits.push_back(client);
                        }
                    }
                    temp.buyers.pop();
                }
            }
            temp.buyers = equities[m].buyers;
            temp.sellers.pop();
        }
        if (profits.empty()){
            if (timestamp1 != -1 && timestamp2 != -2){
                timestamp1 = -1;
                timestamp2 = -1;
                cout << "A time traveler would buy shares of Equity " << m << " at time: " << timestamp1 << " and sell these shares at time: " << timestamp2 << '\n';
            }
        }
        if (timestamp1 != -1 && timestamp2 != -1){
            insertionSort_profit(profits);
            timestamp1 = profits[0].timestamp;
            timestamp2 = profits[0].timestamp2;
            cout << "A time traveler would buy shares of Equity " << m << " at time: " << timestamp1 << " and sell these shares at time: " << timestamp2 << '\n';
        }
        while (!profits.empty()){
            profits.pop_back();
        }
    }
}

int medianprice(vector<order> prices, int equity_ID, int counter){
    vector<order> medians;
    int medprice;
    for (unsigned int i = 0; i < prices.size(); i++){
        if (prices[i].equityID == equity_ID){
            medians.push_back(prices[i]);
            counter++;
        }
    }
    insertionSort_median(medians);
    if (counter % 2 == 1){
        medprice = medians[(medians.size()-1)/2].priceLimit;
    }
    else{
        if (prices.size() == 1){
            medprice = medians[0].priceLimit;
        }
        else{
            medprice = ((medians[(medians.size()/2) - 1].priceLimit) + (medians[(medians.size()/2)].priceLimit))/2;
        }
    }
    return medprice;
}

void insertionSort_median(vector<order>& clients){
    order x;
    for (unsigned int i = 1; i < clients.size(); i++){
        for (int j = i; j > 0; j--){
            x = clients[j];
            if (clients[j-1].priceLimit > x.priceLimit){
                clients[j] = clients[j-1];
                clients[j-1] = x;
            }
        }
    }
}

void insertionSort_profit(vector<order>& clients){
    order x;
    for (unsigned int i = 1; i < clients.size(); i++){
        for (int j = i; j > 0; j--){
            x = clients[j];
            if (clients[j-1].profit < x.profit){
                clients[j] = clients[j-1];
                clients[j-1] = x;
            }
        }
    }
}

void insertionSort_equities(vector<equity>& equities){
    equity x;
    for (unsigned int i = 1; i < equities.size(); i++){
        for (int j = i; j > 0; j--){
            x = equities[j];
            if (equities[j-1].equityID > x.equityID){
                equities[j] = equities[j-1];
                equities[j-1] = x;
            }
        }
    }
}

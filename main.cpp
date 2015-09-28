#include <algorithm>
#include "stock.h"
#include <getopt.h>
#include "P2.h"
using namespace std;

int main(int argc, char** argv) {
    freopen("PR-ex.txt", "r", stdin); //COMMENT THIS OUT BEFORE YOU SUBMIT ON CAEN
    
    vector<equities> eq, eq2;
    vector<int> medians;
    vector<order> prices;
    vector<equity> equities, equities2;
    priority_queue<order, vector<order>, Buyers> buyers2;
    priority_queue<order, vector<order>, Sellers> sellers2;
    stringstream ss;
    struct equities ab;
    
    bool vflag = false;
    bool mflag = false;
    bool cflag = false;
    bool tflag = false;
    bool pr_mode = false;
    
    struct option longOpts[] = {
        { "verbose", no_argument, NULL, 'v' },
        { "median", no_argument, NULL, 'm' },
        { "client_info", no_argument, NULL, 'c' },
        { "time_travelers", no_argument, NULL, 't' },
    };
    opterr = false;
    
    int opt = 0, index = 0;
    while ((opt = getopt_long(argc, argv, "vmct", longOpts, &index)) != -1) {
        switch (opt) {
            case 'v':
                vflag = true;
                break;
            case 'm':
                mflag = true;
                break;
            case 'c':
                cflag = true;
                break;
            case 't':
                tflag = true;
                break;
            default:
                return 0;
        }
    }
    
    int num_client = 0, num_equity = 0;
    string random, input;
    //First three lines
    cin >> random >> input;
    cin >> random >> num_client;
    cin >> random >> num_equity;
    
    cout << "Processing orders...\n";
    
    vector<order> orders (num_client);
    int timestamp = 0, client_ID, equity_num, priceL, quantity, difference, previous;
    int num_processed = 0;
    int medprice = 0;
    int current_timestamp = 0, previous_timestamp = 0;
    int index_num = 0;
    int counter = 0, counter2 = 0;
    int transactions = 0;
    int quant = 0;
    char c;
    string buy_sell;
    equity medianorder;
    order client, temp, temp2;
    //price temp;
    for (int i = 0; i < num_equity; ++i){
        ab = {client, buyers2, sellers2};
        eq.push_back(ab);
        eq2.push_back(ab);
    }
    //Trade list
    if (input == "TL"){
        while (cin >> timestamp >> c >> client_ID >> buy_sell >> c >> equity_num >> c >> priceL >> c >> quantity){
            if (timestamp < 0){
                cerr << "Timestamp is negative integer\n";
                exit(1);
            }
            if (client_ID < 0 || client_ID > num_client - 1){
                cerr << "Client ID invalid\n";
                exit(1);
            }
            if (equity_num < 0 || equity_num > num_equity - 1){
                cerr << "Equity ID invalid\n";
                exit(1);
            }
            if (priceL < 1){
                cerr << "Price is not a positive integer\n";
                exit(1);
            }
            if (quantity < 1){
                cerr << "Quantity is not a positive integer\n";
                exit(1);
            }
            if (previous_timestamp > timestamp){
                cerr << "Timestamps not strictly increasing\n";
                exit(1);
            }
            else if (previous_timestamp <= timestamp){
                previous_timestamp = timestamp;
            }
            client.timestamp = timestamp;
            client.clientID = client_ID;
            client.intent = buy_sell;
            client.equityID = equity_num;
            client.priceLimit = priceL;
            client.quantity = quantity;
            client.index_num = index_num;
            index_num++;
            if (client.timestamp != current_timestamp){
                if (mflag && counter > 0){
                    for (int x = 0; x < transactions; x++){
                        medprice = medianprice(prices, equities2[x].equityID, 0);
                        median(medprice, current_timestamp, equities2[x].equityID);
                        medianorder.medprice = 0;
                    }
                }
                current_timestamp = client.timestamp;
            }
            if (client.intent == "BUY"){ //if client is buyer
                int trans = 0;
                quant = 0;
                counter2 = 0;
                if (!eq[client.equityID].sellers.empty()){
                    difference = 0;
                    if (client.priceLimit >= eq[client.equityID].sellers.top().priceLimit){//if you can buy from seller on market
                        trans++;
                        if (client.quantity > eq[client.equityID].sellers.top().quantity){//you want to buy more shares than guy can sell
                            client.quantity = client.quantity - eq[client.equityID].sellers.top().quantity;
                            quant = client.quantity - eq[client.equityID].sellers.top().quantity;
                            difference = eq[client.equityID].sellers.top().quantity;
                            client.num_bought = difference;
                            orders[client_ID].profit -= (eq[client.equityID].sellers.top().priceLimit * client.num_bought);
                            orders[client_ID].num_bought += client.num_bought;
                            orders[eq[client.equityID].sellers.top().clientID].profit += (eq[client.equityID].sellers.top().priceLimit * client.num_bought);
                            orders[eq[client.equityID].sellers.top().clientID].num_sold += client.num_bought;
                            temp2.clientID = eq[client.equityID].sellers.top().clientID;
                            temp2.priceLimit = eq[client.equityID].sellers.top().priceLimit;
                            temp2.equityID = eq[client.equityID].sellers.top().equityID;
                            temp2.timestamp = eq[client.equityID].sellers.top().timestamp;
                            eq[client.equityID].sellers.pop();
                        }
                        else if (client.quantity == eq[client.equityID].sellers.top().quantity){//equal shares
                            difference = client.quantity;
                            client.num_bought = difference;
                            quant = 0;
                            client.quantity = 0;
                            orders[client_ID].profit -= (eq[client.equityID].sellers.top().priceLimit * client.num_bought);
                            orders[client_ID].num_bought += client.num_bought;
                            orders[eq[client.equityID].sellers.top().clientID].profit += (eq[client.equityID].sellers.top().priceLimit * client.num_bought);
                            orders[eq[client.equityID].sellers.top().clientID].num_sold += client.num_bought;
                            temp2.clientID = eq[client.equityID].sellers.top().clientID;
                            temp2.priceLimit = eq[client.equityID].sellers.top().priceLimit;
                            temp2.equityID = eq[client.equityID].sellers.top().equityID;
                            temp2.timestamp = eq[client.equityID].sellers.top().timestamp;
                            eq[client.equityID].sellers.pop();
                        }
                        else{//they have more shares than you want to buy
                            difference = client.quantity;
                            client.num_bought = difference;
                            quant = 0;
                            client.quantity = 0;
                            orders[client_ID].profit -= (eq[client.equityID].sellers.top().priceLimit * client.num_bought);
                            orders[client_ID].num_bought += client.num_bought;
                            orders[eq[client.equityID].sellers.top().clientID].profit += (eq[client.equityID].sellers.top().priceLimit * client.num_bought);
                            orders[eq[client.equityID].sellers.top().clientID].num_sold += client.num_bought;
                            temp2.clientID = eq[client.equityID].sellers.top().clientID;
                            temp2.priceLimit = eq[client.equityID].sellers.top().priceLimit;
                            temp2.equityID = eq[client.equityID].sellers.top().equityID;
                            temp2.timestamp = eq[client.equityID].sellers.top().timestamp;
                            order client2 = eq[client.equityID].sellers.top();
                            client2.quantity = eq[client.equityID].sellers.top().quantity - difference;
                            eq[client.equityID].sellers.pop();
                            eq[client.equityID].sellers.push(client2);
                        }
                        num_processed++;
                        medianorder.medprice = temp2.priceLimit;
                        medianorder.equityID = temp2.equityID;
                        prices.push_back(temp2); // add prices to list to find middle 2 for even # of prices
                        if (prices.size() == 1){
                            transactions++;
                            equities2.push_back(medianorder);
                        }
                        for (unsigned int l = 0; l < prices.size()-1; ++l){
                            if (temp2.equityID == prices[l].equityID){
                                counter2++;
                            }
                        }
                        if (counter2 == 0 && prices.size() != 1){
                            transactions++;
                            equities2.push_back(medianorder);
                            insertionSort_equities(equities2);
                        }
                        counter++;
                        previous = client.timestamp;
                        if (medianorder.timestamp != previous && medianorder.time_change){ // if timestamp changed
                            medianorder.timestamp = 0;
                            medianorder.time_change = true;
                        }
                        else{
                            medianorder.timestamp = client.timestamp;
                        }
                        if (vflag){
                            verbose(client, temp2, difference, temp2.priceLimit);
                        }
                        medianorder.index_num = client.index_num;
                        if (!eq[client.equityID].sellers.empty()){
                            while(client.quantity != 0 && eq[client.equityID].sellers.top().priceLimit >= client.priceLimit){
                                difference = 0;
                                if (client.quantity > eq[client.equityID].sellers.top().quantity){//you want to buy more shares than guy can sell
                                    client.quantity = client.quantity - eq[client.equityID].sellers.top().quantity;
                                    quant = client.quantity - eq[client.equityID].sellers.top().quantity;
                                    difference = eq[client.equityID].sellers.top().quantity;
                                    client.num_bought = difference;
                                    orders[client_ID].profit -= (eq[client.equityID].sellers.top().priceLimit * client.num_bought);
                                    orders[client_ID].num_bought += client.num_bought;
                                    orders[eq[client.equityID].sellers.top().clientID].profit += (eq[client.equityID].sellers.top().priceLimit * client.num_bought);
                                    orders[eq[client.equityID].sellers.top().clientID].num_sold += client.num_bought;
                                    temp2.clientID = eq[client.equityID].sellers.top().clientID;
                                    temp2.priceLimit = eq[client.equityID].sellers.top().priceLimit;
                                    temp2.equityID = eq[client.equityID].sellers.top().equityID;
                                    temp2.timestamp = eq[client.equityID].sellers.top().timestamp;
                                    eq[client.equityID].sellers.pop();
                                }
                                else if (client.quantity == eq[client.equityID].sellers.top().quantity){//equal shares
                                    difference = client.quantity;
                                    client.num_bought = difference;
                                    quant = 0;
                                    client.quantity = 0;
                                    orders[client_ID].profit -= (eq[client.equityID].sellers.top().priceLimit * client.num_bought);
                                    orders[client_ID].num_bought += client.num_bought;
                                    orders[eq[client.equityID].sellers.top().clientID].profit += (eq[client.equityID].sellers.top().priceLimit * client.num_bought);
                                    orders[eq[client.equityID].sellers.top().clientID].num_sold += client.num_bought;
                                    temp2.clientID = eq[client.equityID].sellers.top().clientID;
                                    temp2.priceLimit = eq[client.equityID].sellers.top().priceLimit;
                                    temp2.equityID = eq[client.equityID].sellers.top().equityID;
                                    temp2.timestamp = eq[client.equityID].sellers.top().timestamp;
                                    eq[client.equityID].sellers.pop();
                                }
                                else{//they have more shares than you want to buy
                                    difference = client.quantity;
                                    client.num_bought = difference;
                                    quant = 0;
                                    client.quantity = 0;
                                    orders[client_ID].profit -= (eq[client.equityID].sellers.top().priceLimit * client.num_bought);
                                    orders[client_ID].num_bought += client.num_bought;
                                    orders[eq[client.equityID].sellers.top().clientID].profit += (eq[client.equityID].sellers.top().priceLimit * client.num_bought);
                                    orders[eq[client.equityID].sellers.top().clientID].num_sold += client.num_bought;
                                    temp2.clientID = eq[client.equityID].sellers.top().clientID;
                                    temp2.priceLimit = eq[client.equityID].sellers.top().priceLimit;
                                    temp2.equityID = eq[client.equityID].sellers.top().equityID;
                                    temp2.timestamp = eq[client.equityID].sellers.top().timestamp;
                                    order client2 = eq[client.equityID].sellers.top();
                                    client2.quantity = eq[client.equityID].sellers.top().quantity - difference;
                                    eq[client.equityID].sellers.pop();
                                    eq[client.equityID].sellers.push(client2);
                                }
                                num_processed++;
                                medianorder.medprice = temp2.priceLimit;
                                medianorder.equityID = temp2.equityID;
                                prices.push_back(temp2);
                                if (prices.size() == 1){
                                    transactions++;
                                    equities2.push_back(medianorder);
                                }
                                for (unsigned int l = 0; l < prices.size()-1; ++l){
                                    if (temp2.equityID == prices[l].equityID){
                                    counter2++;
                                    }
                                }
                                if (counter2 == 0 && prices.size() != 1){
                                    transactions++;
                                    equities2.push_back(medianorder);
                                    insertionSort_equities(equities2);
                                }
                                counter++;
                                previous = client.timestamp;
                                if (medianorder.timestamp != previous && medianorder.time_change){
                                    medianorder.timestamp = 0;
                                    medianorder.time_change = true;
                                }
                                else{
                                    medianorder.timestamp = client.timestamp;
                                }
                                if (vflag){
                                    verbose(client, temp2, difference, temp2.priceLimit);
                                }
                                medianorder.index_num = client.index_num;
                            }
                        }
                    }
                }
                medianorder.medprice = 0;
                medianorder.time_change = false;
                eq2[client.equityID].buyers.push(client);
                if (quant > 0){
                    eq[client.equityID].buyers.push(client);//add to priority queue
                }
                if (trans == 0){
                    eq[client.equityID].buyers.push(client);//add to priority queue
                }
                current_timestamp = timestamp;
            }
            else if (client.intent == "SELL"){//if client is seller
                int trans = 0;
                quant = 0;
                counter2 = 0;
                if (!eq[client.equityID].buyers.empty()){
                    difference = 0;
                    if (client.priceLimit <= eq[client.equityID].buyers.top().priceLimit){//if you can sell to buyer on market
                        trans++;
                        if (client.quantity > eq[client.equityID].buyers.top().quantity){//you have more shares than guy can buy
                            client.quantity = client.quantity - eq[client.equityID].buyers.top().quantity;
                            quant = client.quantity - eq[client.equityID].buyers.top().quantity;
                            difference = eq[client.equityID].buyers.top().quantity;
                            client.num_sold = difference;
                            orders[client_ID].profit += (eq[client.equityID].buyers.top().priceLimit * client.num_sold);
                            orders[client_ID].num_sold += client.num_sold;
                            orders[eq[client.equityID].buyers.top().clientID].profit -= (eq[client.equityID].buyers.top().priceLimit * client.num_sold);
                            orders[eq[client.equityID].buyers.top().clientID].num_bought += client.num_sold;
                            temp2.clientID = eq[client.equityID].buyers.top().clientID;
                            temp2.priceLimit = eq[client.equityID].buyers.top().priceLimit;
                            temp2.equityID = eq[client.equityID].buyers.top().equityID;
                            temp2.timestamp = eq[client.equityID].buyers.top().timestamp;
                            eq[client.equityID].buyers.pop();
                        }
                        else if (client.quantity == eq[client.equityID].buyers.top().quantity){//equal shares
                            difference = client.quantity;
                            client.num_sold = difference;
                            quant = 0;
                            client.quantity = 0;
                            orders[client_ID].profit += (eq[client.equityID].buyers.top().priceLimit * client.num_sold);
                            orders[client_ID].num_sold += client.num_sold;
                            orders[eq[client.equityID].buyers.top().clientID].profit -= (eq[client.equityID].buyers.top().priceLimit * client.num_sold);
                            orders[eq[client.equityID].buyers.top().clientID].num_bought += client.num_sold;
                            temp2.clientID = eq[client.equityID].buyers.top().clientID;
                            temp2.priceLimit = eq[client.equityID].buyers.top().priceLimit;
                            temp2.equityID = eq[client.equityID].buyers.top().equityID;
                            temp2.timestamp = eq[client.equityID].buyers.top().timestamp;
                            eq[client.equityID].buyers.pop();
                        }
                        else{//they want to buy more than you can sell
                            difference = client.quantity;
                            client.num_sold = difference;
                            quant = 0;
                            client.quantity = 0;
                            orders[client_ID].profit += (eq[client.equityID].buyers.top().priceLimit * client.num_sold);
                            orders[client_ID].num_sold += client.num_sold;
                            orders[eq[client.equityID].buyers.top().clientID].profit -= (eq[client.equityID].buyers.top().priceLimit * client.num_sold);
                            orders[eq[client.equityID].buyers.top().clientID].num_bought += client.num_sold;
                            order client2 = eq[client.equityID].buyers.top();
                            client2.quantity = eq[client.equityID].buyers.top().quantity - difference;
                            temp2.clientID = eq[client.equityID].buyers.top().clientID;
                            temp2.priceLimit = eq[client.equityID].buyers.top().priceLimit;
                            temp2.equityID = eq[client.equityID].buyers.top().equityID;
                            temp2.timestamp = eq[client.equityID].buyers.top().timestamp;
                            eq[client.equityID].buyers.pop();
                            eq[client.equityID].buyers.push(client2);
                        }
                        num_processed++;
                        medianorder.medprice = temp2.priceLimit;
                        medianorder.equityID = temp2.equityID;
                        prices.push_back(temp2);
                        if (prices.size() == 1){
                            transactions++;
                            equities2.push_back(medianorder);
                        }
                        for (unsigned int l = 0; l < prices.size()-1; ++l){
                            if (temp2.equityID == prices[l].equityID){
                                counter2++;
                            }
                        }
                        if (counter2 == 0 && prices.size() != 1){
                            transactions++;
                            equities2.push_back(medianorder);
                            insertionSort_equities(equities2);
                        }
                        counter++;
                        previous = client.timestamp;
                        if (medianorder.timestamp != previous && medianorder.time_change){
                            medianorder.timestamp = 0;
                            medianorder.time_change = true;
                        }
                        else{
                            medianorder.timestamp = client.timestamp;
                        }
                        if (vflag){
                            verbose(temp2, client, difference, temp2.priceLimit);
                        }
                        medianorder.index_num = client.index_num;
                        if (!eq[client.equityID].buyers.empty()){
                            while(client.quantity != 0 && eq[client.equityID].buyers.top().priceLimit >= client.priceLimit){
                                difference = 0;
                                if (client.quantity > eq[client.equityID].buyers.top().quantity){//you have more shares than guy can buy
                                    client.quantity = client.quantity - eq[client.equityID].buyers.top().quantity;
                                    quant = client.quantity - eq[client.equityID].buyers.top().quantity;
                                    difference = eq[client.equityID].buyers.top().quantity;
                                    client.num_sold = difference;
                                    orders[client_ID].profit += (eq[client.equityID].buyers.top().priceLimit * client.num_sold);
                                    orders[client_ID].num_sold += client.num_sold;
                                    orders[eq[client.equityID].buyers.top().clientID].profit -= (eq[client.equityID].buyers.top().priceLimit * client.num_sold);
                                    orders[eq[client.equityID].buyers.top().clientID].num_bought += client.num_sold;
                                    temp2.clientID = eq[client.equityID].buyers.top().clientID;
                                    temp2.priceLimit = eq[client.equityID].buyers.top().priceLimit;
                                    temp2.equityID = eq[client.equityID].buyers.top().equityID;
                                    temp2.timestamp = eq[client.equityID].buyers.top().timestamp;
                                    eq[client.equityID].buyers.pop();
                                }
                                else if (client.quantity == eq[client.equityID].buyers.top().quantity){//equal shares
                                    difference = client.quantity;
                                    client.num_sold = difference;
                                    quant = 0;
                                    client.quantity = 0;
                                    orders[client_ID].profit += (eq[client.equityID].buyers.top().priceLimit * client.num_sold);
                                    orders[client_ID].num_sold += client.num_sold;
                                    orders[eq[client.equityID].buyers.top().clientID].profit -= (eq[client.equityID].buyers.top().priceLimit * client.num_sold);
                                    orders[eq[client.equityID].buyers.top().clientID].num_bought += client.num_sold;
                                    temp2.clientID = eq[client.equityID].buyers.top().clientID;
                                    temp2.priceLimit = eq[client.equityID].buyers.top().priceLimit;
                                    temp2.equityID = eq[client.equityID].buyers.top().equityID;
                                    temp2.timestamp = eq[client.equityID].buyers.top().timestamp;
                                    eq[client.equityID].buyers.pop();
                                }
                                else{//they want to buy more than you can sell
                                    difference = client.quantity;
                                    client.num_sold = difference;
                                    quant = 0;
                                    client.quantity = 0;
                                    orders[client_ID].profit += (eq[client.equityID].buyers.top().priceLimit * client.num_sold);
                                    orders[client_ID].num_sold += client.num_sold;
                                    orders[eq[client.equityID].buyers.top().clientID].profit -= (eq[client.equityID].buyers.top().priceLimit * client.num_sold);
                                    orders[eq[client.equityID].buyers.top().clientID].num_bought += client.num_sold;
                                    order client2 = eq[client.equityID].buyers.top();
                                    client2.quantity = eq[client.equityID].buyers.top().quantity - difference;
                                    temp2.clientID = eq[client.equityID].buyers.top().clientID;
                                    temp2.priceLimit = eq[client.equityID].buyers.top().priceLimit;
                                    temp2.equityID = eq[client.equityID].buyers.top().equityID;
                                    temp2.timestamp = eq[client.equityID].buyers.top().timestamp;
                                    eq[client.equityID].buyers.pop();
                                    eq[client.equityID].buyers.push(client2);
                                }
                                num_processed++;
                                medianorder.medprice = temp2.priceLimit;
                                medianorder.equityID = temp2.equityID;
                                prices.push_back(temp2);
                                if (prices.size() == 1){
                                    transactions++;
                                    equities2.push_back(medianorder);
                                }
                                for (unsigned int l = 0; l < prices.size()-1; ++l){
                                    if (temp2.equityID == prices[l].equityID){
                                        counter2++;
                                    }
                                }
                                if (counter2 == 0 && prices.size() != 1){
                                    transactions++;
                                    equities2.push_back(medianorder);
                                    insertionSort_equities(equities2);
                                }
                                counter++;
                                previous = client.timestamp;
                                if (medianorder.timestamp != previous && medianorder.time_change){
                                    medianorder.timestamp = 0;
                                    medianorder.time_change = true;
                                }
                                else{
                                    medianorder.timestamp = client.timestamp;
                                }
                                if (vflag){
                                    verbose(temp2, client, difference, temp2.priceLimit);
                                }
                                medianorder.index_num = client.index_num;
                            }
                        }
                    }
                }
                medianorder.time_change = false;
                eq2[client.equityID].sellers.push(client);
                if (quant > 0){
                    eq[client.equityID].sellers.push(client);//add to priority queue
                }
                if (trans == 0){
                    eq[client.equityID].sellers.push(client);//add to priority queue
                }
                current_timestamp = timestamp;
            }
        }
    }
    
    else if (input == "PR"){
        pr_mode = true;
        int seed = 0, num_orders = 0, arrival_rate = 0;
        cin >> random >> seed;
        cin >> random >> num_orders;
        cin >> random >> arrival_rate;
        P2::PR_init(ss, seed, num_equity, num_client, num_orders, arrival_rate);
        istream& inputStream = pr_mode ? ss : cin;
        while (inputStream >> timestamp >> c >> client_ID >> buy_sell >> c >> equity_num >> c >> priceL >> c >> quantity){
            if (previous_timestamp <= timestamp){
                previous_timestamp = timestamp;
            }
            client.timestamp = timestamp;
            client.clientID = client_ID;
            client.intent = buy_sell;
            client.equityID = equity_num;
            client.priceLimit = priceL;
            client.quantity = quantity;
            client.index_num = index_num;
            index_num++;
            if (client.timestamp != current_timestamp){
                if (mflag && counter > 0){
                    for (int x = 0; x < transactions; x++){
                        medprice = medianprice(prices, equities2[x].equityID, 0);
                        median(medprice, current_timestamp, equities2[x].equityID);
                        medianorder.medprice = 0;
                    }
                }
                current_timestamp = client.timestamp;
            }
            if (client.intent == "BUY"){ //if client is buyer
                int trans = 0;
                quant = 0;
                if (!eq[client.equityID].sellers.empty()){
                    difference = 0;
                    if (client.priceLimit >= eq[client.equityID].sellers.top().priceLimit){//if you can buy from seller on market
                        trans++;
                        if (client.quantity > eq[client.equityID].sellers.top().quantity){//you want to buy more shares than guy can sell
                            client.quantity = client.quantity - eq[client.equityID].sellers.top().quantity;
                            quant = client.quantity - eq[client.equityID].sellers.top().quantity;
                            difference = eq[client.equityID].sellers.top().quantity;
                            client.num_bought = difference;
                            orders[client_ID].profit -= (eq[client.equityID].sellers.top().priceLimit * client.num_bought);
                            orders[client_ID].num_bought += client.num_bought;
                            orders[eq[client.equityID].sellers.top().clientID].profit += (eq[client.equityID].sellers.top().priceLimit * client.num_bought);
                            orders[eq[client.equityID].sellers.top().clientID].num_sold += client.num_bought;
                            temp2.clientID = eq[client.equityID].sellers.top().clientID;
                            temp2.priceLimit = eq[client.equityID].sellers.top().priceLimit;
                            temp2.equityID = eq[client.equityID].sellers.top().equityID;
                            temp2.timestamp = eq[client.equityID].sellers.top().timestamp;
                            eq[client.equityID].sellers.pop();
                        }
                        else if (client.quantity == eq[client.equityID].sellers.top().quantity){//equal shares
                            difference = client.quantity;
                            client.num_bought = difference;
                            quant = 0;
                            client.quantity = 0;
                            orders[client_ID].profit -= (eq[client.equityID].sellers.top().priceLimit * client.num_bought);
                            orders[client_ID].num_bought += client.num_bought;
                            orders[eq[client.equityID].sellers.top().clientID].profit += (eq[client.equityID].sellers.top().priceLimit * client.num_bought);
                            orders[eq[client.equityID].sellers.top().clientID].num_sold += client.num_bought;
                            temp2.clientID = eq[client.equityID].sellers.top().clientID;
                            temp2.priceLimit = eq[client.equityID].sellers.top().priceLimit;
                            temp2.equityID = eq[client.equityID].sellers.top().equityID;
                            temp2.timestamp = eq[client.equityID].sellers.top().timestamp;
                            eq[client.equityID].sellers.pop();
                        }
                        else{//they have more shares than you want to buy
                            difference = client.quantity;
                            client.num_bought = difference;
                            quant = 0;
                            client.quantity = 0;
                            orders[client_ID].profit -= (eq[client.equityID].sellers.top().priceLimit * client.num_bought);
                            orders[client_ID].num_bought += client.num_bought;
                            orders[eq[client.equityID].sellers.top().clientID].profit += (eq[client.equityID].sellers.top().priceLimit * client.num_bought);
                            orders[eq[client.equityID].sellers.top().clientID].num_sold += client.num_bought;
                            temp2.clientID = eq[client.equityID].sellers.top().clientID;
                            temp2.priceLimit = eq[client.equityID].sellers.top().priceLimit;
                            temp2.equityID = eq[client.equityID].sellers.top().equityID;
                            temp2.timestamp = eq[client.equityID].sellers.top().timestamp;
                            order client2 = eq[client.equityID].sellers.top();
                            client2.quantity = eq[client.equityID].sellers.top().quantity - difference;
                            eq[client.equityID].sellers.pop();
                            eq[client.equityID].sellers.push(client2);
                        }
                        num_processed++;
                        medianorder.medprice = temp2.priceLimit;
                        medianorder.equityID = temp2.equityID;
                        prices.push_back(temp2); // add prices to list to find middle 2 for even # of prices
                        if (prices.size() == 1){
                            transactions++;
                            equities2.push_back(medianorder);
                        }
                        for (unsigned int l = 0; l < prices.size()-1; ++l){
                            if (temp2.equityID == prices[l].equityID){
                                counter2++;
                            }
                        }
                        if (counter2 == 0 && prices.size() != 1){
                            transactions++;
                            equities2.push_back(medianorder);
                            insertionSort_equities(equities2);
                        }
                        counter++;
                        previous = client.timestamp;
                        if (medianorder.timestamp != previous && medianorder.time_change){ // if timestamp changed
                            medianorder.timestamp = 0;
                            medianorder.time_change = true;
                        }
                        else{
                            medianorder.timestamp = client.timestamp;
                        }
                        if (vflag){
                            verbose(client, temp2, difference, temp2.priceLimit);
                        }
                        medianorder.index_num = client.index_num;
                        if (!eq[client.equityID].sellers.empty()){
                            while(client.quantity != 0 && eq[client.equityID].sellers.top().priceLimit >= client.priceLimit){
                                difference = 0;
                                if (client.quantity > eq[client.equityID].sellers.top().quantity){//you want to buy more shares than guy can sell
                                    client.quantity = client.quantity - eq[client.equityID].sellers.top().quantity;
                                    quant = client.quantity - eq[client.equityID].sellers.top().quantity;
                                    difference = eq[client.equityID].sellers.top().quantity;
                                    client.num_bought = difference;
                                    orders[client_ID].profit -= (eq[client.equityID].sellers.top().priceLimit * client.num_bought);
                                    orders[client_ID].num_bought += client.num_bought;
                                    orders[eq[client.equityID].sellers.top().clientID].profit += (eq[client.equityID].sellers.top().priceLimit * client.num_bought);
                                    orders[eq[client.equityID].sellers.top().clientID].num_sold += client.num_bought;
                                    temp2.clientID = eq[client.equityID].sellers.top().clientID;
                                    temp2.priceLimit = eq[client.equityID].sellers.top().priceLimit;
                                    temp2.equityID = eq[client.equityID].sellers.top().equityID;
                                    temp2.timestamp = eq[client.equityID].sellers.top().timestamp;
                                    eq[client.equityID].sellers.pop();
                                }
                                else if (client.quantity == eq[client.equityID].sellers.top().quantity){//equal shares
                                    difference = client.quantity;
                                    client.num_bought = difference;
                                    quant = 0;
                                    client.quantity = 0;
                                    orders[client_ID].profit -= (eq[client.equityID].sellers.top().priceLimit * client.num_bought);
                                    orders[client_ID].num_bought += client.num_bought;
                                    orders[eq[client.equityID].sellers.top().clientID].profit += (eq[client.equityID].sellers.top().priceLimit * client.num_bought);
                                    orders[eq[client.equityID].sellers.top().clientID].num_sold += client.num_bought;
                                    temp2.clientID = eq[client.equityID].sellers.top().clientID;
                                    temp2.priceLimit = eq[client.equityID].sellers.top().priceLimit;
                                    temp2.equityID = eq[client.equityID].sellers.top().equityID;
                                    temp2.timestamp = eq[client.equityID].sellers.top().timestamp;
                                    eq[client.equityID].sellers.pop();
                                }
                                else{//they have more shares than you want to buy
                                    difference = client.quantity;
                                    client.num_bought = difference;
                                    quant = 0;
                                    client.quantity = 0;
                                    orders[client_ID].profit -= (eq[client.equityID].sellers.top().priceLimit * client.num_bought);
                                    orders[client_ID].num_bought += client.num_bought;
                                    orders[eq[client.equityID].sellers.top().clientID].profit += (eq[client.equityID].sellers.top().priceLimit * client.num_bought);
                                    orders[eq[client.equityID].sellers.top().clientID].num_sold += client.num_bought;
                                    temp2.clientID = eq[client.equityID].sellers.top().clientID;
                                    temp2.priceLimit = eq[client.equityID].sellers.top().priceLimit;
                                    temp2.equityID = eq[client.equityID].sellers.top().equityID;
                                    temp2.timestamp = eq[client.equityID].sellers.top().timestamp;
                                    order client2 = eq[client.equityID].sellers.top();
                                    client2.quantity = eq[client.equityID].sellers.top().quantity - difference;
                                    eq[client.equityID].sellers.pop();
                                    eq[client.equityID].sellers.push(client2);
                                }
                                num_processed++;
                                medianorder.medprice = temp2.priceLimit;
                                medianorder.equityID = temp2.equityID;
                                prices.push_back(temp2);
                                if (prices.size() == 1){
                                    transactions++;
                                    equities2.push_back(medianorder);
                                }
                                for (unsigned int l = 0; l < prices.size()-1; ++l){
                                    if (temp2.equityID == prices[l].equityID){
                                        counter2++;
                                    }
                                }
                                if (counter2 == 0 && prices.size() != 1){
                                    transactions++;
                                    equities2.push_back(medianorder);
                                    insertionSort_equities(equities2);
                                }
                                counter++;
                                previous = client.timestamp;
                                if (medianorder.timestamp != previous && medianorder.time_change){
                                    medianorder.timestamp = 0;
                                    medianorder.time_change = true;
                                }
                                else{
                                    medianorder.timestamp = client.timestamp;
                                }
                                if (vflag){
                                    verbose(client, temp2, difference, temp2.priceLimit);
                                }
                                medianorder.index_num = client.index_num;
                            }
                        }
                    }
                }
                medianorder.medprice = 0;
                medianorder.time_change = false;
                eq2[client.equityID].buyers.push(client);
                if (quant > 0){
                    eq[client.equityID].buyers.push(client);//add to priority queue
                }
                if (trans == 0){
                    eq[client.equityID].buyers.push(client);//add to priority queue
                }
                current_timestamp = timestamp;
            }
            else if (client.intent == "SELL"){//if client is seller
                int trans = 0;
                quant = 0;
                if (!eq[client.equityID].buyers.empty()){
                    difference = 0;
                    if (client.priceLimit <= eq[client.equityID].buyers.top().priceLimit){//if you can sell to buyer on market
                        trans++;
                        if (client.quantity > eq[client.equityID].buyers.top().quantity){//you have more shares than guy can buy
                            client.quantity = client.quantity - eq[client.equityID].buyers.top().quantity;
                            quant = client.quantity - eq[client.equityID].buyers.top().quantity;
                            difference = eq[client.equityID].buyers.top().quantity;
                            client.num_sold = difference;
                            orders[client_ID].profit += (eq[client.equityID].buyers.top().priceLimit * client.num_sold);
                            orders[client_ID].num_sold += client.num_sold;
                            orders[eq[client.equityID].buyers.top().clientID].profit -= (eq[client.equityID].buyers.top().priceLimit * client.num_sold);
                            orders[eq[client.equityID].buyers.top().clientID].num_bought += client.num_sold;
                            temp2.clientID = eq[client.equityID].buyers.top().clientID;
                            temp2.priceLimit = eq[client.equityID].buyers.top().priceLimit;
                            temp2.equityID = eq[client.equityID].buyers.top().equityID;
                            temp2.timestamp = eq[client.equityID].buyers.top().timestamp;
                            eq[client.equityID].buyers.pop();
                        }
                        else if (client.quantity == eq[client.equityID].buyers.top().quantity){//equal shares
                            difference = client.quantity;
                            client.num_sold = difference;
                            quant = 0;
                            client.quantity = 0;
                            orders[client_ID].profit += (eq[client.equityID].buyers.top().priceLimit * client.num_sold);
                            orders[client_ID].num_sold += client.num_sold;
                            orders[eq[client.equityID].buyers.top().clientID].profit -= (eq[client.equityID].buyers.top().priceLimit * client.num_sold);
                            orders[eq[client.equityID].buyers.top().clientID].num_bought += client.num_sold;
                            temp2.clientID = eq[client.equityID].buyers.top().clientID;
                            temp2.priceLimit = eq[client.equityID].buyers.top().priceLimit;
                            temp2.equityID = eq[client.equityID].buyers.top().equityID;
                            temp2.timestamp = eq[client.equityID].buyers.top().timestamp;
                            eq[client.equityID].buyers.pop();
                        }
                        else{//they want to buy more than you can sell
                            difference = client.quantity;
                            client.num_sold = difference;
                            quant = 0;
                            client.quantity = 0;
                            orders[client_ID].profit += (eq[client.equityID].buyers.top().priceLimit * client.num_sold);
                            orders[client_ID].num_sold += client.num_sold;
                            orders[eq[client.equityID].buyers.top().clientID].profit -= (eq[client.equityID].buyers.top().priceLimit * client.num_sold);
                            orders[eq[client.equityID].buyers.top().clientID].num_bought += client.num_sold;
                            order client2 = eq[client.equityID].buyers.top();
                            client2.quantity = eq[client.equityID].buyers.top().quantity - difference;
                            temp2.clientID = eq[client.equityID].buyers.top().clientID;
                            temp2.priceLimit = eq[client.equityID].buyers.top().priceLimit;
                            temp2.equityID = eq[client.equityID].buyers.top().equityID;
                            temp2.timestamp = eq[client.equityID].buyers.top().timestamp;
                            eq[client.equityID].buyers.pop();
                            eq[client.equityID].buyers.push(client2);
                        }
                        num_processed++;
                        medianorder.medprice = temp2.priceLimit;
                        medianorder.equityID = temp2.equityID;
                        prices.push_back(temp2);
                        if (prices.size() == 1){
                            transactions++;
                            equities2.push_back(medianorder);
                        }
                        for (unsigned int l = 0; l < prices.size()-1; ++l){
                            if (temp2.equityID == prices[l].equityID){
                                counter2++;
                            }
                        }
                        if (counter2 == 0 && prices.size() != 1){
                            transactions++;
                            equities2.push_back(medianorder);
                            insertionSort_equities(equities2);
                        }
                        counter++;
                        previous = client.timestamp;
                        if (medianorder.timestamp != previous && medianorder.time_change){
                            medianorder.timestamp = 0;
                            medianorder.time_change = true;
                        }
                        else{
                            medianorder.timestamp = client.timestamp;
                        }
                        if (vflag){
                            verbose(temp2, client, difference, temp2.priceLimit);
                        }
                        medianorder.index_num = client.index_num;
                        if (!eq[client.equityID].buyers.empty()){
                            while(client.quantity != 0 && eq[client.equityID].buyers.top().priceLimit >= client.priceLimit){
                                difference = 0;
                                if (client.quantity > eq[client.equityID].buyers.top().quantity){//you have more shares than guy can buy
                                    client.quantity = client.quantity - eq[client.equityID].buyers.top().quantity;
                                    quant = client.quantity - eq[client.equityID].buyers.top().quantity;
                                    difference = eq[client.equityID].buyers.top().quantity;
                                    client.num_sold = difference;
                                    orders[client_ID].profit += (eq[client.equityID].buyers.top().priceLimit * client.num_sold);
                                    orders[client_ID].num_sold += client.num_sold;
                                    orders[eq[client.equityID].buyers.top().clientID].profit -= (eq[client.equityID].buyers.top().priceLimit * client.num_sold);
                                    orders[eq[client.equityID].buyers.top().clientID].num_bought += client.num_sold;
                                    temp2.clientID = eq[client.equityID].buyers.top().clientID;
                                    temp2.priceLimit = eq[client.equityID].buyers.top().priceLimit;
                                    temp2.equityID = eq[client.equityID].buyers.top().equityID;
                                    temp2.timestamp = eq[client.equityID].buyers.top().timestamp;
                                    eq[client.equityID].buyers.pop();
                                }
                                else if (client.quantity == eq[client.equityID].buyers.top().quantity){//equal shares
                                    difference = client.quantity;
                                    client.num_sold = difference;
                                    quant = 0;
                                    client.quantity = 0;
                                    orders[client_ID].profit += (eq[client.equityID].buyers.top().priceLimit * client.num_sold);
                                    orders[client_ID].num_sold += client.num_sold;
                                    orders[eq[client.equityID].buyers.top().clientID].profit -= (eq[client.equityID].buyers.top().priceLimit * client.num_sold);
                                    orders[eq[client.equityID].buyers.top().clientID].num_bought += client.num_sold;
                                    temp2.clientID = eq[client.equityID].buyers.top().clientID;
                                    temp2.priceLimit = eq[client.equityID].buyers.top().priceLimit;
                                    temp2.equityID = eq[client.equityID].buyers.top().equityID;
                                    temp2.timestamp = eq[client.equityID].buyers.top().timestamp;
                                    eq[client.equityID].buyers.pop();
                                }
                                else{//they want to buy more than you can sell
                                    difference = client.quantity;
                                    client.num_sold = difference;
                                    quant = 0;
                                    client.quantity = 0;
                                    orders[client_ID].profit += (eq[client.equityID].buyers.top().priceLimit * client.num_sold);
                                    orders[client_ID].num_sold += client.num_sold;
                                    orders[eq[client.equityID].buyers.top().clientID].profit -= (eq[client.equityID].buyers.top().priceLimit * client.num_sold);
                                    orders[eq[client.equityID].buyers.top().clientID].num_bought += client.num_sold;
                                    order client2 = eq[client.equityID].buyers.top();
                                    client2.quantity = eq[client.equityID].buyers.top().quantity - difference;
                                    temp2.clientID = eq[client.equityID].buyers.top().clientID;
                                    temp2.priceLimit = eq[client.equityID].buyers.top().priceLimit;
                                    temp2.equityID = eq[client.equityID].buyers.top().equityID;
                                    temp2.timestamp = eq[client.equityID].buyers.top().timestamp;
                                    eq[client.equityID].buyers.pop();
                                    eq[client.equityID].buyers.push(client2);
                                }
                                num_processed++;
                                medianorder.medprice = temp2.priceLimit;
                                medianorder.equityID = temp2.equityID;
                                prices.push_back(temp2);
                                if (prices.size() == 1){
                                    transactions++;
                                    equities2.push_back(medianorder);
                                }
                                for (unsigned int l = 0; l < prices.size()-1; ++l){
                                    if (temp2.equityID == prices[l].equityID){
                                        counter2++;
                                    }
                                }
                                if (counter2 == 0 && prices.size() != 1){
                                    transactions++;
                                    equities2.push_back(medianorder);
                                    insertionSort_equities(equities2);
                                }
                                counter++;
                                previous = client.timestamp;
                                if (medianorder.timestamp != previous && medianorder.time_change){
                                    medianorder.timestamp = 0;
                                    medianorder.time_change = true;
                                }
                                else{
                                    medianorder.timestamp = client.timestamp;
                                }
                                if (vflag){
                                    verbose(temp2, client, difference, temp2.priceLimit);
                                }
                                medianorder.index_num = client.index_num;
                            }
                        }
                    }
                }
                medianorder.time_change = false;
                eq2[client.equityID].sellers.push(client);
                if (quant > 0){
                    eq[client.equityID].sellers.push(client);//add to priority queue
                }
                if (trans == 0){
                    eq[client.equityID].sellers.push(client);//add to priority queue
                }
                current_timestamp = timestamp;
            }
        }
    }
    if (mflag){
        for (unsigned int x = 0; x < equities2.size(); ++x){
            medprice = medianprice(prices, equities2[x].equityID, 0);
            median(medprice, current_timestamp, equities2[x].equityID);
        }
    }
    while(!equities.empty()){
        equities.pop_back();
    }
    while(!equities2.empty()){
        equities2.pop_back();
    }
    cout << "---End of Day---\n" << "Orders Processed: " << num_processed << '\n';
    
    if (cflag){
        client_info(orders);
    }
    
    if (tflag){
        time_travelers(eq2, num_equity);
    }
    return 0;
}

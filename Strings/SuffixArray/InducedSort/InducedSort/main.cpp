//
//  main.cpp
//  InducedSort
//
//  Created by Alex on 16.11.15.
//  Copyright © 2015 Alex. All rights reserved.
//

#include <iostream>
#include "SAInducedSort.hpp"

using namespace std;
typedef long long ll;
int main(int argc, const char * argv[]) {
    // insert code here...
    string str;
    cin >> str;
    vector<ll> suff_array;

    SAInducedSort::sort(str,suff_array);
    for(int i = 0; i < suff_array.size(); ++i){
        cout << suff_array[i] <<" ";
    }
    return 0;
}

//
//  main.cpp
//  InducedSort
//
//  Created by Alex on 16.11.15.
//  Copyright © 2015 Alex. All rights reserved.
//

#include <iostream>
#include "InducedSort.hpp"

using namespace std;
int main(int argc, const char * argv[]) {
    // insert code here...
    string str;
    cin >> str;
    vector<size_t> suff_array;

    InducedSorter::sort(str,suff_array);
    for(int i = 0; i < suff_array.size(); ++i){
        cout << suff_array[i] <<" ";
    }
    return 0;
}

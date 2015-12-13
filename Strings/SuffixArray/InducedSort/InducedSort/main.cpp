//
//  main.cpp
//  InducedSort
//
//  Created by Alex on 16.11.15.
//  Copyright © 2015 Alex. All rights reserved.
//

#include <iostream>
#include "SAInducedSort.hpp"

using std::vector;
using std::string;

typedef long long ll;
ll calc_substrings(const vector<size_t> & suff , const vector<size_t> & lcp) {
    ll ans = 0;
    for(size_t i = 1; i < suff.size(); ++i) {
        ans += ((ll)suff.size() - 1 - (ll)suff[i]) - (ll)lcp[i-1];
    }
    return ans;
}

int main() {
    std::ios::sync_with_stdio(false);
    string str;
    size_t k;
    std::cin >> k >> str;
    string full_str = str + str;
    
    full_str += 1;
    
    vector<size_t> sub_suff_array(k + 1);
    vector<size_t> lcp(k + 1);
    string substr;
    substr.reserve(k + 1);
    vector<ll> ans;
    for(ll i = 0; i < str.size(); ++i) {
        substr = full_str.substr(i,k);
        sub_suff_array = SAInducedSort::buildSA(substr);
        substr += 1;
     
        lcp = SAInducedSort::buildLCP(substr, sub_suff_array);
        ans.push_back(calc_substrings(sub_suff_array, lcp));
    }
    for(ll i = 0; i < ans.size(); ++i) {
        std::cout << ans[i] << " ";
    }
    std::cout << std::endl;
    return 0;
}

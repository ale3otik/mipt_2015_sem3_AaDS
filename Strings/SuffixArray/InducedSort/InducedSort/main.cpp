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

void find_sub_suff(const vector<ll> & suff_arr, vector<ll> & sub_suff, ll l, ll r) {
    ll j = 1;
    sub_suff[0] = sub_suff.size() - 1;
    for(ll i = 0; i < suff_arr.size(); ++i) {
        if(suff_arr[i] < r && suff_arr[i] >= l) {
            sub_suff[j++] = suff_arr[i] - l;
        }
    }
}

ll calc_substrings(const vector<ll> & suff , const vector<ll> & lcp) {
    ll ans = 0;
    for(int i = 1; i < suff.size(); ++i) {
        ans += (suff.size() - 1 - suff[i]) - lcp[i-1];
    }
    return ans;
}

int main() {
    ios::sync_with_stdio(false);
    string str;
    ll k;
    cin >> k >> str;
    string full_str = str + str;
    vector<ll> suff_array;
    
    full_str += 1;
    
    vector<ll> sub_suff(k + 1);
    vector<ll> lcp(k + 1);
    string substr;
    substr.reserve(k + 1);
    vector<ll> ans;
    for(ll i = 0; i < str.size(); ++i) {
        substr = full_str.substr(i,k);
        SAInducedSort::sort(substr,sub_suff);
        substr += 1;
        // find_sub_suff(suff_array, sub_suff, i, i + k);
        SAInducedSort::buildLCP(substr, sub_suff, lcp);
        ans.push_back(calc_substrings(sub_suff, lcp));
    }
    for(ll i = 0; i < ans.size(); ++i) {
        cout << ans[i] << " ";
    }
    cout << endl;
    return 0;
}

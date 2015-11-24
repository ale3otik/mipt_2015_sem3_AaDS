//
//  main.cpp
//  SuffTreeUkkonen
//
//  Created by Alex on 22.11.15.
//  Copyright © 2015 Alex. All rights reserved.
//

#include <iostream>
#include <string>
#include "SuffixTree.hpp"
using namespace std;
typedef long long ll;
int main() {
    ios::sync_with_stdio(false);
    
    string str;
    ll k;
    cin >> k >> str;
    for(int i = 0; i < str.size(); ++i) {
        str[i] -= 'a' - 2;
    }
    
    string full_str = str + str;
    
    string substr;
    substr.reserve(k);
    vector<ll> ans(str.size(),0);
    for(ll j = 0; j < str.size(); ++j) {
        substr = full_str.substr(j,k);
        SuffixTree tree(substr);
        for(int i = 2; i < tree.nodes.size(); ++i) {
            ans[j] += tree.getEdgeLength(i);
        }
    }
    
    for(ll i = 0; i < ans.size(); ++i) {
        cout << ans[i] << " ";
    }
    cout << endl;
    return 0;
}
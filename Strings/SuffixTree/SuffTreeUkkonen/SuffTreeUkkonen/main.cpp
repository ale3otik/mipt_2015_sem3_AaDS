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

typedef long long ll;
using std::vector;
using std::string;

ll getTreeEdgeLengthSumm(const SuffixTree & tree) {
    ll ans = 0;
    for(size_t i = 2; i < tree.nodes.size(); ++i) {
        ans += tree.getEdgeLength(i);
    }
    return ans;
}

int main() {
    std::ios::sync_with_stdio(false);
    string str;
    ll k;
    std::cin >> k >> str;
    for(size_t i = 0; i < str.size(); ++i) {
        str[i] -= 'a' - 2;
    }
    
    string full_str = str + str;
    
    string substr;
    substr.reserve(k);
    
    vector<ll> ans(str.size(),0);
    for(size_t j = 0; j < str.size(); ++j) {
        substr = full_str.substr(j,k);
        SuffixTree tree(substr);
        tree.buildTree();
        ans[j] = getTreeEdgeLengthSumm(tree);
    }
    
    for(ll i = 0; i < ans.size(); ++i) {
        std::cout << ans[i] << " ";
    }
    std::cout << std::endl;
    return 0;
}

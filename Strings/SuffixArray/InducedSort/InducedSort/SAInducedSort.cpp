//
//  SAInducedSort.cpp
//  SAInducedSort
//
//  Created by Alex on 16.11.15.
//  Copyright © 2015 Alex. All rights reserved.
//

#include "SAInducedSort.hpp"
#include <cassert>

typedef long long ll;
using namespace std;

void SAInducedSorter::initLmsPointers(const vector<bool> & type, vector<ll> & lms_pointers, vector<bool> & is_lms) {
    is_lms.assign(type.size(), false);
    for(ll i = 1; i < type.size(); ++i) {
        if(type[i] == S_TYPE && type[i-1] == L_TYPE) {
            lms_pointers.push_back(i);
            is_lms[i] = true;
        }
    }
}

void SAInducedSorter::initType(const std::vector<ll> & str, std::vector<bool> & type) {
    type[str.size() - 1] = S_TYPE;
    for(ll i = str.size() - 2; i >= 0; --i) {
        if(str[i] < str[i+1]) {
            type[i] = S_TYPE;
        } else {
            if(str[i] > str[i+1]) {
                type[i] = L_TYPE;
            } else {
                type[i] = type[i+1];
            }
        }
    }
}

void SAInducedSorter::initBuckets(const std::vector<ll> & str, std::vector<ll> & buckets) {
    buckets.assign(buckets.size(),0);
    for(ll i = 0; i < str.size(); ++i) {
        ++buckets[str[i]];
    }
    
    ll cur_head = 0;
    for(ll i = 0; i < buckets.size(); ++i) {
        cur_head += buckets[i];
        buckets[i] = cur_head - buckets[i];
    }
}

bool SAInducedSorter::isLmsSubstringEqual(const vector<long long> & str,
                                const vector<bool> & type,
                                const vector<bool> & is_lms,
                                const long long fr,
                                const long long sd) {
    ll i = 0;
    while(true) {
        if(str[fr + i] != str[sd + i]) return false;
        if(type[fr + i] != type[sd + i]) return false;
        if(i > 0 && is_lms[fr + i]) return true;
        ++i;
    }
}

void SAInducedSorter::makeNewStr(std::vector<long long> & new_str,
                                 long long & new_alp,
                                 const std::vector<long long> & str,
                                 const std::vector<bool> & is_lms,
                                 const std::vector<bool> & type,
                                 const std::vector<long long> & suff_array) {
    
    /******** make new alphabet ***************************/
    new_alp = 0;
    ll r = suff_array.size() - 1;
    ll l = r - 1;
    vector<ll> alp_type(str.size(),-1);
    alp_type[r] = new_alp;
    
    l = r - 1;
    while(l >= 0) {
        if(is_lms[l]){
            if(isLmsSubstringEqual(str, type, is_lms, l, r)) {
                alp_type[l] = alp_type[r];
            } else {
                ++new_alp;
                alp_type[l] = new_alp;
            }
            r = l;
        }
        --l;
    }
    
    /****************** build new str **********************/
    for(ll i = 0, j = 0; i < str.size(); ++i) {
        if(is_lms[i]) {
            new_str[j] = alp_type[i];
            ++j;
        }
    }
}

/***************sorting of lms prefixes*****************************/
void SAInducedSorter::LMSinducedSort(
                                     const vector<ll> & str,
                                     const vector<ll> & lms_pointers,
                                     vector<ll> & buckets,
                                     vector<ll> & suff_array,
                                     vector<bool> & type) {
    
    vector<ll> bucket_heads(buckets);
    vector<ll> bucket_tails(bucket_heads.size());
    
    for(ll i = 0; i < bucket_tails.size() - 1; ++i) {
        bucket_tails[i] = bucket_heads[i + 1] - 1;
    }
    bucket_tails[bucket_tails.size() - 1] = bucket_tails.size() - 1;
    vector<ll> tmp_bucket_tails(bucket_tails);
    
    //step 1 : first initialization by lms substrings
    for(ll i = 0; i < lms_pointers.size(); ++i) {
        ll lms_head = lms_pointers[i];
        suff_array[tmp_bucket_tails[str[lms_head]]] = lms_head;
        --tmp_bucket_tails[str[lms_head]];
    }
    //step 2 : induced sort of lms prefixes
    // scan from head to end
    // assign from head to end each bucket
    for(ll i = 0; i < suff_array.size(); ++i) {
        if(suff_array[i] <= 0) continue;
        
        ll checked_pos = suff_array[i] - 1;
        if(type[checked_pos] == L_TYPE) {
            suff_array[bucket_heads[str[checked_pos]]] = checked_pos;
            ++bucket_heads[str[checked_pos]];
        }
    }
    
    // scan from end to head
    // assign from end to head each bucket
    for( ll i = suff_array.size() - 1; i >= 0; --i) {
        if(suff_array[i] <= 0) continue;
        
        ll checked_pos = suff_array[i] - 1;
        if(type[checked_pos] == S_TYPE) {
            suff_array[bucket_tails[str[checked_pos]]] = checked_pos;
            --bucket_tails[str[checked_pos]];
        }
    }
}

void SAInducedSorter::SAinducedSort(vector<ll> & str, vector<ll> & suff_array, const ll alp_size) {
    ll length = str.size();
    suff_array.clear();
    suff_array.resize(length,-1);
    
    vector<bool> type(length); // L or S type
    vector<ll> lms_pointers; // positions of all first symb in LMS substrings
    vector<ll> buckets(alp_size); //index of head each i-sym bucket
    vector<bool> is_lms(length);
    
    initType(str, type);
    initLmsPointers(type, lms_pointers, is_lms);
    initBuckets(str,buckets);
    LMSinducedSort(str,lms_pointers,buckets,suff_array,type);
    
    ll new_alp;
    vector<ll> new_str(lms_pointers.size());
    makeNewStr(new_str, new_alp, str, is_lms, type, suff_array);
    
    
}

/********************************************************************/
void SAInducedSorter::sort(const string & inputStr, vector<ll> & ans_suff_array) {
    if(inputStr.size() == 0) return;
    
    ll shift = LONG_MAX;
    
    for(ll i = 0; i < inputStr.size(); ++i){
        if(inputStr[i] < shift) shift = inputStr[i];
    }
    
    vector<ll> str(inputStr.size() + 1);
    str[str.size()-1] = special_symb;
    
    for(ll i = 0; i < inputStr.size(); ++i) {
        str[i] = inputStr[i] - shift + 1;
    }
    
    vector<ll> suff_array;

    SAinducedSort(str, suff_array, ALP_SIZE + 1);
    
    /* repair right answer */
}


//
//  main.cpp
//  InducedSort
//
//  Created by Alex on 16.11.15.
//  Copyright © 2015 Alex. All rights reserved.

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#define LONG_MAX 1e4
using namespace std;
typedef long long ll;

class SAInducedSort{
public:
    static void sort(const std::string & str, std::vector<long long> & suff_array);
    static void buildLCP(const std::string & str,
                         const std::vector<long long> & suff_arr,
                         std::vector<long long> & lcp);
private:

    class RecursiveSorter {
    public:
        RecursiveSorter(const std::vector<long long> & str,
                        std::vector<long long> & suff_array,
                        const long long alp_sz);
        void SAinducedSort();
    private:

        enum TypeOfSort {
            LMSsubstrings,
            InduceSAfromSA1
        };
        
        static const bool L_TYPE = false;
        static const bool S_TYPE = true;

        const long long alp_size;

        const std::vector<long long> & str;
        std::vector<long long> & suff_array;

        std::vector<bool> type; // L or S type
        std::vector<long long> lms_pointers; // positions of all first symb in LMS substrings
        std::vector<bool> is_lms; //index of head each i-sym bucket
        std::vector<long long> buckets;

        std::vector<long long> back_map_alp; // to iduce SA from SA
        // for next recursive step and induce
        long long next_alp;
        std::vector<long long> next_str;
        std::vector<long long> next_suff_array;
        
        bool isLmsSubstringEqual(const long long fr, const long long sd);
        void buildNewStr();
        bool initType(); // return is each symb unique
        void initLmsPointers();
        void initBuckets();
        void lmsInitForSort(std::vector<long long> bucket_tails);
        void saInitForSort(std::vector<long long> bucket_tails);
        void SAsimpleSort();
        void generalInducedSort(TypeOfSort action);
    };
};

void SAInducedSort::RecursiveSorter::initLmsPointers() {
    is_lms.assign(type.size(), false);
    for(ll i = 1; i < type.size(); ++i) {
        if(type[i] == S_TYPE && type[i-1] == L_TYPE) {
            lms_pointers.push_back(i);
            is_lms[i] = true;
        }
    }
}

bool SAInducedSort::RecursiveSorter::initType() {
    if(str.size() == 1) return true;
    bool isEachUnique = true;
    vector<bool> used(alp_size,false);
    
    type[str.size() - 1] = S_TYPE;
    for(ll i = str.size() - 2; i >= 0; --i) {
        
        if(used[str[i]]) isEachUnique = false;
        used[str[i]] = true;
        
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
    return isEachUnique;
}

void SAInducedSort::RecursiveSorter::initBuckets() {
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

bool SAInducedSort::RecursiveSorter::isLmsSubstringEqual(const long long fr, const long long sd) {
    ll i = 0;
    while(true) {
        if(str[fr + i] != str[sd + i]) return false;
        if(type[fr + i] != type[sd + i]) return false;
        if(i > 0 && is_lms[fr + i]) return true;
        ++i;
        assert(fr + i < str.size() && sd + i < str.size());
    }
}

void SAInducedSort::RecursiveSorter::buildNewStr() {
    
    /******** make new alphabet ***************************/
    next_alp = 0;
    ll r,l;
    vector<ll> alp_type(str.size(),-1);
    l = 0;
    r = 1;
    alp_type[suff_array[l]] = 0;
    while(r < suff_array.size()) {
        if(is_lms[suff_array[r]]){
            if(isLmsSubstringEqual(suff_array[l], suff_array[r])) {
                alp_type[suff_array[r]] = alp_type[suff_array[l]];
            } else {
                ++next_alp;
                alp_type[suff_array[r]] = next_alp;
            }
            l = r;
        }
        ++r;
    }
    ++next_alp;
    /****************** build new str **********************/
    for(ll i = 0, j = 0; i < str.size(); ++i) {
        if(is_lms[i]) {
            next_str[j] = alp_type[i];
            ++j;
        }
    }
}

/***************sorting of lms prefixes or induce SA from SA1 **************/
void SAInducedSort::RecursiveSorter::lmsInitForSort(vector<ll> bucket_tails) {
    for(ll i = 0; i < lms_pointers.size(); ++i) {
        ll lms_head = lms_pointers[i];
        suff_array[bucket_tails[str[lms_head]]] = lms_head;
        --bucket_tails[str[lms_head]];
    }
}

void SAInducedSort::RecursiveSorter::saInitForSort(vector<ll> bucket_tails) {
    for(ll i = next_suff_array.size() - 1; i >= 0; --i) {
        ll checked_position = lms_pointers[next_suff_array[i]];
        suff_array[bucket_tails[str[checked_position]]] = checked_position;
        --bucket_tails[str[checked_position]];
    }
}

void SAInducedSort::RecursiveSorter::generalInducedSort(TypeOfSort action) {

    vector<ll> bucket_heads(buckets);
    vector<ll> bucket_tails(bucket_heads.size());
    bucket_tails[bucket_tails.size() - 1] = bucket_tails.size() - 1;
    for(ll i = 0; i < (ll)bucket_tails.size() - 1; ++i) {
        bucket_tails[i] = bucket_heads[i + 1] - 1;
    }
    
    switch (action) {
        case LMSsubstrings:
            //step 1 : first initialization by lms substrings
            lmsInitForSort(bucket_tails);
            break;
        case InduceSAfromSA1:
            saInitForSort(bucket_tails);
            break;
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
    for( ll i = (ll)suff_array.size() - 1; i >= 0; --i) {
        if(suff_array[i] <= 0) continue;
        
        ll checked_pos = suff_array[i] - 1;
        if(type[checked_pos] == S_TYPE) {
            suff_array[bucket_tails[str[checked_pos]]] = checked_pos;
            --bucket_tails[str[checked_pos]];
        }
    }
}
void SAInducedSort::RecursiveSorter::SAsimpleSort() {
    vector<ll> sorted(alp_size,-1);
    for(ll i = 0; i < str.size(); ++i) {
        sorted[str[i]] = i;
    }
    
    ll it = 0;
    for(ll i = 0; i < alp_size; ++i) {
        if(sorted[i] != -1) {
            suff_array[it++] = sorted[i];
        }
    }
}

void SAInducedSort::RecursiveSorter::SAinducedSort() {
    suff_array.clear();
    suff_array.resize(str.size(), -1);
    
    // all symbols are uniq
    
    bool isEachSymbUnique = initType();
    
    if(isEachSymbUnique) {
        SAsimpleSort();
        return;
    }
    initLmsPointers();
    initBuckets();
    generalInducedSort(LMSsubstrings);
    
    next_str.resize(lms_pointers.size());
    next_suff_array.resize(lms_pointers.size());
    buildNewStr();
    
    RecursiveSorter(next_str,next_suff_array,next_alp).SAinducedSort();
    
    suff_array.assign(suff_array.size(), -1);
    generalInducedSort(InduceSAfromSA1);
}

SAInducedSort::RecursiveSorter::RecursiveSorter(const vector<ll> & rcv_str,
                                                vector<ll> & suff_array,
                                                const ll alp_sz):
alp_size(alp_sz),
str(rcv_str),
suff_array(suff_array),
type(rcv_str.size()),
is_lms(rcv_str.size()),
buckets(alp_sz){};

/********************************************************************/

void SAInducedSort::buildLCP(const string & str, const std::vector<ll> & suff_array, std::vector<ll>  &lcp) {
    ll length = str.size();
    if(length != lcp.size()) lcp.resize(length);

    vector<ll> pos(length);
    for(ll i = 0; i < length; ++i) {
        pos[suff_array[i]] = i;
    }
    
    ll k = 0;
    for(ll i = 0; i < length; ++i)
    {
        if(k > 0) --k;
        if(pos[i] == length - 1) {
            lcp[length - 1] = -1;
            k = 0;
        } else {
            ll j = suff_array[pos[i] + 1];
            while(max(i + k, j + k) < length && str[i + k] == str[j + k]) {
                ++k;
            }
            
            lcp[pos[i]] = k;
        }
    }
}

/********************************************************************/
void SAInducedSort::sort(const string & inputStr, vector<ll> & suff_array) {
    if(inputStr.size() == 0) return;
    ll alp_size = 0;
    ll shift = LONG_MAX;
   
    for(ll i = 0; i < inputStr.size(); ++i){
        if(inputStr[i] < shift) shift = inputStr[i];
        if(inputStr[i] > alp_size) alp_size = inputStr[i];
    }
    
    vector<ll> str(inputStr.size() + 1);
    str[str.size()-1] = 0; // special symb
    
    //to count different symbs
    for(ll i = 0; i < inputStr.size(); ++i) {
        str[i] = inputStr[i] - shift + 1;
    }
    alp_size -= shift - 2;

    RecursiveSorter(str,suff_array,alp_size).SAinducedSort();
}


/**********************************************************************************/
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
    for(int i = 1; i < (ll)suff.size(); ++i) {
        ans += ((ll)suff.size() - 1 - suff[i]) - lcp[i-1];
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


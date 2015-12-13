//
//  SAInducedSort.cpp
//  SAInducedSort
//
//  Created by Alex on 16.11.15.
//  Copyright © 2015 Alex. All rights reserved.
//

#include "SAInducedSort.hpp"

using std::vector;
using std::string;

void SAInducedSort::RecursiveSorter::initLmsPointers_() {
    is_lms_.assign(type_.size(), false);
    for(size_t i = 1; i < type_.size(); ++i) {
        if(type_[i] == S_TYPE_ && type_[i-1] == L_TYPE_) {
            lms_pointers_.push_back(i);
            is_lms_[i] = true;
        }
    }
}

bool SAInducedSort::RecursiveSorter::initType_() {
    if(str_.size() == 1) return true;
    bool isAllUnique = true;
    vector<bool> used(alp_size_,false);
    
    type_[str_.size() - 1] = S_TYPE_;
    for(size_t j = 1; j < str_.size(); ++j) {
        size_t i = str_.size() - j - 1;
        if(used[str_[i]]) isAllUnique = false;
        used[str_[i]] = true;
        
        if(str_[i] < str_[i+1]) {
            type_[i] = S_TYPE_;
        } else {
            if(str_[i] > str_[i+1]) {
                type_[i] = L_TYPE_;
            } else {
                type_[i] = type_[i+1];
            }
        }
    }
    return isAllUnique;
}

void SAInducedSort::RecursiveSorter::initBuckets_() {
    buckets_.assign(buckets_.size(),0);
    for(size_t i = 0; i < str_.size(); ++i) {
        ++buckets_[str_[i]];
    }
    
    size_t cur_head = 0;
    for(size_t i = 0; i < buckets_.size(); ++i) {
        cur_head += buckets_[i];
        buckets_[i] = cur_head - buckets_[i];
    }
}

bool SAInducedSort::RecursiveSorter::isLmsSubstringEqual_(size_t first,size_t second) {
    size_t i = 0;
    while(true) {
        if(str_[first + i] != str_[second + i]) return false;
        if(type_[first + i] != type_[second + i]) return false;
        if(i > 0 && is_lms_[first + i]) return true;
        ++i;
    }
}

void SAInducedSort::RecursiveSorter::buildNewStr_() {
    
    /******** make new alphabet ***************************/
    next_alp_size_ = 0;
    size_t r,l;
    vector<size_t> alp_type(str_.size(),SAInducedSort::INF);
    l = 0;
    r = 1;
    alp_type[suff_array_[l]] = 0;
    while(r < suff_array_.size()) {
        if(is_lms_[suff_array_[r]]){
            if(isLmsSubstringEqual_(suff_array_[l], suff_array_[r])) {
                alp_type[suff_array_[r]] = alp_type[suff_array_[l]];
            } else {
                ++next_alp_size_;
                alp_type[suff_array_[r]] = next_alp_size_;
            }
            l = r;
        }
        ++r;
    }
    ++next_alp_size_;
    
    /****************** build new str **********************/
    for(size_t i = 0, j = 0; i < str_.size(); ++i) {
        if(is_lms_[i]) {
            next_str_[j] = alp_type[i];
            ++j;
        }
    }
}

/***************sort lms prefixes or induce SA from SA1 **************/
void SAInducedSort::RecursiveSorter::lmsInitForSort_(vector<size_t> bucket_tails) {
    for(size_t i = 0; i < lms_pointers_.size(); ++i) {
        size_t lms_head = lms_pointers_[i];
        suff_array_[bucket_tails[str_[lms_head]]] = lms_head;
        --bucket_tails[str_[lms_head]];
    }
}

void SAInducedSort::RecursiveSorter::saInitForSort_(vector<size_t> bucket_tails) {
    for(size_t i = next_suff_array_.size() - 1; i != SAInducedSort::INF; --i) {
        size_t checked_position = lms_pointers_[next_suff_array_[i]];
        suff_array_[bucket_tails[str_[checked_position]]] = checked_position;
        --bucket_tails[str_[checked_position]];
    }
}

void SAInducedSort::RecursiveSorter::generalInducedSort_(ETS_TYPE_OF_SORT_ type_of_sort) {
    vector<size_t> bucket_heads(buckets_);
    vector<size_t> bucket_tails(bucket_heads.size());
    bucket_tails[bucket_tails.size() - 1] = bucket_tails.size() - 1;
    for(size_t i = 0; i < bucket_tails.size() - 1; ++i) {
        bucket_tails[i] = bucket_heads[i + 1] - 1;
    }

    //step 1 : first initialization
    if(type_of_sort == LMS_SUBSTRINGS_) {
            lmsInitForSort_(bucket_tails);
    } else {
            saInitForSort_(bucket_tails);
    }

    
    //step 2 : induced sort of lms prefixes
    // scan from head to end
    // assign from head to end each bucket
    for(size_t i = 0; i < suff_array_.size(); ++i) {
        if(suff_array_[i] == SAInducedSort::INF) continue;
        
        size_t checked_pos = suff_array_[i] - 1;
        if(type_[checked_pos] == L_TYPE_) {
            suff_array_[bucket_heads[str_[checked_pos]]] = checked_pos;
            ++bucket_heads[str_[checked_pos]];
        }
    }
    
    // scan from end to head
    // assign from end to head each bucket
    for(size_t i = suff_array_.size() - 1; i != SAInducedSort::INF; --i) {
        if(suff_array_[i] == SAInducedSort::INF) continue;
        
        size_t checked_pos = suff_array_[i] - 1;
        if(type_[checked_pos] == S_TYPE_) {
            suff_array_[bucket_tails[str_[checked_pos]]] = checked_pos;
            --bucket_tails[str_[checked_pos]];
        }
    }
}
void SAInducedSort::RecursiveSorter::SAsimpleSort_() {
    vector<size_t> sorted(alp_size_, SAInducedSort::INF);
    for(size_t i = 0; i < str_.size(); ++i) {
        sorted[str_[i]] = i;
    }
    
    size_t it = 0;
    for(size_t i = 0; i < alp_size_; ++i) {
        if(sorted[i] != -1) {
            suff_array_[it++] = sorted[i];
        }
    }
}

void SAInducedSort::RecursiveSorter::SAinducedSort() {
    suff_array_.clear();
    suff_array_.resize(str_.size(), SAInducedSort::INF);
    
    // all symbols are unique
    
    bool isAllSymbolsUnique = initType_();
    
    if(isAllSymbolsUnique) {
        SAsimpleSort_();
        return;
    }
    initLmsPointers_();
    initBuckets_();
    generalInducedSort_(LMS_SUBSTRINGS_);
    
    next_str_.clear();
    next_suff_array_.clear();
    next_str_.resize(lms_pointers_.size());
    next_suff_array_.resize(lms_pointers_.size());
    buildNewStr_();
    
    RecursiveSorter(next_str_,next_suff_array_,next_alp_size_).SAinducedSort();
    
    suff_array_.assign(suff_array_.size(), SAInducedSort::INF);
    generalInducedSort_(INDUCE_SA_FROM_SA1_);
}

SAInducedSort::RecursiveSorter::RecursiveSorter(const vector<size_t> & rcv_str,
                                                std::vector<size_t> & suff_array,
                                                size_t alp_sz):
alp_size_(alp_sz),
str_(rcv_str),
suff_array_(suff_array),
type_(rcv_str.size()),
is_lms_(rcv_str.size()),
buckets_(alp_sz){};

/********************************************************************/

vector<size_t> SAInducedSort::buildLCP(const string & str, const std::vector<size_t> & suff_array) {
    vector<size_t> lcp;
    size_t length = str.size();
    lcp.resize(length);

    vector<size_t> pos(length);
    for(size_t i = 0; i < length; ++i) {
        pos[suff_array[i]] = i;
    }
    
    size_t k = 0;
    for(size_t i = 0; i < length; ++i) {
        if(k > 0) --k;
        if(pos[i] == length - 1) {
            lcp[length - 1] = -1;
            k = 0;
        } else {
            size_t j = suff_array[pos[i] + 1];
            while(std::max(i + k, j + k) < length && str[i + k] == str[j + k]) {
                ++k;
            }
            
            lcp[pos[i]] = k;
        }
    }
    return lcp;
}

/********************************************************************/
const size_t SAInducedSort::INF;
vector<size_t> SAInducedSort::buildSA(const string & inputStr) {
    vector<size_t> suff_array;
    if(inputStr.size() == 0) return suff_array;
    size_t alp_size = 0;
    size_t shift = INF;
   
    for(size_t i = 0; i < inputStr.size(); ++i){
        if(inputStr[i] < shift) shift = inputStr[i];
        if(inputStr[i] > alp_size) alp_size = inputStr[i];
    }
    
    vector<size_t> str(inputStr.size() + 1);
    str[str.size()-1] = 0; // special symbol
    
    //do shift to economy memory
    for(size_t i = 0; i < inputStr.size(); ++i) {
        str[i] = inputStr[i] - shift + 1;
    }
    
    alp_size -= shift - 2;
    RecursiveSorter(str,suff_array,alp_size).SAinducedSort();
    return suff_array;
}


//
//  InducedSort.cpp
//  InducedSort
//
//  Created by Alex on 16.11.15.
//  Copyright © 2015 Alex. All rights reserved.
//
#include "InducedSort.hpp"

using namespace std;
typedef unsigned long long ui64;
typedef long long i64;

InducedSorter::InducedSorterImpl::InducedSorterImpl(const string & base_str_to_save):
base_str(base_str_to_save.size() + 1) {
    
    for(size_t i = 0; i < base_str_to_save.size(); ++i) {
        base_str[i] = base_str_to_save[i];
    }
    base_str[base_str_to_save.size()] = special_symb;
}

void InducedSorter::InducedSorterImpl::initLmsPointers(const vector<bool> & type, vector<size_t> & lms_pointers) {
    for(size_t i = 1; i < type.size() - 1; ++i) {
        if(type[i] == S_TYPE && type[i-1] == L_TYPE) {
            lms_pointers.push_back(i);
        }
    }
}

void InducedSorter::InducedSorterImpl::initType(const std::vector<size_t> & str, std::vector<bool> & type) {
    ui64 length = str.size();
    type[length - 1] = S_TYPE;
    for(size_t i = 2; i <= str.size(); ++i) {
        size_t ind = str.size() - i;
        if(str[ind] < str[ind+1]) {
            type[ind] = S_TYPE;
        } else {
            if(str[ind] > str[ind+1]) {
                type[ind] = L_TYPE;
            } else {
                type[ind] = type[ind+1];
            }
        }
    }
}

void InducedSorter::InducedSorterImpl::inducedSort(vector<size_t> & str, vector<size_t> & suff_array) {
    
    size_t length = str.size();
    suff_array.clear();
    suff_array.resize(length);
    
    vector<bool> type(length);
    vector<size_t> lms_pointers;
    
    initType(str, type);
    initLmsPointers(type, lms_pointers);
    
}

void InducedSorter::sort(const string & inputStr, vector<size_t> & ans_suff_array) {
    if(inputStr.size() == 0) return;
    
    vector<size_t> suff_array;
    InducedSorterImpl sorter(inputStr);
    sorter.inducedSort(sorter.base_str, suff_array);
    
    /* repair right answer */
}


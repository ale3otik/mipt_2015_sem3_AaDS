//
//  InducedSort.hpp
//  InducedSort
//
//  Created by Alex on 16.11.15.
//  Copyright © 2015 Alex. All rights reserved.
//

#ifndef InducedSort_hpp
#define InducedSort_hpp
#include <vector>
#include <string>

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

#endif /* InducedSort_hpp */

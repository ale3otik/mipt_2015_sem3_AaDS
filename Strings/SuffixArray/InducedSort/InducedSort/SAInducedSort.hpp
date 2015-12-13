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
    static const size_t INF = (size_t)(-1);
    
    static std::vector<size_t> buildSA(const std::string & str);
    static std::vector<size_t> buildLCP(const std::string & str,
                                        const std::vector<size_t> & suff_arr);
    
private:

    class RecursiveSorter {
    public:
        RecursiveSorter(const std::vector<size_t> & str,
                        std::vector<size_t> & suff_array,
                        const size_t alp_sz);
        void SAinducedSort();
    private:
        
        enum ETS_TYPE_OF_SORT_ {
            LMS_SUBSTRINGS_,
            INDUCE_SA_FROM_SA1_
        };

        enum ETS_LMS_SUBSTRINGS_ {
            L_TYPE_,
            S_TYPE_
        };
        
        const size_t alp_size_;

        const std::vector<size_t> & str_;
        std::vector<size_t> & suff_array_;

        std::vector<ETS_LMS_SUBSTRINGS_> type_; // L or S type
        std::vector<size_t> lms_pointers_; // positions of all first symb in LMS substrings
        std::vector<bool> is_lms_; //is s[i] is start of lms substring
        std::vector<size_t> buckets_; //index of head each i-sym bucket

        std::vector<size_t> back_map_alp_; // to iduce SA from SA
        // for next recursive step and induce
        size_t next_alp_size_;
        std::vector<size_t> next_str_;
        std::vector<size_t> next_suff_array_;
        
        bool isLmsSubstringEqual_(size_t first, size_t second);
        void buildNewStr_();
        bool initType_(); // return is all symbol are unique
        void initLmsPointers_();
        void initBuckets_();

        // need vector to copy to save previous version of bucket_tails
        void lmsInitForSort_(std::vector<size_t> bucket_tails);
        void saInitForSort_(std::vector<size_t> bucket_tails);
        void SAsimpleSort_();
        void generalInducedSort_(ETS_TYPE_OF_SORT_ action);
    };
};

#endif /* InducedSort_hpp */

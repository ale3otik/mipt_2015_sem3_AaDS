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

class SAInducedSorter{
public:
    static void sort(const std::string & str, std::vector<long long> & suff_array);
private:
    static const int ALP_SIZE = 26;
    static const bool L_TYPE = false;
    static const bool S_TYPE = true;
    static const char special_symb = 0;
    static bool isLmsSubstringEqual(const std::vector<long long> & str,
                                    const std::vector<bool> & type,
                                    const std::vector<bool> & is_lms,
                                    const long long fr,
                                    const long long sd);
    
    static void makeNewStr(std::vector<long long> & new_str,
                           long long & new_alp,
                           const std::vector<long long> & str,
                           const std::vector<bool> & is_lms,
                           const std::vector<bool> & type,
                           const std::vector<long long> & suff_array);
    
    static void SAinducedSort(std::vector<long long> & str, std::vector<long long> & suff_array, const long long alp_size);
    
    static void initType(const std::vector<long long> & str, std::vector<bool> & type);
    
    static void initLmsPointers(const std::vector<bool> & type,
                                std::vector<long long> & lms_pointers,
                                std::vector<bool> & is_lms);
    
    static void initBuckets(const std::vector<long long> & str, std::vector<long long> & buckets);

    static void LMSinducedSort(const std::vector<long long> & str,
                               const std::vector<long long> & lms_pointers,
                               std::vector<long long> & buckets,
                               std::vector<long long> & suff_array,
                               std::vector<bool> & type);
};

#endif /* InducedSort_hpp */

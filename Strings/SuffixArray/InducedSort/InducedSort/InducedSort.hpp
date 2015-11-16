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

class InducedSorter{
public:
    static void sort(const std::string & str, std::vector<size_t> & suff_array);
private:
    class InducedSorterImpl {
    public:
        std::vector<size_t> base_str;
        
        InducedSorterImpl(const std::string & base_str_to_save);
        void inducedSort(std::vector<size_t> & str, std::vector<size_t> & suff_array);
    
    private:
        static const int ALP_SIZE = 26;
        static const bool L_TYPE = false;
        static const bool S_TYPE = true;
        static const char special_symb = 0;
        
        void initType(const std::vector<size_t> & str, std::vector<bool> & type);
        void initLmsPointers(const std::vector<bool> & type, std::vector<size_t> & lms_pointers);
    };
};

#endif /* InducedSort_hpp */

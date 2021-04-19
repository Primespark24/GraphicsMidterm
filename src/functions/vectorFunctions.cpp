#include <vectorFunctions.h>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <iostream>

//calculate min and max and return as tuple of both
std::tuple<float, float> minMax(std::vector<float> vec){
    if(vec.size() != 0){
        float min = vec.at(0);
        float max = vec.at(0);
        
        for(int i = 1; i < vec.size(); i++){
            if(vec[i] > max)
                max = vec[i];
            if(vec[i] < min)
                min = vec[i];
        }
        
        return {min, max};
    }
    return {};
}


//return vector as string using ostream per-element and an iterator
std::string toString(std::vector<float> vec){
   
    if(vec.size() != 0){
        std::ostringstream oss;

        std::copy(vec.begin(), vec.end() - 1, std::ostream_iterator<float>(oss, ","));
        
        oss << vec.back();
        return oss.str();
    }
    return "";
}

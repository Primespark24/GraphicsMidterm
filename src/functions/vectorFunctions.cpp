#include <vectorFunctions.h>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <iostream>

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

std::string toString(std::vector<float> vec){
   
    if(vec.size() != 0){
        std::ostringstream oss;

        std::copy(vec.begin(), vec.end() - 1, std::ostream_iterator<float>(oss, ","));
        
        oss << vec.back();
        return oss.str();
    }
    return "";
}

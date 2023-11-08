#include "binaryread.hpp"
#include <iostream>

unsigned int ler_num(std::vector<bool> bnum){
    unsigned int num = 0;
    for(auto i = bnum.begin(); i != bnum.end(); i++){
        if(*i){
            num += pow(2, std::distance(bnum.begin(), i));
        }
    }
    return num;
}

std::vector<bool> conv_num(unsigned int num){
    std::vector<bool> bnum;
    float log = std::log2((float)num);
    unsigned int maxbits = std::ceil(log);
    if(log - (int)log == 0.0){
        maxbits++;
    }
    for(unsigned int i = 0; i < maxbits; i++){
        bool bit = (num >> i) & 1;
        bnum.push_back(bit);
    }
    return bnum;
}

std::vector<bool> read_char(char c){
    std::vector<bool> bitc;
    for(int j = 0; j < 8; j++){
         bool bit = (c >> j) & 1;
         bitc.push_back(bit);
    }
    return bitc;
}


std::pair<char, std::vector<bool>> read_bitc(std::vector<bool> bitc, std::vector<bool> carry){
    char c = 0;
    unsigned short int counter = 0;
    unsigned int last_carry = carry.size();
    unsigned int last_bitc = bitc.size(); 

    for(auto i = carry.begin(); i != carry.end(); i++){
        if(*i){
            c += pow(2, counter);
        }
        counter++;
        if(counter >= 8){
            last_carry = std::distance(carry.begin(), i) + 1;
            break;
        }
    }

    for(auto i = bitc.begin(); i != bitc.end(); i++){
        if(counter >= 8){
            last_bitc = std::distance(bitc.begin(), i);
            break;
        }
        if(*i){
            c += pow(2, counter);
        }
        counter++;
    }

    std::vector<bool> spares;
    for(unsigned int i = last_carry; i < carry.size(); i++){
        spares.push_back(carry[i]);
    }
    for(unsigned int i = last_bitc; i < bitc.size(); i++){
        spares.push_back(bitc[i]);
    }
    return std::pair(c, spares);
}

std::vector<std::vector<bool>> read_str(std::string s){
    std::vector<std::vector<bool>> bits;
    for(auto i = s.begin(); i != s.end(); i++){
        std::vector<bool> bitc = read_char(*i);
        bits.push_back(bitc);
    }
    return bits;
}


std::string read_bits(std::vector<std::vector<bool>> bits){
    std::string s;
    std::vector<bool> carry;
    for(auto i = bits.begin(); i != bits.end(); i++){
        auto p = read_bitc(*i, carry);
        s.push_back(p.first);
        carry = p.second;
    }
    std::vector<bool> none;
    while(carry.size() > 0){
        auto p = read_bitc(none, carry);
        s.push_back(p.first);
        carry = p.second;
    }
    return s;
}

std::vector<std::vector<bool>> constroi_matriz(std::string file, unsigned int maxsize){
    std::vector<std::vector<bool>> mat;
    std::vector<bool> elem;
    unsigned int count = 0;
    for(auto i = file.begin(); i != file.end(); i++){
        for(int j = 0; j < 8; j++){
            bool bit = (*i >> j) & 1;
            elem.push_back(bit);
            count++;
            if(count == maxsize){
                mat.push_back(elem);
                elem.clear();
                count = 0;
            }
        }
    }
    return mat;   
}
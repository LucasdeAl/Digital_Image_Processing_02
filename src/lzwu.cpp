#include "lzwu.hpp"
#include <iostream>
#include <iterator>
#include <new>
#include <vector>

LZW_cell::LZW_cell(unsigned int nkey, unsigned int val1, unsigned int val2){
        key = nkey;
        values = std::pair(val1, val2);
}

std::string LZW_write(std::string in){
    std::vector<LZW_cell> table;
    auto file = read_str(in);
    unsigned int last_key = 255;
    unsigned int current = ler_num(*(file.begin()));
    std::vector<std::vector<bool>> newfile;
    bool hasMatch = false;
    for(auto i = file.begin()+1; i != file.end(); i++){
        unsigned int prox = ler_num(*i);
        for(auto j = table.begin(); j != table.end(); j++){
            if(current == j->values.first and prox == j->values.second){
                hasMatch = true;
                current = j->key;
                break;
            }
        }
        if(!hasMatch){
            table.emplace_back(++last_key, current, prox);
            newfile.push_back(conv_num(current));
            current  = prox;
        } else{
            hasMatch = false;
        }
    }
    newfile.push_back(conv_num(current));
    for(auto i = table.begin(); i != table.end(); i++){
        std::cout << "key - " << i->key << "/ values - " << i->values.first << " | " << i->values.second << std::endl;
    }
    unsigned int maxsize = 0;
    std::vector<bool> placeholder;
    std::string out;
    for(auto i = newfile.begin(); i != newfile.end(); i++){
        if(i->size() > maxsize){
            maxsize = i->size();
        }
    }
    if(maxsize < 8){maxsize = 8;}
    auto maxrep = conv_num(maxsize);
    while(maxrep.size() < 8){
        maxrep.push_back(false);
    }
    auto po = read_bitc(maxrep, placeholder);
    out.push_back(po.first);
    std::cout << std::endl;
    for(auto i = newfile.begin(); i != newfile.end(); i++){
        while(i->size() < maxsize){
            i->push_back(false);
        }
    }
    for(auto i = newfile.begin(); i != newfile.end(); i++){
        for(auto j = i->rbegin(); j != i->rend(); j++){
            std::cout << *j << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    out += read_bits(newfile);
    return out;
}

void substitute(std::vector<unsigned int> &v, unsigned int index, LZW_cell c){
    unsigned int first = c.values.first;
    unsigned int last = c.values.second;
    v[index] = last; v.push_back(first);
    for(unsigned int i = v.size() - 1; i > index; i--){
        unsigned int aux = v[i];
        v[i] = v[i-1];
        v[i-1] = aux;
    } 
}

std::string LZW_read(std::string out){
    std::vector<bool> placeholder;
    std::vector<LZW_cell> table;
    unsigned int nkey = 255;
    const unsigned int maxsize = ler_num(read_char(out[0]));
    std::string outslice = out.substr(1, out.size() - 1);
    auto file = constroi_matriz(outslice, maxsize);
    for(auto i = file.begin(); i != file.end(); i++){
        for(auto j = i->rbegin(); j!= i->rend(); j++){
            std::cout << *j << " ";
        }
    }
    unsigned int last = 0;
    std::vector<unsigned int> queue;
    bool exists = false;
    std::string in;
    if(file.begin() != file.end()){last = ler_num(file[0]);} else{return in;}
    in.push_back(last);
    for(auto i = file.begin() + 1; i != file.end(); i++){
        unsigned int num = ler_num(*i);
        queue.push_back(num);

        for(unsigned int k = 0; k < table.size(); k++){
            if((num == table[k].values.second && last == table[k].values.first) || num > 255){
                exists = true;
            }
            if(num == table[k].key){
                unsigned int aux = table[k].values.first;
                while(aux >= 256){
                    aux = table[aux - 256].values.first;
                }
                table.emplace_back(++nkey, last, aux);
            }
        }

        if(table.size() > 0){
            if(num > table[table.size() - 1].key){
                unsigned int aux = last;
                while(aux >= 256){
                    aux = table[aux - 256].values.first;
                }
                table.emplace_back(++nkey, last, aux);
            }
        }

        for(unsigned int j = 0; j < queue.size(); j++){
            if(queue[j] > 255){
                for(auto k = table.begin(); k != table.end(); k++){
                    if(queue[j] == k->key){
                        substitute(queue, j, *k);
                        j = -1;
                        break;
                    }
                }
            }
        }
        for(auto j = queue.begin(); j != queue.end(); j++){
            in.push_back(*j);
        }
        queue.clear();
        if(!exists){
            table.emplace_back(++nkey, last, num);
        }
        else{
            exists = false;
        }
        last = num;
    }
    std::cout << std::endl;
    for(auto i = table.begin(); i != table.end(); i++){
        std::cout << "key - " << i->key << "/ values - " << i->values.first << " | " << i->values.second << std::endl;
    }
    return in;
}
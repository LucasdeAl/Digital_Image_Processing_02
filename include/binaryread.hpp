#include <vector>
#include <string>
#include <cmath>

unsigned int ler_num(std::vector<bool> bnum);

std::vector<bool> conv_num(unsigned int num);

std::vector<bool> read_char(char c);

std::pair<char, std::vector<bool>> read_bitc(std::vector<bool> bitc, std::vector<bool> carry);

std::vector<std::vector<bool>> read_str(std::string s);

std::string read_bits(std::vector<std::vector<bool>> bits);

std::vector<std::vector<bool>> constroi_matriz(std::string file, unsigned int maxsize);

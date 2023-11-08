#include <binaryread.hpp>

struct LZW_cell{
    unsigned int key;
    std::pair<unsigned int, unsigned int> values;
    LZW_cell(unsigned int nkey, unsigned int val1, unsigned int val2);
};

std::string LZW_write(std::string in);

std::string LZW_read(std::string out);
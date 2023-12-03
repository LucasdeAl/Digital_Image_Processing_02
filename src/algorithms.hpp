
#include <iostream>
#include <queue>
#include <map>
#include <vector>
#include <bitset>
#include <opencv2/opencv.hpp>
#include "algorithms.cpp"
//#include "algorithms.hpp"

using namespace std;
using namespace cv;

void generateHuffmanCodes(HuffmanNode* root, map<Pixel, string>& huffmanCodes, string code = "");
Mat decodeImage(HuffmanNode* root, const string& encodedData, const Size& image_size);
void deleteHuffmanTree(HuffmanNode* root);



class Pixel {
public:
    uchar blue;
    uchar green;
    uchar red;

    bool operator<(const Pixel& other) const {
        if (red != other.red) return red < other.red;
        if (green != other.green) return green < other.green;
        return blue < other.blue;
    }
};

class HuffmanNode {
public:
    Pixel pixel;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(const Pixel& pixel, int freq) : pixel(pixel), freq(freq), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq;
    }
};

class HuffmanCoding {
public:
    HuffmanNode* buildHuffmanTree(priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare>& pq) {
        while (pq.size() > 1) {
            HuffmanNode* left = pq.top();
            pq.pop();
            HuffmanNode* right = pq.top();
            pq.pop();
            HuffmanNode* parent = new HuffmanNode({0, 0, 0}, left->freq + right->freq);
            parent->left = left;
            parent->right = right;
            pq.push(parent);
        }
        return pq.top();
    }
};
#include <iostream>
#include <fstream>
#include <queue>
#include <map>
#include <vector>
#include <bitset>
#include <opencv2/opencv.hpp>
#include <quant.hpp>
    

using namespace std;
using namespace cv;

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

    void generateHuffmanCodes(HuffmanNode* root, map<Pixel, string>& huffmanCodes, string code = "") {
        if (root) {
            if (root->freq > 0) {
                huffmanCodes[root->pixel] = code;
            }
            generateHuffmanCodes(root->left, huffmanCodes, code + "0");
            generateHuffmanCodes(root->right, huffmanCodes, code + "1");
        }
    }

    string encodeImage(const Mat& image, const map<Pixel, string>& huffmanCodes) {
        string encodedData = "";
        for (int i = 0; i < image.rows; i++) {
            for (int j = 0; j < image.cols; j++) {
                Pixel pixel = image.at<Pixel>(i, j);
                encodedData += huffmanCodes.at(pixel);
            }
        }
        return encodedData;
    }

    Mat decodeImage(HuffmanNode* root, const string& encodedData, const Size& image_size) {
        Mat decodedImage(image_size, CV_8UC3);
        HuffmanNode* current = root;
        int dataIndex = 0;

        for (int i = 0; i < image_size.height; i++) {
            for (int j = 0; j < image_size.width; j++) {
                while (current->left || current->right) {
                    char bit = encodedData[dataIndex++];
                    if (bit == '0') {
                        current = current->left;
                    } else {
                        current = current->right;
                    }
                }
                decodedImage.at<Pixel>(i, j) = current->pixel;
                current = root;
            }
        }

        return decodedImage;
    }

    void deleteHuffmanTree(HuffmanNode* root) {
        if (root) {
            deleteHuffmanTree(root->left);
            deleteHuffmanTree(root->right);
            delete root;
        }
    }
};

int main() {
    /*Mat image = imread("../images/benchmark.bmp",IMREAD_COLOR);

    if (image.empty()) {
        cerr << "Error: Unable to load the image." << endl;
        return -1;
    }

    Size image_size = image.size();

    map<Pixel, int> freq;
    for (int i = 0; i < image_size.height; i++) {
        for (int j = 0; j < image_size.width; j++) {
            Pixel pixel = image.at<Pixel>(i, j);
            freq[pixel]++;
        }
    }

    priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> pq;
    for (auto pair : freq) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }

    HuffmanCoding huffman;
    HuffmanNode* huffmanTreeRoot = huffman.buildHuffmanTree(pq);

    map<Pixel, string> huffmanCodes;
    huffman.generateHuffmanCodes(huffmanTreeRoot, huffmanCodes);

    string encodedData = huffman.encodeImage(image, huffmanCodes);

    Mat decodedImage = huffman.decodeImage(huffmanTreeRoot, encodedData, image_size);

    imshow("Original Image", image);
    imshow("Decoded Image", decodedImage);
    waitKey(0);

    huffman.deleteHuffmanTree(huffmanTreeRoot);

    return 0;*/

    try{
        unsigned int range = 1;
        std:cout << "Insira um intervalo de cores entre 0 e 999 (Não incluindo esses números!)" << std::endl;
        std::cin >> range;
        Mat im = imread("../images/benchmark.bmp", IMREAD_COLOR);
        auto outs = QuantitizationCompress(im, range);
        std::ofstream out("../images/test.mirai");
        out << outs;
        out.close();
        std::ifstream in("../images/test.mirai");
        std::string ins;
        ins.assign( std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
        auto fin = QuantitizationDecompress(ins);
        imshow("Output", fin);
        waitKey();
    }
    catch(char const *e){
        std::cout << e << std::endl;
    }
}

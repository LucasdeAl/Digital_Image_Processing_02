#include <iostream>
#include <queue>
#include <map>
#include <vector>
#include <bitset>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

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

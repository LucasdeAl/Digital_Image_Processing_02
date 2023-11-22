#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

vector<vector<pair<int, int>> > encodeRunLength(const Mat& image) {
    vector<vector<pair<int, int>> > encodedData(3);

    for (int channel = 0; channel < 3; channel++) {
        int currentPixel = -1;
        vector<pair<int, int>> counts;
        int count = 0;

        for (int y = 0; y < image.rows; y++) {
            for (int x = 0; x < image.cols; x++) {
                int pixel = image.at<Vec3b>(y, x)[channel];
                if (pixel == currentPixel) {
                    count++;
                } else {
                    if (currentPixel != -1) {
                        counts.push_back({currentPixel, count});
                    }
                    currentPixel = pixel;
                    count = 1;
                }
            }
        }
        counts.push_back({currentPixel, count});
        encodedData[channel] = counts;
    }
    return encodedData;
}

Mat decodeRunLength(const vector<vector<pair<int, int>>>& encodedData, int width, int height) {
    Mat decodedImage(height, width, CV_8UC3, Scalar(0, 0, 0));

    for (int channel = 0; channel < 3; channel++) {
        int x = 0;
        int y = 0;

        for (pair<int, int> comp : encodedData[channel]) {
            int pixel = comp.first;
            for (int i = 0; i < comp.second; i++) {
                decodedImage.at<Vec3b>(y, x)[channel] = pixel;
                x++;
                if (x == width) {
                    x = 0;
                    y++;
                }
            }
        }
    }

    return decodedImage;
}



int main() {
    Mat originalImage = imread("../images/benchmark.bmp",IMREAD_COLOR);

    
    vector<vector<pair<int,int>>> encodedData = encodeRunLength(originalImage);

    
    Mat decodedImage = decodeRunLength(encodedData, originalImage.cols, originalImage.rows);
    
    size_t origin = originalImage.total() * originalImage.elemSize(); // tamanho da imagem

    size_t encodedBytes = 0;
    for (const auto& innerVector : encodedData) {
        encodedBytes += innerVector.size() * sizeof(std::pair<int, int>);
    }

    cout<<origin<<" "<<encodedBytes;

    


    // Exibe a imagem original e a imagem decodificada
    //imshow("Original Image", originalImage);
    //imshow("Decoded Image", decodedImage);
    waitKey(0);
    return 0;
}

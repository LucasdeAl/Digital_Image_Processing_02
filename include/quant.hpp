#include <opencv2/opencv.hpp>

enum cor{RED, GREEN, BLUE};

struct RGBcell{
    unsigned char c[3];

    RGBcell(unsigned char red, unsigned char green, unsigned char blue);

    bool operator==(RGBcell color);

    double dist();

    double operator <<(RGBcell o);
};


std::string read_bits_mod(std::vector<std::vector<bool>> bits);

std::vector<RGBcell> intercala(std::vector<RGBcell> a, std::vector<RGBcell> b, cor wider);

std::vector<RGBcell> CmergeSort(std::vector<RGBcell> v, cor wider);

cor avalia_intervalo(std::vector<RGBcell> mat);

std::vector<RGBcell> criaMatriz(cv::Mat img, std::pair<unsigned int, unsigned int> &size);

std::pair<std::vector<RGBcell>,std::vector<RGBcell>> ordenaMatriz(std::vector<RGBcell> mat, cor wider);

RGBcell media_pix(std::vector<RGBcell> s);

std::vector<RGBcell> constroiPaleta(std::vector<RGBcell> mat, unsigned int &qtd);

unsigned int compress_palette(std::vector<RGBcell> &p);

cv::Mat applyMatPalette(std::vector<RGBcell> palette, std::vector<RGBcell> image, std::pair<unsigned int, unsigned int> size);

std::vector<unsigned int> applyVecPalette(std::vector<RGBcell> palette, std::vector<RGBcell> image);

void threeDigitConv(unsigned int num, std::vector<std::vector<bool>> &m);

std::vector<std::vector<bool>> toBMat(std::vector<RGBcell> palette, std::vector<unsigned int> indexes, std::pair<unsigned int, unsigned int> size);

cv::Mat QuantitizeImage(cv::Mat image, unsigned int range);

std::string QuantitizationCompress(cv::Mat image, unsigned int range);

unsigned int converteBool(std::vector<bool> most, std::vector<bool> middle, std::vector<bool> least);

cv::Mat QuantitizationDecompress(std::string in);
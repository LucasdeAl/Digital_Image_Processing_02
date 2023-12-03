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


int main() {
    

    try{
        unsigned int range = 1;
        std::cout << "Insira um intervalo de cores entre 0 e 999 (Não incluindo esses números!)" << std::endl;
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
        imwrite("../images/benchmarkModificado.bmp",fin);
        imshow("Output", fin);
        waitKey();
    }
    catch(char const *e){
        std::cout << e << std::endl;
    }
}

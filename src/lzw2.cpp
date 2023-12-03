
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <opencv2/opencv.hpp>
#include <quant.hpp>


template <typename Iterator>
Iterator compress(const std::string &uncompressed, Iterator result) {
  // Build the dictionary.
  int dictSize = 256;
  std::map<std::string,int> dictionary;
  for (int i = 0; i < 256; i++)
    dictionary[std::string(1, i)] = i;
  
  std::string w;
  for (std::string::const_iterator it = uncompressed.begin();
       it != uncompressed.end(); ++it) {
    char c = *it;
    std::string wc = w + c;
    if (dictionary.count(wc))
      w = wc;
    else {
      *result++ = dictionary[w];
      // Add wc to the dictionary.
      dictionary[wc] = dictSize++;
      w = std::string(1, c);
    }
  }
  
  // Output the code for w.
  if (!w.empty())
    *result++ = dictionary[w];
  return result;
}


template <typename Iterator>
std::string decompress(Iterator begin, Iterator end) {
  // Build the dictionary.
  int dictSize = 256;
  std::map<int,std::string> dictionary;
  for (int i = 0; i < 256; i++)
    dictionary[i] = std::string(1, i);
  
  std::string w(1, *begin++);
  std::string result = w;
  std::string entry;
  for ( ; begin != end; begin++) {
    int k = *begin;
    if (dictionary.count(k))
      entry = dictionary[k];
    else if (k == dictSize)
      entry = w + w[0];
    else
      throw "Bad compressed k";
    
    result += entry;
    
    // Add w+entry[0] to the dictionary.
    dictionary[dictSize++] = w + entry[0];
    
    w = entry;
  }
  return result;
}


std::vector<int> decodeBinaryFile(const char* filename) {
    std::vector<int> result;

    // Abrir o arquivo binário para leitura em modo binário
    std::ifstream inputFile(filename, std::ios::binary);

    // Verificar se o arquivo foi aberto corretamente
    if (!inputFile.is_open()) {
        std::cerr << "Erro ao abrir o arquivo para leitura." << std::endl;
        return result; // Retorna um vetor vazio em caso de erro
    }

    // Ler o tamanho do vetor
    size_t vectorSize;
    inputFile.read(reinterpret_cast<char*>(&vectorSize), sizeof(size_t));

    // Ler os dados do vetor
    result.resize(vectorSize);
    inputFile.read(reinterpret_cast<char*>(result.data()), vectorSize * sizeof(int));
    inputFile.close();

    return result;
}

using namespace cv;


int main() {
    

    try{
        std::vector<int> compressed;
        unsigned int range = 1;
        std::cout << "Insira um intervalo de cores entre 0 e 999 (Não incluindo esses números!)" << std::endl;
        std::cin >> range;
        Mat im = imread("../images/benchmark.bmp", IMREAD_COLOR);
        //primeira compressão mirai
        auto outs = QuantitizationCompress(im, range);
        //lzw
        compress(outs,std::back_inserter(compressed));
        //salva arquivo
        std::ofstream out("../images/test.mils", std::ios::binary);
        size_t vectorSize = compressed.size();
        out.write(reinterpret_cast<const char*>(&vectorSize), sizeof(size_t));
        out.write(reinterpret_cast<const char*>(compressed.data()), compressed.size() * sizeof(int));
        out.close();


        // abre arquivo e descomprime lzw
        compressed = decodeBinaryFile("../images/test.mils");
        std::string decompressed = decompress(compressed.begin(), compressed.end());
        //descomprime mirai
        auto fin = QuantitizationDecompress(decompressed);
        imwrite("../images/benchmarkModificado.bmp",fin);
        imshow("Output", fin);
        waitKey();


        //std::ofstream out("../images/test.mirai");
        //out << outs;
        //out.close();
        //std::ifstream in("../images/test.mirai");
        //std::string ins;
        //ins.assign( std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
        //auto fin = QuantitizationDecompress(ins);
        //imwrite("../images/benchmarkModificado.bmp",fin);
        //imshow("Output", fin);
        //waitKey();
    }
    catch(char const *e){
        std::cout << e << std::endl;
    }
}

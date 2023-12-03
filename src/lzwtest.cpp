#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <opencv2/opencv.hpp>


template <typename Iterator>
Iterator compress(cv::Mat& image, Iterator result) {
  int dictSize = 256;
  std::map<std::string,int> dictionary;
  for (int i = 0; i < 256; i++)
    dictionary[std::string(1, (char)i)] = i;
  
  uint8_t* pixelImagePtr;
  pixelImagePtr = (uint8_t*)image.data;
  int cn = image.channels();

  std::string w;
   for(int k = 0 ; k < cn; k++)
    {
        for(int j = 0; j < image.cols; j++)
        {
            for(int i = 0; i < image.rows; i++)
            {
                uint8_t c = pixelImagePtr[i*image.cols*cn + j*cn + k];
                std::string wc = w + (char)c;
                if (dictionary.count(wc))
                  w = wc;
                else {
                  *result++ = dictionary[w];
                  // adiciona wc ao dicionario
                  dictionary[wc] = dictSize++;
                  w = std::string(1, (char)c);
                }
            }
        }
    }
  

  if (!w.empty())
    *result++ = dictionary[w];
  return result;
}


template <typename Iterator>
cv::Mat decompress(Iterator begin, Iterator end,int rows, int cols) {

  int dictSize = 256;
  std::map<int,std::string> dictionary;
  for (int i = 0; i < 256; i++)
    dictionary[i] = std::string(1, (char)i);
  
  std::string w(1, *begin++);
  cv::Mat decompressed(rows,cols,CV_8UC3,cv::Scalar(0,0,0));
  int cn = decompressed.channels();
  uint8_t* result = decompressed.data;
  std::string entry;
  int i=0;
  int j=0;
  int l=0;
  for ( ; begin != end; begin++) {  
    int k = *begin;
    if (dictionary.count(k))
      entry = dictionary[k];
    else if (k == dictSize)
      entry = w + w[0];
    else
      throw "má compressão";
   int c = 0; 
   for (; l < cn; ++l) {
            for (; j < cols; ++j) {
                for (; i < rows; ++i) {
                    if (c < entry.size()) {
                        result[i * cols * cn + j * cn + l] = static_cast<uint8_t>(entry[c]);
                        ++c;
                        //std::cout << "entrou" << std::endl;
                    }
                    else goto add;
                }
                i = 0; 
            }
            j = 0; 
        }
    
    add:
    // Adiciona w+entry[0] ao dicionario.
    dictionary[dictSize++] = w + entry[0];
    
    w = entry;
  }
  return decompressed;
}


std::string vectorToString(const std::vector<int>& vec) {
    std::ostringstream oss;
    
    // Utiliza um iterador para percorrer o vector e inserir os inteiros no fluxo de saída
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        oss << *it;
        // Adiciona um espaço entre os inteiros (pode ser ajustado conforme necessário)
        if (std::next(it) != vec.end()) {
            oss << ' ';
        }
    }
    
    // Retorna a string resultante
    return oss.str();
}

// Função que converte uma string para um vector de inteiros
std::vector<int> stringToVector(const std::string& str) {
    std::vector<int> result;
    std::istringstream iss(str);
    
    // Utiliza um fluxo de entrada para ler os inteiros da string
    int value;
    while (iss >> value) {
        // Adiciona cada inteiro ao vector
        result.push_back(value);
    }
    
    // Retorna o vector resultante
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

    // Fechar o arquivo
    inputFile.close();

    return result;
}

int main() {
  std::vector<int> compressed;
  cv::Mat image = cv::imread("../images/benchmarkModificado.bmp",cv::IMREAD_UNCHANGED);
  compress(image, std::back_inserter(compressed));

  std::ofstream out("../images/test.mils", std::ios::binary);
  // Escrever o tamanho do vetor no arquivo
  size_t vectorSize = compressed.size();
  out.write(reinterpret_cast<const char*>(&vectorSize), sizeof(size_t));

  // Escrever os dados do vetor no arquivo
  out.write(reinterpret_cast<const char*>(compressed.data()), compressed.size() * sizeof(int));

  // Fechar o arquivo
  out.close();
  
  compressed = decodeBinaryFile("../images/test.mils");
  //std::cout << "tamanho comprimido do binário: "<<in<<std::endl;
  std::cout << "tamanho comprimido no vector: "<<compressed.size()*sizeof(int)<<std::endl;
  size_t origin = image.total() * image.elemSize();
  std::cout<<"tamanho original: "<< origin<<std::endl;
  cv::Mat decompressed = decompress(compressed.begin(),compressed.end(),image.rows,image.cols);
  cv::imshow("descomprimida",decompressed);
  cv::waitKey();
  //std::string decompressed = decompress(compressed.begin(), compressed.end());
  //std::cout << decompressed << std::endl;
  
  return 0;
}
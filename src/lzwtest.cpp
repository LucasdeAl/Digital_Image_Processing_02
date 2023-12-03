#include <string>
#include <map>
#include <iostream>
#include <iterator>
#include <vector>
#include <opencv2/opencv.hpp>

// Compress a string to a list of output symbols.
// The result will be written to the output iterator
// starting at "result"; the final iterator is returned.
template <typename Iterator>
Iterator compress(cv::Mat& image, Iterator result) {
  // Build the dictionary.
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
                  // Add wc to the dictionary.
                  dictionary[wc] = dictSize++;
                  w = std::string(1, (char)c);
                }
            }
        }
    }
  
  // Output the code for w.
  if (!w.empty())
    *result++ = dictionary[w];
  return result;
}

// Decompress a list of output ks to a string.
// "begin" and "end" must form a valid range of ints
template <typename Iterator>
cv::Mat decompress(Iterator begin, Iterator end,int rows, int cols) {
  // Build the dictionary.
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
      throw "Bad compressed k";
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
    // Add w+entry[0] to the dictionary.
    dictionary[dictSize++] = w + entry[0];
    
    w = entry;
  }
  return decompressed;
}



int main() {
  std::vector<int> compressed;
  cv::Mat image = cv::imread("../images/benchmarkModificado.bmp",cv::IMREAD_UNCHANGED);
  compress(image, std::back_inserter(compressed));
  //copy(compressed.begin(), compressed.end(), std::ostream_iterator<int>(std::cout, ", "));
  std::cout << "tamanho comprimido: "<<compressed.size()<<std::endl;
  size_t origin = image.total() * image.elemSize();
  std::cout<<"tamanho original: "<< origin<<std::endl;
  cv::Mat decompressed = decompress(compressed.begin(),compressed.end(),image.rows,image.cols);
  cv::imshow("descomprimida",decompressed);
  cv::waitKey();
  //std::string decompressed = decompress(compressed.begin(), compressed.end());
  //std::cout << decompressed << std::endl;
  
  return 0;
}
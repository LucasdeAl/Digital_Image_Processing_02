#include <quant.hpp>
#include <binaryread.hpp>
#include <ostream>
#include <cmath>
#include <fstream>
using namespace cv;

RGBcell::RGBcell(unsigned char red, unsigned char green, unsigned char blue){
    c[0] = red;
    c[1] = green;
    c[2] = blue;
}

bool RGBcell::operator==(RGBcell color){
    return (c[0] == color.c[0] && c[1] == color.c[1] && c[2] == color.c[2]);
}

double RGBcell::dist(){
    return sqrt(c[0]*c[0] + c[1]*c[1] + c[2]*c[2]);
}

double RGBcell::operator <<(RGBcell o){
    auto r = abs(c[0] - o.c[0]);
    auto g = abs(c[1] - o.c[1]);
    auto b = abs(c[2] - o.c[2]);
    return sqrt(r*r + g*g + b*b);
} 

std::string read_bits_mod(std::vector<std::vector<bool>> bits){
    std::string s;
    std::vector<bool> carry;
    for(auto i = bits.begin()+1; i != bits.end(); i+=2){
        carry = *(i-1);
        auto p = read_bitc(*i, carry);
        s.push_back(p.first);
        carry = p.second;
    }
    std::vector<bool> none;
    while(carry.size() > 0){
        auto p = read_bitc(none, carry);
        s.push_back(p.first);
        carry = p.second;
    }
    return s;
}

std::vector<RGBcell> intercala(std::vector<RGBcell> a, std::vector<RGBcell> b, cor wider){
    std::vector<RGBcell> out;
    auto aptr = a.begin();
    auto bptr = b.begin();
    while(aptr != a.end() || bptr != b.end()){
        if(aptr == a.end()){
            out.push_back(*(bptr++));
        } else if(bptr == b.end()){
            out.push_back(*(aptr++));
        } else{
            if(aptr->c[wider] <= bptr->c[wider]){
                out.push_back(*(aptr++));
            } else{
                out.push_back(*(bptr++));
            }
        }
    }
    return out;
}

std::vector<RGBcell> CmergeSort(std::vector<RGBcell> v, cor wider){
    if(v.size() <= 1){
        return v;
    }
    else{
        auto v1 = CmergeSort(std::vector<RGBcell>(v.begin(), v.begin() + v.size()/2), wider);
        auto v2 = CmergeSort(std::vector<RGBcell>(v.begin() + v.size()/2 + 1, v.end()), wider);
        return intercala(v1, v2, wider);
    }
}

cor avalia_intervalo(std::vector<RGBcell> mat){
    unsigned char maxr = 0, maxg = 0, maxb = 0;
    unsigned char minr = 255, ming = 255, minb = 255;
    for(auto j = mat.begin(); j != mat.end(); j++){
        if(j->c[0] > maxr){
            maxr = j->c[0];
        }
        if(j->c[0] < minr){
            minr = j->c[0];
        }
        if(j->c[1] > maxg){
            maxg = j->c[1];
        }
        if(j->c[1] < ming){
            ming = j->c[1];
        }
        if(j->c[2] > maxb){
            maxb = j->c[2];
        }
        if(j->c[2] < minb){
            minb = j->c[2];
        }
    }
    int intr = maxr - minr;
    int intg = maxg - ming;
    int intb = maxb - minb;
    if(intr >= intg && intr >= intb){
        return RED;
    }
    else if(intg >= intr && intg >= intb){
        return GREEN;
    }
    else{
        return BLUE;
    }
}

std::vector<RGBcell> criaMatriz(Mat img, std::pair<unsigned int, unsigned int> &size){
    int rows = img.rows, cols = img.cols;
    unsigned char* p = (unsigned char*)img.data;
    std::vector<RGBcell> row;

    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            unsigned char b = p[i*cols*3 + j*3];
            unsigned char g = p[i*cols*3 + j*3 + 1];
            unsigned char r = p[i*cols*3 + j*3 + 2];
            row.emplace_back(r, g, b);
        }
    }

    size.first = rows; size.second = cols;
    return row;
}

std::pair<std::vector<RGBcell>,std::vector<RGBcell>> ordenaMatriz(std::vector<RGBcell> mat, cor wider){
    auto vetor = CmergeSort(mat, wider);
    auto frac1 = std::vector<RGBcell>(vetor.begin(), vetor.begin() + vetor.size() * 0.5);
    auto frac2 = std::vector<RGBcell>(vetor.begin() + vetor.size() * 0.5, vetor.end());
    return std::pair(frac1, frac2);
}

RGBcell media_pix(std::vector<RGBcell> s){
    unsigned long int somar = 0;
    unsigned long int somag = 0;
    unsigned long int somab = 0;
    for(auto i = s.begin(); i != s.end(); i++){
        somar += i->c[0];
        somag += i->c[1];
        somab += i->c[2]; 
    }
    return RGBcell(somar/s.size(), somag/s.size(), somab/s.size());
}

std::vector<RGBcell> constroiPaleta(std::vector<RGBcell> mat, unsigned int &qtd){
    std::vector<std::vector<RGBcell>> list;
    std::vector<RGBcell> paleta;
    list.push_back(mat);
    for(unsigned int i = 0; i < qtd; i++){
        auto aux = ordenaMatriz(list[i], avalia_intervalo(list[i]));
        list.push_back(aux.first);
        list.push_back(aux.second);
    }
    for(unsigned int i = 0; i < qtd; i++){
        paleta.push_back(media_pix(list[i]));
    }
    return paleta;
}

unsigned int compress_palette(std::vector<RGBcell> &p){
    std::vector<RGBcell> aux;
    unsigned int qtd = 0;
    for(auto i = p.begin(); i != p.end(); i++){
        bool isIn = false;
        for(auto j = aux.begin(); j != aux.end(); j++){
            if(*j == *i){
                isIn = true;
                break;
            }
        }
        if(isIn){
            auto t = *i;
            *i = p[p.size() - (++qtd)];
            p[p.size() - qtd] = t;
        } else{
            aux.push_back(*i);
        }
    }

    for(auto i = 0; i < qtd; i++){
        p.pop_back();
    }

    return qtd;
}

cv::Mat applyMatPalette(std::vector<RGBcell> palette, std::vector<RGBcell> image, std::pair<unsigned int, unsigned int> size){
    cv::Mat out(size.first, size.second, CV_8UC3);
    auto iptr = (unsigned char*)out.data;
    for(auto i = image.begin(); i != image.end(); i++){
        double min = 4096;
        unsigned int index = 0;
        for(auto j = palette.begin(); j != palette.end(); j++){
            double dist = *i << *j;
            if(dist < min){
                min = dist;
                index = std::distance(palette.begin(), j);
            }
        }
        *i = palette[index];

        *iptr = i->c[2];
        iptr++;
        *iptr = i->c[1];
        iptr++;
        *iptr = i->c[0];
        iptr++;
    }
    return out;
}

std::vector<unsigned int> applyVecPalette(std::vector<RGBcell> palette, std::vector<RGBcell> image){
    std::vector<unsigned int> indexes;
    for(auto i = image.begin(); i != image.end(); i++){
        double min = 4096;
        unsigned int index = 0;
        for(auto j = palette.begin(); j != palette.end(); j++){
            double dist = *i << *j;
            if(dist < min){
                min = dist;
                index = std::distance(palette.begin(), j);
            }
        }
        indexes.push_back(index);
    }
    return indexes;
}


void threeDigitConv(unsigned int num, std::vector<std::vector<bool>> &m){
    unsigned int soma = 0;
    for(int i = 2; i >= 0; i--){
        unsigned char digit = (num/pow(10, i))- soma;
        switch(digit){
            case 0:
                m.push_back(std::vector<bool>{0, 0, 0, 0});
                break;
            case 1:
                m.push_back(std::vector<bool>{1, 0, 0, 0});
                break;
            case 2:
                m.push_back(std::vector<bool>{0, 1, 0, 0});
                break;
            case 3:
                m.push_back(std::vector<bool>{1, 1, 0, 0});
                break;
            case 4:
                m.push_back(std::vector<bool>{0, 0, 1, 0});
                break;
            case 5:
                m.push_back(std::vector<bool>{1, 0, 1, 0});
                break;
            case 6:
                m.push_back(std::vector<bool>{0, 1, 1, 0});
                break;
            case 7:
                m.push_back(std::vector<bool>{1, 1, 1, 0});
                break;
            case 8:
                m.push_back(std::vector<bool>{0, 0, 0, 1});
                break;
            case 9:
                m.push_back(std::vector<bool>{1, 0, 0, 1});
        }
        soma += digit;
        soma *= 10;
    }
    if(soma != num*10){
        throw "Algo de errado!";
    }
}

void fourDigitConv(unsigned int num, std::vector<std::vector<bool>> &m){
    unsigned int soma = 0;
    for(int i = 3; i >= 0; i--){
        unsigned char digit = (num/pow(10, i))- soma;
        switch(digit){
            case 0:
                m.push_back(std::vector<bool>{0, 0, 0, 0});
                break;
            case 1:
                m.push_back(std::vector<bool>{1, 0, 0, 0});
                break;
            case 2:
                m.push_back(std::vector<bool>{0, 1, 0, 0});
                break;
            case 3:
                m.push_back(std::vector<bool>{1, 1, 0, 0});
                break;
            case 4:
                m.push_back(std::vector<bool>{0, 0, 1, 0});
                break;
            case 5:
                m.push_back(std::vector<bool>{1, 0, 1, 0});
                break;
            case 6:
                m.push_back(std::vector<bool>{0, 1, 1, 0});
                break;
            case 7:
                m.push_back(std::vector<bool>{1, 1, 1, 0});
                break;
            case 8:
                m.push_back(std::vector<bool>{0, 0, 0, 1});
                break;
            case 9:
                m.push_back(std::vector<bool>{1, 0, 0, 1});
        }
        soma += digit;
        soma *= 10;
    }
    if(soma != num*10){
        throw "Algo de errado!";
    }
}

std::vector<std::vector<bool>> toBMat(std::vector<RGBcell> palette, std::vector<unsigned int> indexes, std::pair<unsigned int, unsigned int> size){
    std::vector<std::vector<bool>> out;
    fourDigitConv(size.first, out);
    fourDigitConv(size.second, out);
    for(auto i = palette.begin(); i != palette.end(); i++){
        threeDigitConv(i->c[0], out);
        threeDigitConv(i->c[1], out);
        threeDigitConv(i->c[2], out);
    }
    out.push_back(std::vector<bool>{true, true, true, true});
    for(auto i = indexes.begin(); i != indexes.end(); i++){
        threeDigitConv(*i, out);
    }
    return out;
}

Mat QuantitizeImage(Mat image, unsigned int range){
    if(range >= 999){
        throw "Número de cores inviável!";
    }
    std::pair<unsigned int, unsigned int> dim;
    auto newmat = criaMatriz(image, dim);
    auto pal = constroiPaleta(newmat, range);
    range -= compress_palette(pal);
    Mat output = applyMatPalette(pal, newmat, dim);
    return output;
}

std::string QuantitizationCompress(Mat image, unsigned int range){
    if(range >= 999){
        throw "Número de cores inviável!";
    }
    std::pair<unsigned int, unsigned int> dim;
    auto newmat = criaMatriz(image, dim);
    auto pal = constroiPaleta(newmat, range);
    range -= compress_palette(pal);
    auto trans = applyVecPalette(pal, newmat);
    auto bits = toBMat(pal, trans, dim);
    auto out = read_bits_mod(bits);
    return out;
}

unsigned int converteBool(std::vector<bool> most, std::vector<bool> middle, std::vector<bool> least){
    auto nmost = ler_num(most);
    auto nmiddle = ler_num(middle);
    auto nleast = ler_num(least);
    return (nmost * 100) + (nmiddle * 10) + (nleast);
}

unsigned int converteBool4(std::vector<bool> most, std::vector<bool> middle, std::vector<bool> least, std::vector<bool> garbage){
    auto nmost = ler_num(most);
    auto nmiddle = ler_num(middle);
    auto nleast = ler_num(least);
    auto ngarbage = ler_num(garbage);
    return (nmost * 1000) + (nmiddle * 100) + (nleast * 10) + (ngarbage);
}


Mat QuantitizationDecompress(std::string in){
    auto mat = constroi_matriz(in, 4);
    unsigned int imInit;
    std::vector<RGBcell> palette;
    auto size = std::pair<unsigned int, unsigned int>(converteBool4(mat[0], mat[1], mat[2], mat[3]), converteBool4(mat[4], mat[5], mat[6], mat[7]));
    for(unsigned int i = 8; i < mat.size(); i+=9){
        if(ler_num(mat[i]) == 15){
            imInit = i+1;
            break;
        }else{
            auto cr = converteBool(mat[i], mat[i+1], mat[i+2]);
            auto cg = converteBool(mat[i+3], mat[i+4], mat[i+5]);
            auto cb = converteBool(mat[i+6], mat[i+7], mat[i+8]);
            palette.emplace_back(cr, cg, cb);
        }
    }
    std::vector<unsigned int> indexes;
    for(int i = imInit+2; i < mat.size(); i+=3){
        auto index = converteBool(mat[i-2], mat[i-1], mat[i]);
        indexes.push_back(index);
    }
    cv::Mat out(size.first, size.second, CV_8UC3);
    auto iptr = (unsigned char*)out.data;
    for(auto i = indexes.begin(); i != indexes.end(); i++){
        auto c = palette[*i];

        *iptr = c.c[2];
        iptr++;
        *iptr = c.c[1];
        iptr++;
        *iptr = c.c[0];
        iptr++;
    }
    return out;
}
//
// Created by shirogiro on 14.06.19.
//

#include <cstring>
#include "huffman.h"

int main(int argc, char const* argv[]) {
    if(argc != 4) {
        std::cout << "Usage: encode/decode <input filename> <output filename>" << std::endl;
        return 0;
    }
    if(strcmp(argv[1], "encode") == 0) {
        std::ifstream ifc;
        ifc.open(argv[2]);
        if(!ifc) {
            std::cout << "Input file is corrupted" << std::endl;
            return 0;
        }
        huffman::encode(std::ifstream(argv[2]), argv[3]);
        std::cout << "Encoding completed!" << std::endl;
    } else if(strcmp(argv[1], "decode") == 0) {
        std::ifstream ifc;
        ifc.open(argv[2]);
        if(!ifc) {
            std::cout << "Input file is corrupted" << std::endl;
            return 0;
        }
        huffman::decode(std::ifstream(argv[2]), argv[3]);
        std::cout << "Decoding completed!" << std::endl;
    } else {
        std::cout << "Usage: encode/decode <input filename> <output filename>" << std::endl;
        return 0;
    }
    return 0;
}
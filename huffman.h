//
// Created by shirogiro on 14.06.19.
//


#ifndef HUFFMAN_HUFFMAN_H
#define HUFFMAN_HUFFMAN_H

#include <iostream>
#include <map>
#include <queue>
#include <memory>
#include <algorithm>
#include <fstream>

namespace huffman {
    void encode(std::ifstream ifs, const std::string &outFileName = "out.txt");

    void decode(std::ifstream encoded, const std::string &outFileName = "out.txt");
}

#endif //HUFFMAN_HUFFMAN_H

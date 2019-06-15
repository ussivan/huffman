//
// Created by shirogiro on 14.06.19.
//

#include "huffman.h"

void test(std::string s) {
//    std::cout << "Testing " << s << std::endl;
    std::ofstream ofs("in.txt", std::ofstream::out);
    ofs << s << std::endl;
    ofs.close();
    huffman::encode(std::ifstream("in.txt", std::ifstream::in), "out.txt");
    huffman::decode(std::ifstream("out.txt", std::ifstream::in), "output.txt");
    std::ifstream ifs("output.txt", std::ifstream::in);
    char c = static_cast<char>(ifs.get());
    int i = 0;
    while(ifs.good()) {
        if(c != s[i] && s[i] != '\0') {
            std::cout << "Failed on test " << s << std::endl;
            std::exit(0);
        }
        i++;
        c = static_cast<char>(ifs.get());
    }
//    std::cout << "Passed" << std::endl;
}

void randomTest(uint32_t size, uint32_t testCount) {
    for(size_t i = 0; i < testCount; i++) {
        std::string s;
        for(size_t j = 0; j < size; j++) {
            s += (char)std::rand() % 256;
        }
        test(s);
    }
    std::cout << "Passed random test " << size << std::endl;
}

int main(){
    test("a");
    test("");
    test("aaaaaaaaaaaaaavvvvvvvvvvvvvvvvvvavavavavaaaaaaaa00");
    test("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    test("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaabcaaaaaaaaaaaaaaaaaaaaaaaa");
    test("qwertyuiop[]asdfghjkl;'zxcvbnm,./1234567890!@#$%^&*()-+-=");
    randomTest(10, 1000);
    randomTest(100, 1000);
    randomTest(1000, 100);
    randomTest(100000, 100);
    randomTest(1000000, 10);
    randomTest(10000000, 1);
//    randomTest(100000000, 1);
    return 0;
}
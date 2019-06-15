#include <iostream>
#include <map>
#include <queue>
#include <memory>
#include <algorithm>
#include <fstream>

#include "huffman.h"

namespace huffman {

    struct node {
        std::unique_ptr<node> left, right;
        unsigned char val;
        uint32_t priority;

        explicit node(unsigned char val = 0, uint32_t priority = 0) : val(val), priority(priority) {
            left = nullptr;
            right = nullptr;
        };

    };

    std::string toCharString(uint64_t num, uint32_t length) {
        length = (length + BYTE_LENGTH - 1) / BYTE_LENGTH;
        std::string ans;
        for (size_t i = 0; i < length; i++) {
            ans.push_back((unsigned char) (num % (1 << BYTE_LENGTH)));
            num >>= BYTE_LENGTH;
        }
        for (size_t i = 0; i < length / 2; i++) {
            std::swap(ans[i], ans[length - i - 1]);
        }
        return ans;
    }

    uint32_t charSeqToUI(std::string s, uint32_t length) {
        uint32_t ans = 0;
        for (size_t i = 0; i < length; i++) {
            ans <<= BYTE_LENGTH;
            ans += (unsigned char) s[i];
        }
        return ans;
    }

    using ptr_t = std::unique_ptr<node>;

    void dfs(const ptr_t &v, std::pair<uint32_t, uint32_t> code, // first - code, second - size
             std::vector<std::pair<uint32_t, uint32_t>> &codes) {
        if (v->left == nullptr && v->right == nullptr) {
            codes[v->val] = code;
            return;
        }
        dfs(v->left, std::pair<uint32_t, uint32_t>(code.first << 1, code.second + 1), codes);
        dfs(v->right, std::pair<uint32_t, uint32_t>((code.first << 1) + 1, code.second + 1), codes);
    }

    ptr_t make_tree(uint32_t *sym_count) {
        auto cmp = [](ptr_t &a, ptr_t &b) {
            return a->priority > b->priority;
        };
        std::vector<ptr_t> queue;
        queue.reserve(ALPHABET_SIZE);
        for (size_t c = 0; c < ALPHABET_SIZE; c++) {
            queue.push_back(std::make_unique<node>(c, sym_count[c]));
        }
        std::make_heap(queue.begin(), queue.end(), cmp);
        while (queue.size() > 1) {
            std::pop_heap(queue.begin(), queue.end(), cmp);
            auto a = std::move(queue.back());
            queue.pop_back();
            std::pop_heap(queue.begin(), queue.end(), cmp);
            auto b = std::move(queue.back());
            queue.pop_back();
            auto newNode = std::make_unique<node>(0, a->priority + b->priority);
            newNode->left = std::move(a);
            newNode->right = std::move(b);
            queue.push_back(std::move(newNode));
            std::push_heap(queue.begin(), queue.end(), cmp);
        }
        auto root = std::move(queue.back());
        queue.pop_back();
        return root;
    }

    void encode(std::ifstream ifs, const std::string &outFileName) {
        // first - code, second - size
        std::vector<std::pair<uint32_t, uint32_t>> codes(ALPHABET_SIZE);
        std::ofstream ofs(outFileName);
        uint32_t sym_count[ALPHABET_SIZE];
        for (uint32_t &i : sym_count) {
            i = 0;
        }
        auto c = static_cast<unsigned char>(ifs.get());
        while (ifs.good()) {
            sym_count[c]++;
            c = static_cast<unsigned char>(ifs.get());
        }
        ptr_t root = make_tree(sym_count);
        dfs(root, std::pair<uint32_t, uint32_t>(0, 0), codes);
        uint32_t lastCount = 0;
        for (size_t i = 0; i < ALPHABET_SIZE; i++) {
            ofs << toCharString(sym_count[i], 4*BYTE_LENGTH);
            lastCount += (sym_count[i] * codes[i].second) % BYTE_LENGTH;
        }
        ofs << (unsigned char) (((lastCount + BYTE_LENGTH - 1) % BYTE_LENGTH) + 1);
        ifs.clear();   //  reset
        ifs.seekg(0, std::istream::beg);
        c = static_cast<unsigned char>(ifs.get());
        if (!ifs.good()) {
            return;
        }
        uint64_t buff = 0;
        uint32_t length = 0;
        while (ifs.good()) {
            buff <<= codes[c].second;
            length += codes[c].second;
            buff += codes[c].first;
            if (length > BYTE_LENGTH) {
                ofs << toCharString(buff >> (length % BYTE_LENGTH), length - length % BYTE_LENGTH);
                length = length % BYTE_LENGTH;
                buff = buff % (1 << length);
            }
            c = static_cast<unsigned char>(ifs.get());
        }
        buff <<= (BYTE_LENGTH - (((length + BYTE_LENGTH - 1) % BYTE_LENGTH) + 1));
        if(length != 0) {
            ofs << toCharString(buff, BYTE_LENGTH);
        }
        ofs.close();
        ifs.close();
    }

    unsigned char walk(const ptr_t &cur, uint64_t code, uint32_t length, uint32_t &rest) {
        if (cur->left == nullptr && cur->right == nullptr) {
            rest = length;
            return cur->val;
        }
        if (code >> (length - 1)) {
            return walk(cur->right, code - (1LL << (length - 1)), length - 1, rest);
        }
        return walk(cur->left, code, length - 1, rest);
    }

    void decode(std::ifstream encoded, const std::string &outFileName) {
        std::ofstream ofs(outFileName);
        std::vector<std::pair<uint32_t, uint32_t>> codes(ALPHABET_SIZE);
        uint32_t sym_count[ALPHABET_SIZE];
        unsigned char c;
        for (size_t i = 0; i < ALPHABET_SIZE; i++) {
            std::string s;
            for (size_t j = 0; j < 4; j++) {
                c = static_cast<unsigned char>(encoded.get());
                if(!encoded.good()) {
                    throw std::logic_error("input file corrupted");
                }
                s += c;
            }
            sym_count[i] = charSeqToUI(s, 4);
        }
        ptr_t root = make_tree(sym_count);
        dfs(root, std::pair<uint32_t, uint32_t>(0, 0), codes);
        uint32_t lastCount = 0;
        for (size_t i = 0; i < ALPHABET_SIZE; i++) {
            lastCount += (sym_count[i] * codes[i].second) % BYTE_LENGTH;
        }
        lastCount = (unsigned char) (((lastCount + BYTE_LENGTH - 1) % BYTE_LENGTH) + 1);
        auto lastBitCount = (unsigned char) encoded.get();
        if(lastBitCount != lastCount) {
            throw std::logic_error("input file corrupted");
        }
        uint64_t buff = 0;
        uint32_t length = 0;
        c = static_cast<unsigned char>(encoded.get());
        while (encoded.good()) {
            length += BYTE_LENGTH;
            buff <<= BYTE_LENGTH;
            buff += c;
            while (length >= 4 * BYTE_LENGTH) {
                uint32_t rest;
                ofs << walk(root, buff, length, rest);
                buff = buff % (1LL << rest);
                length = rest;
            }
            c = static_cast<unsigned char>(encoded.get());
        }
        length -= (BYTE_LENGTH - lastBitCount);
        buff >>= (BYTE_LENGTH - lastBitCount);
        while (length > 0) {
            uint32_t rest;
            ofs << walk(root, buff, length, rest);
            buff = buff % (1LL << rest);
            length = rest;
        }
        encoded.close();
        ofs.close();
    }

}

#include <iostream>
#include <map>
#include <queue>
#include <memory>
#include <algorithm>
#include <fstream>


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
        length = (length + 7) / 8;
        std::string ans;
        for (size_t i = 0; i < length; i++) {
            ans.push_back((unsigned char) (num % (1 << 8)));
            num >>= 8;
        }
        for (size_t i = 0; i < length / 2; i++) {
            std::swap(ans[i], ans[length - i - 1]);
        }
        return ans;
    }

    uint32_t charSeqToUI(std::string s, uint32_t length) {
        uint32_t ans = 0;
        for (size_t i = 0; i < length; i++) {
            ans <<= 8;
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
        queue.reserve(256);
        for (size_t c = 0; c < 256; c++) {
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

    void encode(std::ifstream ifs, const std::string &outFileName = "out.txt") {
        // first - code, second - size
        std::vector<std::pair<uint32_t, uint32_t>> codes(256);
        std::ofstream ofs(outFileName);
        uint32_t sym_count[256];
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
        for (size_t i = 0; i < 256; i++) {
            ofs << toCharString(sym_count[i], 32);
            lastCount += (sym_count[i] * codes[i].second) % 8;
        }
        ofs << (unsigned char) (((lastCount + 7) % 8) + 1);
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
            if (length > 8) {
                ofs << toCharString(buff >> (length % 8), length - length % 8);
                length = length % 8;
                buff = buff % (1 << length);
            }
            c = static_cast<unsigned char>(ifs.get());
        }
        buff <<= (8 - (((length + 7) % 8) + 1));
        if(length != 0) {
            ofs << toCharString(buff, 8);
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

    void decode(std::ifstream encoded, const std::string &outFileName = "out.txt") {
        std::ofstream ofs(outFileName);
        uint32_t sym_count[256];
        unsigned char c;
        for (size_t i = 0; i < 256; i++) {
            std::string s;
            for (size_t j = 0; j < 4; j++) {
                c = static_cast<unsigned char>(encoded.get());
                if(!encoded.good()) {
                    throw std::logic_error("file corrupted");
                }
                s += c;
            }
            sym_count[i] = charSeqToUI(s, 4);
        }
        auto lastBitCount = encoded.get();
        ptr_t root = make_tree(sym_count);
        uint64_t buff = 0;
        uint32_t length = 0;
        c = static_cast<unsigned char>(encoded.get());
        while (encoded.good()) {
            length += 8;
            buff <<= 8;
            buff += c;
            while (length >= 32) {
                uint32_t rest;
                ofs << walk(root, buff, length, rest);
                buff = buff % (1LL << rest);
                length = rest;
            }
            c = static_cast<unsigned char>(encoded.get());
        }
        length -= (8 - lastBitCount);
        buff >>= (8 - lastBitCount);
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

#include <fstream>
#include <iostream>
#include <sstream>

#ifndef LBXTEST_DATA_STATIC_H
#define LBXTEST_DATA_STATIC_H
#include <inttypes.h>

#include "lbx.h"
const unsigned LBXTEST_DATA_STATIC_SIZE = 62;
const uint8_t LBXTEST_DATA_STATIC[LBXTEST_DATA_STATIC_SIZE] = {5, 0, 173, 254, 0, 0, 42, 0, 37, 0, 0, 0, 42, 0, 0, 0, 47, 0, 0, 0, 52, 0, 0, 0, 57, 0, 0, 0, 62, 0, 0, 0, 116, 101, 115, 116, 255, 116, 101, 115, 116, 0, 116, 101, 115, 116, 1, 116, 101, 115, 116, 2, 116, 101, 115, 116, 3, 116, 101, 115, 116, 4};
#endif /* LBXTEST_DATA_STATIC_H */

LbxFile::Content gen(uint8_t nr) {
    LbxFile::Content c;
    c.data = new char[5];
    c.data[0] = 't';
    c.data[1] = 'e';
    c.data[2] = 's';
    c.data[3] = 't';
    c.data[4] = (char) nr;
    c.size = 5;
    return c;
}

uint8_t get(LbxFile::Content& content) {
    if (content.size != 5
            || content.data[0] != 't'
            || content.data[1] != 'e'
            || content.data[2] != 's'
            || content.data[3] != 't') {
        throw std::runtime_error("invalid content");
    }
    return (uint8_t) content.data[4];
}

#define LBX_TEST_CMP(x, y, z){\
    if (y != z) {\
        std::stringstream e;\
        e << msg << " test failed - " << x << " missmatch (" << y << "!=" << z << ")";\
        throw std::runtime_error(e.str());\
    }\
}

void cmp(std::string msg, LbxFile& lbx, uint16_t unknown0, uint8_t unknown1, std::vector<uint8_t> values) {
    LBX_TEST_CMP("size", lbx.size(), values.size());
    LBX_TEST_CMP("unknown0", lbx.unknown0(), unknown0);
    LBX_TEST_CMP("unknown1", get(lbx.unknown1()), unknown1);
    for (uint16_t i = 0; i < lbx.size(); ++i) {
        LBX_TEST_CMP("data[" << i << "]", get(lbx[i]), values[i]);
    }
}

void test() {
    // test swap16
    for (uint16_t i = 0;; ++i) {
        std::string msg("swap16");
        LBX_TEST_CMP("number", i, LbxFile::swap16(LbxFile::swap16(i)));
        if (i == UINT16_MAX) {
            break;
        }
    }

    // test swap32
    for (uint32_t i = 255;; i += 256) {
        std::string msg("swap32");
        LBX_TEST_CMP("number", i, LbxFile::swap32(LbxFile::swap32(i)));
        if (i == UINT32_MAX) {
            break;
        }
    }


    // write test file
    {
        std::ofstream o("test0.lbx");
        if (!o.good()) {
            throw std::runtime_error("could not write lbx test file");
        }
        o.write((char*) &LBXTEST_DATA_STATIC, LBXTEST_DATA_STATIC_SIZE);
    }

    // test file class
    std::string msg("class");
    
    LbxFile lbx("test0.lbx");
    LBX_TEST_CMP("path0", "test0.lbx", lbx.path())
    cmp("read", lbx, 42, 255,{0, 1, 2, 3, 4});

    lbx.save("test1.lbx");
    LBX_TEST_CMP("path1", "test1.lbx", lbx.path())
    lbx.open("test1.lbx");
    cmp("save", lbx, 42, 255,{0, 1, 2, 3, 4});

    lbx.remove(0);
    cmp("remove start", lbx, 42, 255,{1, 2, 3, 4});

    lbx.remove(3);
    cmp("remove end", lbx, 42, 255,{1, 2, 3});

    lbx.remove(1);
    cmp("remove middle", lbx, 42, 255,{1, 3});
    
    lbx.swap(0, 1);
    cmp("swap", lbx, 42, 255,{3, 1});

    lbx.insert(0, gen(4));
    cmp("insert start", lbx, 42, 255,{4, 3, 1});

    lbx.insert(3, gen(0));
    cmp("insert end", lbx, 42, 255,{4, 3, 1, 0});

    lbx.insert(2, gen(2));
    cmp("insert middle", lbx, 42, 255,{4, 3, 2, 1, 0});

    lbx.add(gen(5));
    cmp("add", lbx, 42, 255,{4, 3, 2, 1, 0, 5});

    lbx.unknown0() = 7;
    cmp("unknown0", lbx, 7, 255,{4, 3, 2, 1, 0, 5});

    delete[] lbx.unknown1().data;
    lbx.unknown1() = gen(23);
    cmp("unknown1", lbx, 7, 23,{4, 3, 2, 1, 0, 5});

    lbx.clear();
    cmp("clear", lbx, 7, 23,{});
}

int main(int argc, char** argv) {
    try {
        test();
    } catch (std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

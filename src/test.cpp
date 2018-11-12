#include <fstream>
#include <iostream>
#include <sstream>

#include "lbx.h"

const size_t LBXTEST_DATA_SIZE = 80;
const uint8_t LBXTEST_DATA[80] = {5, 0, 173, 254, 0, 0, 42, 0, 40, 0, 0, 0, 48, 0, 0, 0, 56, 0, 0, 0, 64, 0, 0, 0, 72, 0, 0, 0, 80, 0, 0, 0, 116, 101, 255, 0, 0, 0, 115, 116, 116, 101, 0, 0, 0, 0, 115, 116, 116, 101, 1, 0, 0, 0, 115, 116, 116, 101, 2, 0, 0, 0, 115, 116, 116, 101, 3, 0, 0, 0, 115, 116, 116, 101, 4, 0, 0, 0, 115, 116};

std::pair<char*, uint32_t> gen(uint32_t nr) {
    std::pair<char*, uint32_t> p;
    p.first = new char[8];
    p.first[0] = 't';
    p.first[1] = 'e';
    p.first[6] = 's';
    p.first[7] = 't';
    p.second = 8;
    LbxFile::write32(p.first, 2, nr);
    return p;
}

uint32_t get(std::pair<char*, uint32_t>& content) {
    if (content.second != 8
            || content.first[0] != 't'
            || content.first[1] != 'e'
            || content.first[6] != 's'
            || content.first[7] != 't') {
        throw std::runtime_error("invalid content");
    }

    return LbxFile::read32(content.first, 2);
}

#define LBX_TEST_CMP(x, y, z){\
    if (y != z) {\
        std::stringstream e;\
        e << msg << " test failed - " << x << " missmatch (" << y << "!=" << z << ")";\
        throw std::runtime_error(e.str());\
    }\
}

void cmp(std::string msg, LbxFile& lbx, uint16_t unknown0, uint8_t unknown1, std::vector<uint32_t> values) {
    LBX_TEST_CMP("size", lbx.size(), values.size());
    LBX_TEST_CMP("unknown0", lbx.unknown0(), unknown0);
    LBX_TEST_CMP("unknown1", get(lbx.unknown1()), unknown1);
    for (uint16_t i = 0; i < lbx.size(); ++i) {
        LBX_TEST_CMP("data[" << i << "]", get(lbx[i]), values[i]);
    }
}

void test() {
    // test read8 / write8
    {
        char d[2];
        std::string msg("read8/write8");
        for (uint32_t o = 0; o < 2; ++o) {
            for (uint8_t i = 0;; ++i) {

                LbxFile::write8(d, o, i);
                LBX_TEST_CMP("number", (uint32_t) i, (uint32_t) LbxFile::read8(d, o));

                if (i == UINT8_MAX) {
                    break;
                }
            }
        }
    }

    // test read16 / write16
    {
        char d[3];
        std::string msg("read16/write16");
        for (uint32_t o = 0; o < 2; ++o) {
            for (uint16_t i = 0;; ++i) {

                LbxFile::write16(d, o, i);
                LBX_TEST_CMP("number", i, LbxFile::read16(d, o));

                if (i == UINT16_MAX) {
                    break;
                }
            }
        }
    }

    // test read32 / write32
    {
        char d[5];
        std::string msg("read32/write32");
        for (uint32_t o = 0; o < 2; ++o) {
            for (uint32_t i = 255;; i += 256) {

                LbxFile::write32(d, o, i);
                LBX_TEST_CMP("number", i, LbxFile::read32(d, o));

                if (i == UINT32_MAX) {
                    break;
                }
            }
        }
    }

    // test file class
    std::string msg("class");

    LbxFile lbx(std::pair<char*, uint32_t>((char*) &LBXTEST_DATA, LBXTEST_DATA_SIZE));
    LBX_TEST_CMP("path0", "", lbx.path())
    cmp("read", lbx, 42, 255,{0, 1, 2, 3, 4});

    lbx.save("test.lbx");
    LBX_TEST_CMP("path1", "test.lbx", lbx.path())
    lbx.open("test.lbx");
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

    delete[] lbx.unknown1().first;
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

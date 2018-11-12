#include <fstream>
#include <stdexcept>

#include "lbx.h"

LbxFile::LbxFile() {
    u0 = 0;
    u1.first = NULL;
    u1.second = 0;
}

LbxFile::LbxFile(std::string _path) : LbxFile() {
    open(_path);
}

LbxFile::LbxFile(std::pair<char*, uint32_t> _data) {
    load(_data);
}

LbxFile::~LbxFile() {
    clear();
    delete[] u1.first;
}

void LbxFile::clear() {
    for (uint16_t i = 0; i < content.size(); ++i) {
        delete[] at(i).first;
    }
    content.clear();
}

std::pair<char*, uint32_t>& LbxFile::operator[](uint16_t _index) {
    return at(_index);
}

std::pair<char*, uint32_t>& LbxFile::at(uint16_t _index) {
    return content[_index];
}

std::string LbxFile::path() {
    return filePath;
}

void LbxFile::add(std::pair<char*, uint32_t> _content) {
    content.push_back(_content);
}

uint16_t LbxFile::size() {
    return (uint16_t) content.size();
}

uint16_t& LbxFile::unknown0() {
    return u0;
}

std::pair<char*, uint32_t>& LbxFile::unknown1() {
    return u1;
}

void LbxFile::save() {
    save(path());
}

void LbxFile::save(std::string _path) {
    std::ofstream o(_path, std::ifstream::binary);
    if (!o.good()) {
        throw std::runtime_error("could not write to '" + _path + "'");
    }
    std::pair<char*, uint32_t> d = serialize();
    o.write(d.first, d.second);
    delete[] d.first;
    filePath = _path;
}

std::pair<char*, uint32_t> LbxFile::serialize() {
    // get size and create array
    uint32_t s = offset(size());
    char* d = new char[s];
    
    // write header
    write16(d, 0, size());
    write32(d, 2, SIGNATURE);
    write16(d, 6, u0);
    
    // write offsets
    for (uint16_t i = 0; i <= size(); ++i) {
        write32(d, 8 + 4 * i, offset(i));
    }
    
    // write unknown area
    for (uint32_t i = 0; i < u1.second; ++i) {
        d[headerSize() + i] = u1.first[i];
    }
    
    // write content
    for (uint16_t i = 0; i < size(); ++i) {
        for (uint32_t j = 0; j < at(i).second; ++j) {
            d[offset(i) + j] = at(i).first[j];
        }
    }
    
    return {d, s};
}

uint32_t LbxFile::offset(uint16_t _index) {
    uint32_t o = headerSize() + unknown1().second;
    for (uint16_t i = 0; i < _index; ++i) {
        o += at(i).second;
    }
    return o;
}

uint32_t LbxFile::headerSize() {
    return headerSize(size());
}

uint32_t LbxFile::headerSize(uint16_t _nrOfFiles) {
    return _nrOfFiles * 4 + 12;
}

uint8_t LbxFile::read8(char* _data, uint32_t _offset) {
    return (uint8_t) _data[_offset];
}

uint16_t LbxFile::read16(char* _data, uint32_t _offset) {
    return (uint16_t) (read8(_data, _offset) | read8(_data, _offset + 1) << 8);
}

uint32_t LbxFile::read32(char* _data, uint32_t _offset) {
    return (uint32_t) (read16(_data, _offset) | read16(_data, _offset + 2) << 16);
}

void LbxFile::write8(char* _data, uint32_t _offset, uint8_t _value) {
    _data[_offset] = (char) _value;
}

void LbxFile::write16(char* _data, uint32_t _offset, uint16_t _value) {
    write8(_data, _offset, (uint8_t) _value);
    write8(_data, _offset + 1, (uint8_t) (_value >> 8));
}

void LbxFile::write32(char* _data, uint32_t _offset, uint32_t _value) {
    write16(_data, _offset, (uint16_t) _value);
    write16(_data, _offset + 2, (uint16_t) (_value >> 16));
}

void LbxFile::remove(uint16_t _index) {
    delete[] at(_index).first;
    content.erase(content.begin() + _index);
}

void LbxFile::insert(uint16_t _index, std::pair<char*, uint32_t> _content) {
    content.insert(content.begin() + _index, _content);
}

void LbxFile::swap(uint16_t index0, uint16_t index1) {
    std::pair<char*, uint32_t> p = at(index0);
    at(index0) = at(index1);
    at(index1) = p;
}

void LbxFile::open(std::string _path) {
    std::ifstream f(_path, std::ifstream::ate | std::ifstream::binary);
    if (!f.good()) {
        throw std::runtime_error("could not read file");
    }

    uint32_t s = (uint32_t) f.tellg();
    char* d = new char[s];
    f.seekg(0);
    f.read(d, s);

    load({d, s});
    delete[] d;

    filePath = _path;
}

void LbxFile::load(std::pair<char*, uint32_t> _data) {

    // check if size is big enought for header
    if (_data.second < 12 || _data.second > UINT32_MAX) {
        throw std::runtime_error("too small");
    }

    // read number of files and check file size again
    uint16_t nrOfFiles = read16(_data.first, 0);
    if (_data.second < (uint32_t) (12 + nrOfFiles * 4)) {
        throw std::runtime_error("too small for header");
    }

    // check signature
    if (read32(_data.first, 2) != SIGNATURE) {
        throw std::runtime_error("invalid signature");
    }

    // check if offsets make sense
    for (uint16_t i = 0; i < nrOfFiles; ++i) {
        if (read32(_data.first, 8 + i * 4) > read32(_data.first, 8 + i * 4)) {
            throw std::runtime_error("invalid offsets");
        }
    }

    // check if last offset matches file size
    if (read32(_data.first, 8 + nrOfFiles * 4) != _data.second) {
        throw std::runtime_error("invalid offset");
    }

    // file is good, clean up
    clear();
    delete[] u1.first;

    // read unknown data
    u0 = read16(_data.first, 6);
    u1.second = read32(_data.first, 8) - headerSize(nrOfFiles);
    u1.first = new char[u1.second];
    for (uint32_t i = 0; i < u1.second; ++i) {
        u1.first[i] = _data.first[headerSize(nrOfFiles) + i];
    }

    // read file data
    for (uint16_t i = 0; i < nrOfFiles; ++i) {
        uint32_t o = read32(_data.first, 8 + i * 4);
        uint32_t s = read32(_data.first, 12 + i * 4) - o;
        char* d = new char[s];
        for (uint32_t j = 0; j < s; ++j) {
            d[j] = _data.first[o + j];
        }
        add({d, s});
    }
}
#include <fstream>
#include <stdexcept>

#include "lbx.h"

LbxFile::LbxFile() {
    u0 = 0;
    u1.data = NULL;
    u1.size = 0;
}

LbxFile::LbxFile(std::string _path) : LbxFile() {
    open(_path);
}

LbxFile::~LbxFile() {
    clear();
    delete[] u1.data;
}

void LbxFile::clear() {
    for (uint16_t i = 0; i < content.size(); ++i) {
        delete[] at(i).data;
    }
    content.clear();
}

LbxFile::Content& LbxFile::operator[](uint16_t _index) {
    return at(_index);
}

LbxFile::Content& LbxFile::at(uint16_t _index) {
    return content[_index];
}

std::string LbxFile::path() {
    return filePath;
}

void LbxFile::add(Content _content) {
    content.push_back(_content);
}

void LbxFile::add(char* _data, uint32_t _size) {
    Content c;
    c.data = _data;
    c.size = _size;
    add(c);
}

uint16_t LbxFile::size() {
    return (uint16_t) content.size();
}

uint16_t& LbxFile::unknown0() {
    return u0;
}

LbxFile::Content& LbxFile::unknown1() {
    return u1;
}

#define LBX_WRITE_16(x) {\
    uint16_t w16 = x;\
    o.write((char*) &w16, 2);\
}

#define LBX_WRITE_32(x) {\
    uint32_t w32 = x;\
    o.write((char*) &w32, 4);\
}

void LbxFile::save() {
    save(path());
}

void LbxFile::save(std::string _path) {
    std::ofstream o(_path, std::ifstream::binary);
    if (!o.good()) {
        throw std::runtime_error("could not write to '" + _path + "'");
    }

    LBX_WRITE_16(size());
    LBX_WRITE_32(SIGNATURE);
    LBX_WRITE_16(u0);

    uint32_t offset = headerSize() + unknown1().size;
    LBX_WRITE_32(offset);
    for (uint16_t i = 0; i < size(); ++i) {
        offset += at(i).size;
        LBX_WRITE_32(offset);
    }

    o.write(u1.data, u1.size);

    for (uint16_t i = 0; i < size(); ++i) {
        o.write(at(i).data, at(i).size);
    }
    
    filePath = _path;
}

uint32_t LbxFile::offset(uint16_t _index) {
    uint32_t o = headerSize() + unknown1().size;
    for (uint16_t i = 0; i < _index; ++i) {
        o += at(i).size;
    }
    return o;
}

uint32_t LbxFile::headerSize() {
    return headerSize(size());
}

uint32_t LbxFile::headerSize(uint16_t _nrOfFiles) {
    return _nrOfFiles * 4 + 12;
}

uint16_t LbxFile::swap16(uint16_t _nr) {
    return (uint16_t) ((_nr << 8) | (_nr >> 8));
}

uint32_t LbxFile::swap32(uint32_t _nr) {
    _nr = ((_nr << 8) & 0xFF00FF00) | ((_nr >> 8) & 0xFF00FF);
    return (_nr << 16) | (_nr >> 16);
}

void LbxFile::remove(uint16_t _index) {
    delete[] at(_index).data;
    content.erase(content.begin() + _index);
}

void LbxFile::insert(uint16_t _index, Content _content) {
    content.insert(content.begin() + _index, _content);
}

void LbxFile::swap(uint16_t index0, uint16_t index1) {
    Content c = at(index0);
    at(index0) = at(index1);
    at(index1) = c;
}

#define LBX_OPEN_ERROR(x)\
throw std::runtime_error("could not open '" + _path + "' - invalid " + x);

void LbxFile::open(std::string _path) {
    // open file at the end to get file size, then set pointer to beginning
    std::ifstream file(_path, std::ifstream::ate | std::ifstream::binary);
    if (!file.good()) {
        LBX_OPEN_ERROR("file");
    }
    std::ifstream::pos_type fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // check if file size is big enought for header
    if (fileSize < 12 || fileSize > UINT32_MAX) {
        LBX_OPEN_ERROR("size");
    }

    uint16_t nrOfFiles;
    file.read((char*) & nrOfFiles, 2);
    if (fileSize < (uint32_t) (12 + nrOfFiles * 4)) {
        LBX_OPEN_ERROR("size");
    }

    uint32_t signature;
    file.read((char*) & signature, 4);
    if (signature != SIGNATURE) {
        LBX_OPEN_ERROR("signature");
    }

    uint16_t wtf;
    file.read((char*) &wtf, 2);
    
    uint32_t* offsets = new uint32_t[nrOfFiles + 1];
    for (uint16_t i = 0; i <= nrOfFiles; ++i) {
        file.read((char*) & offsets[i], 4);
        if (i > 0 && (offsets[i] < offsets[i - 1] || offsets[i] > fileSize)) {
            delete[] offsets;
            LBX_OPEN_ERROR("offset");
        }
    }

    if (fileSize != offsets[nrOfFiles]) {
        delete[] offsets;
        LBX_OPEN_ERROR("offset");
    }

    // file is good, clean up
    clear();
    delete[] u1.data;
    filePath = _path;

    // save unknown data
    u0 = wtf;
    u1.size = offsets[0] - headerSize(nrOfFiles);
    u1.data = new char[u1.size];
    file.read(u1.data, u1.size);

    // read file data
    for (uint16_t i = 0; i < nrOfFiles; ++i) {
        uint32_t s = offsets[i + 1] - offsets[i];
        char* d = new char[s];
        file.read(d, s);
        add(d, s);
    }
    
    delete[] offsets;
}
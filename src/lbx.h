#ifndef LBX_H
#define LBX_H

#include <string>
#include <vector>

/**
 * A lbx file is a simple container format.
 * 
 */
class LbxFile {
public:

    /**
     * A new empty lbx file.
     */
    LbxFile();

    /**
     * Construct a lbx file using given data in memory.
     * Throws exception if something goes wrong.
     * @param data
     */
    LbxFile(std::pair<char*, uint32_t> data);

    /**
     * Load a lbx file from given path.
     * Throws exception if something goes wrong.
     * @param path
     */
    LbxFile(std::string path);

    /**
     * Destructor.
     */
    virtual ~LbxFile();

    /**
     * Save lbx file.
     * Throws exception if something goes wrong.
     */
    void save();

    /**
     * Save lbx file to given path.
     * Throws exception if something goes wrong.
     * @param path
     */
    void save(std::string path);
    
    /**
     * Serialize lbx file.
     * You have to take care of deleting the created data.
     * @return 
     */
    std::pair<char*, uint32_t> serialize();

    /**
     * Open lbx file from given path.
     * Previous content will be deleted.
     * Throws exception if something goes wrong.
     * @param path
     */
    void open(std::string path);

    /**
     * Load a lbx file from give data in memory.
     * Throws exception if something goes wrong.
     * @param data
     * @param size
     */
    void load(std::pair<char*, uint32_t> data);

    /**
     * Get path of lbx file.
     * @return path
     */
    std::string path();

    /**
     * Get number of files contained in the lbx archive.
     * @return number of files
     */
    uint16_t size();

    /**
     * Get content at given index.
     * @param index
     * @return content
     */
    std::pair<char*, uint32_t>& at(uint16_t index);

    /**
     * Get content at given index.
     * @param index
     * @return content
     */
    std::pair<char*, uint32_t>& operator[](uint16_t index);

    /**
     * Calculate starting offset for given index.
     * @param index
     * @return file offset
     */
    uint32_t offset(uint16_t index);

    /**
     * Deletes all content.
     * "Unknown" values will not be touched.
     */
    void clear();

    /**
     * Add content to archive.
     * Added data will be deleted by this class.
     * @param content
     */
    void add(std::pair<char*, uint32_t> content);

    /**
     * Insert content at given index.
     * Inserted data will be deleted by this class.
     * @param index
     * @param content
     */
    void insert(uint16_t index, std::pair<char*, uint32_t> content);

    /**
     * Switch content at given indexes.
     * @param index0
     * @param index1
     */
    void swap(uint16_t index0, uint16_t index1);

    /**
     * Removes and deletes content at given index.
     * @param index
     */
    void remove(uint16_t index);

    /**
     * Get and set unknown 2byte header value.
     * @return uint16_t reference
     */
    uint16_t& unknown0();

    /**
     * Get and set unknown space.
     * @return content reference
     */
    std::pair<char*, uint32_t>& unknown1();

    /**
     * Calculate header size.
     * @return header size
     */
    uint32_t headerSize();

    /**
     * Static function to calculate header size based on given number of files.
     * @param nrOfFiles
     * @return header size
     */
    static uint32_t headerSize(uint16_t nrOfFiles);

    /**
     * Static function to read a uint8_t value from a data array.
     * @param data
     * @param offset
     * @return 
     */
    static uint8_t read8(char* data, uint32_t offset);

    /**
     * Static function to read a uint16_t value from a little endian data array.
     * @param data
     * @param offset
     * @return 
     */
    static uint16_t read16(char* data, uint32_t offset);

    /**
     * Static function to read a uint32_t value from a little endian data array.
     * @param data
     * @param offset
     * @return 
     */
    static uint32_t read32(char* data, uint32_t offset);

    /**
     * Static function to write a uint8_t value to a data array.
     * @param data
     * @param offset
     * @return 
     */
    static void write8(char* data, uint32_t offset, uint8_t value);

    /**
     * Static function to write a uint16_t value to a little endian data array.
     * @param data
     * @param offset
     * @return 
     */
    static void write16(char* data, uint32_t offset, uint16_t value);

    /**
     * Static function to write a uint32_t value to a little endian data array.
     * @param data
     * @param offset
     * @return 
     */
    static void write32(char* data, uint32_t offset, uint32_t value);

    /**
     * 4byte signature at byte 2 to 6.
     */
    static const uint32_t SIGNATURE = 65197;

private:
    uint16_t u0;
    std::pair<char*, uint32_t> u1;
    std::vector<std::pair<char*, uint32_t>> content;
    std::string filePath;
};

#endif /* LBX_H */
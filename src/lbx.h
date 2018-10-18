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
     * Simple container class to hold archived files.
     */
    class Content {
    public:
        char* data;
        uint32_t size;
    };

    /**
     * A new empty lbx file.
     */
    LbxFile();

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
     * Save lbx file to given path.
     * Throws exception if something goes wrong.
     * @param path
     */
    void save(std::string path);

    /**
     * Open lbx file from given path.
     * Previous content will be deleted.
     * Throws exception if something goes wrong.
     * @param path
     */
    void open(std::string path);

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
    Content& at(uint16_t index);

    /**
     * Get content at given index.
     * @param index
     * @return content
     */
    Content& operator[](uint16_t index);
    
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
    void add(Content content);
    
    /**
     * Add content to archive.
     * Added data will be deleted by this class.
     * @param data
     * @param size
     */
    void add(char* data, uint32_t size);
    
    /**
     * Insert content at given index.
     * Inserted data will be deleted by this class.
     * @param index
     * @param content
     */
    void insert(uint16_t index, Content content);
    
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
    Content& unknown1();

    /**
     * Static function to easily switch endianess of a given number.
     * @param nr
     * @return uint32_t with switched endian
     */
    static uint16_t swap16(uint16_t nr);
    
    /**
     * Static function to easily switch endianess of a given number.
     * @param nr
     * @return uint32_t with switched endian
     */
    static uint32_t swap32(uint32_t nr);

    /**
     * 4byte signature at byte 2 to 6.
     */
    static const uint32_t SIGNATURE = 65197;

private:
    uint32_t headerSize();
    static uint32_t headerSize(uint16_t nrOfFiles);
    uint16_t u0;
    Content u1;
    std::vector<Content> content;
};

#endif /* LBX_H */
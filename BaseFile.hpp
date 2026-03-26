#include <cstdio>
using namespace std;

class BaseFile{
    private:
    FILE* file;
    bool readable = false;
    bool writable = false;
    
    public:

    BaseFile() : file(nullptr) {}
    BaseFile(const char* path, const char* par) {
        file = fopen(path, par);
    }
    BaseFile(FILE* f) : file(f) {}
    ~BaseFile() {
        if (file != nullptr) {
            fclose(file);
        }
    }
    bool is_open() {return file != nullptr;};
    bool can_read() {return file && readable;};
    bool can_write() {return file && writable;};
    
    size_t write_raw(const void *buf, size_t n_bytes); 
    size_t read_raw(void *buf, size_t max_bytes);
    long tell();
    bool seek(long offset);
    size_t write(const void *buf, size_t n_bytes); 
    size_t read(void *buf, size_t max_bytes);
    
};
#include <cstdio>
#include <iostream>
using namespace std;

class BaseFile{
    protected:
    FILE* file;
    bool readable = false;
    bool writable = false;
    public:

    BaseFile() : file(nullptr) {}
    BaseFile(const char* path, const char* par);
    BaseFile(FILE* f) : file(f) {}
    virtual ~BaseFile() {
        if (file != nullptr) {
            fclose(file);
        }
        //cout << "BaseFile Destructor" << endl;
    }
    bool is_open() {return (file);};
    bool can_read() {return readable;};
    bool can_write() {return writable;};
    
    size_t write_raw(const void *buf, size_t n_bytes); 
    size_t read_raw(void *buf, size_t max_bytes);
    long tell();
    bool seek(long offset);
    virtual size_t write(const void *buf, size_t n_bytes); 
    virtual size_t read(void *buf, size_t max_bytes);  
};
class Base32File:public BaseFile{
    public:
    Base32File(const char* path, const char* par):BaseFile(path,par){};
    Base32File(const char* path, const char* par, const char* table):BaseFile(path,par){encode_table=table;};
    size_t write(const void *buf, size_t n_bytes) override; 
    size_t read(void *buf, size_t max_bytes) override;  
    private:
    int encoded32_size(int raw_size);
    int decoded32_size(int encode_size);
    int encode32(const char *raw_data, int raw_size, char *dst);
    int decode32(const char *encoded_data, int encoded_size, char *dst);
    const char* encode_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZ123456";

};
class RleFile : public BaseFile {
private:
public:
    RleFile(const char* path, const char* par):BaseFile(path,par){
        cout << "RleFile Constructor" << endl;
    };
    size_t write(const char* data, size_t size);
    size_t read(char* buf, size_t size);
};
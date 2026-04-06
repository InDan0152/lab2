#include <cstdio>
#include <iostream>
using namespace std;


class IFile{
    public:
    virtual bool can_read() const = 0;
    virtual bool can_write() const = 0;
    virtual size_t write(const void *buf, size_t n_bytes) = 0; 
    virtual size_t read(void *buf, size_t max_bytes) = 0; 
    
};

class BaseFile:public IFile{
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
    bool can_read() const override;
    bool can_write() const override;
    
    size_t write_raw(const void *buf, size_t n_bytes); 
    size_t read_raw(void *buf, size_t max_bytes);
    long tell();
    bool seek(long offset);
    size_t write(const void *buf, size_t n_bytes) override; 
    size_t read(void *buf, size_t max_bytes) override; 
     
};
class Base32File:public BaseFile{
    public:
    Base32File(const char* path, const char* par):BaseFile(path,par){};
    Base32File(const char* path, const char* par, const char* table):BaseFile(path,par){encode_table=table;};
    size_t write(const void *buf, size_t n_bytes); 
    size_t read(void *buf, size_t max_bytes);  
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
        //cout << "RleFile Constructor" << endl;
    };
    size_t write(const char* data, size_t size);
    size_t read(char* buf, size_t size);
};
void write_int(IFile &file, int n);
class Base32File2:public IFile{
private:
    IFile* target;
    const char* encode_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZ123456";
public:
    Base32File2(IFile* file) : target(file) {}
    ~Base32File2 () {delete target;}
    bool can_read() const override { return target->can_read(); }
    bool can_write() const override { return target->can_write(); }
    
    size_t write(const void *buf, size_t n_bytes) override;
    size_t read(void *buf, size_t n_bytes) override;
    int encode32(const char *raw_data, int raw_size, char *dst);
    int encoded32_size(int raw_size);
};

class RleFile2:public IFile{
    private:
    IFile* target;
    public:
    RleFile2(IFile* t) : target(t) {}
    ~RleFile2() {delete target;}
    size_t write(const void* buf, size_t n_bytes) override;
    size_t read(void* buf, size_t n_bytes) override;
    bool can_read() const override { return target->can_read(); }
    bool can_write() const override { return target->can_write(); }
    size_t write_raw(const void* buf, size_t n_bytes);
};

#include "BaseFile.hpp"
#include <cstdio>
using namespace std;

    size_t BaseFile::write_raw(const void *buf, size_t n_bytes) {
    if (!is_open() || !can_write()) {
        return 0;
    }
    size_t written = fwrite(buf, 1, n_bytes, file); //fwrite(откуда, рамер 1 эл, кол-во эл, ук на файл)
    return written; 
}

    size_t BaseFile::read_raw(void *buf, size_t max_bytes) {
    if (!is_open() || !can_read()) {
        return 0; 
    }
    size_t bytes_read = fread(buf, 1, max_bytes, file); //fread(куда, рамер 1 эл, макс эл, ук на файл)
    return bytes_read;
}

    long BaseFile::tell() {
    if (!is_open()) {
        return false; 
    }
    long position = ftell(file);
    return position;
}

    bool BaseFile::seek(long offset){
        if (!is_open()) {
        return false; 
    }
    int result = fseek(file, offset, SEEK_SET);// SEEK_SET(считаем смещение от начала) offset(длина прыжка)
    return (result == 0);
    }
    size_t BaseFile::read(void *buf, size_t max_bytes) {
    if (!is_open() || !can_read()) {
        return 0; 
    }
    size_t bytes_read = fread(buf, 1, max_bytes, file); //fread(куда, рамер 1 эл, макс эл, ук на файл)
    return bytes_read;
}
    size_t BaseFile::write(const void *buf, size_t n_bytes) {
    if (!is_open() || !can_write()) {
        return 0;
    }
    size_t written = fwrite(buf, 1, n_bytes, file); //fwrite(откуда, рамер 1 эл, кол-во эл, ук на файл)
    return written; 
}

#include "BaseFile.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>
using namespace std;

bool strchr(const char* str, const char ch){
    if (str) {
        while(*str) {
            if (*str==ch) {
                return true;
            }
            str++;
        }
    }
    return false;
}

BaseFile::BaseFile(const char* path, const char* par) {
    file = fopen(path, par);
    //cout << "BaseFile Constructor" << path << " par=" << endl;
    if (file) { // "rb", "wb+", "ab", "ab+"
        if (strchr(par,'+')) { // strchr ищет символ 
            readable = true;
            writable = true;
        }
        // нельзя одновременно ar aw rw
        if (strchr(par,'r')) {
            readable = true;
        }
        else 
        if (strchr(par,'w')) { // в режиме w и a без + читать нельзя 
            writable = true;
        }
        else 
        if (strchr(par,'a')) {
            writable = true;
        }
    }
}



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
        return -1; 
    }
    long position = ftell(file);
    return position;
}

bool BaseFile::seek(long offset){
    if (!is_open()) {
       return -1; 
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
int Base32File::encoded32_size(int raw_size)
{
    return (raw_size * 8 + 4) / 5; //+4 округляет вверх
}

int Base32File::decoded32_size(int encode_size)
{
    return (encode_size * 5) / 8; // Преобразуем в байты общее кол-во бит
}
int Base32File::encode32(const char *raw_data, int raw_size, char *dst)
{
    // Проверка на корректный ввод
    if (!raw_data || raw_size <= 0 || !dst)
    {
        return 1;
    }

    int bit_buffer = 0;     // Буфер для битов - временной хранилище
    int bits_in_buffer = 0; // Биты в буффере
    int dst_index = 0;      // Индекс в выходном массиве

    // Накапливаем буффер
    for (int i = 0; i < raw_size; i++)
    {

        // Добавляем байт в буфер, сдвиг на 8 бит влево
        bit_buffer = (bit_buffer << 8) | (unsigned char)raw_data[i]; 
        // сдвигаем влево, чтобы освободить место для новых данных
        bits_in_buffer += 8;

        // Пока в буфере >=5 бит, извлекаем группы по 5 бит.
        while (bits_in_buffer >= 5)
        {
            int index = (bit_buffer >> (bits_in_buffer - 5)) & 0x1F; // Извлекаем старшие 5 бит, используя побитовую маску из 5 единиц
            dst[dst_index++] = encode_table[index];                  // Находим соответсвующий символ и записываем его в массив
            bits_in_buffer -= 5;
        }
    }

    // Если после всех байт осталось < 5 бит, добиваем нулями справа до 5 бит.
    if (bits_in_buffer > 0)
    {
        int index = (bit_buffer << (5 - bits_in_buffer)) & 0x1F;
        dst[dst_index++] = encode_table[index];
    }

    return 0;
}
/*Ключевой принцип: Эта функция работает как "дробилка": она собирает биты из входных 8-битных байтов 
в один большой временный буфер (bit_buffer), а затем "откусывает" от него по 5 бит за раз, 
преобразуя каждую 5-битную группу в символ из encode_table.*/
int Base32File::decode32(const char *encoded_data, int encoded_size, char *dst)

{
    if (!encoded_data || encoded_size <= 0 || !dst)
    {
        return 1;
    }

    int bit_buffer = 0;
    int bits_in_buffer = 0;
    int dst_index = 0;

    for (int i = 0; i < encoded_size; ++i)
    {
        int index;

        // Определяем индекса символа из таблицы
        if (encoded_data[i] >= 'A' && encoded_data[i] <= 'Z')
        {
            // A - 0, B - 1, ..., Z - 25
            index = encoded_data[i] - 'A';
        }
        else if (encoded_data[i] >= '1' && encoded_data[i] <= '6')
        {
            // Продолжаем, '1' = 26, ...
            index = encoded_data[i] - '1' + 26;
        }
        else
        {
            return 2; // Недопустимый символ
        }

        // Освобождаем место в конце буфера, сдвигая имеющиеся символы, вставляем полученные биты
        bit_buffer = (bit_buffer << 5) | index;
        bits_in_buffer += 5;

        // Отрезаем по 8бит через побитовое или с 11111111
        while (bits_in_buffer >= 8)
        {
            dst[dst_index++] = (bit_buffer >> (bits_in_buffer - 8)) & 0xFF;

            // Уменьшаем счетчик
            bits_in_buffer -= 8;
        }
    }
// Ключевой принцип: Функция определяет числовое значение каждого входного символа (его индекс от 0 до 31).
// Эти 5-битные значения собираются в буфер, и как только в буфере набирается 8 или более бит, они "откусываются" 
//и записываются как готовый байт в выходной массив dst
    return 0;
}

size_t Base32File::write(const void *buf, size_t n_bytes) {    
    int encoded_size = encoded32_size(n_bytes); // Получаем размер строки после кодирования
    char *encoded_data = new char[encoded_size + 1];
    encoded_data[encoded_size] = '\0'; 
    if (encode32(reinterpret_cast<const char*>(buf), n_bytes, encoded_data) == 0){
        BaseFile::write(encoded_data,encoded_size);
        delete[] encoded_data;
        return encoded_size;        
    }
    else{
        delete[] encoded_data;
        return -1;
    }
}

size_t Base32File::read(void *buf, size_t max_bytes) {
    char *encoded_data = new char[max_bytes];
    int encoded_size = BaseFile::read(encoded_data, max_bytes);
    int decoded_size = decoded32_size(encoded_size);
    if (decode32(encoded_data, encoded_size, reinterpret_cast< char*>(buf)) == 0){
        delete[] encoded_data;
        return decoded_size;        
    }
    else{
        delete[] encoded_data;
        return -1;
    }
}
size_t RleFile::write(const char* data, size_t size) {
        size_t writeCount=0;
        size_t i = 0;
        while (i < size) {// AAAA AA4  AAA AA3 AA AA2 
            char current_char = data[i];
            unsigned short count = 0;

            // Считаем серию одинаковых байтов (макс 255)
            while (i < size && data[i] == current_char && count < 257) {
                count++;
                i++;
            }
            if (count>1){
                // Записываем пару в файл через базовый метод
                write_raw(&current_char, 1);
                write_raw(&current_char, 1);
                count-=2; // два у нас есть 
                write_raw(&count, 1);
                writeCount+=3;
            }
            else{
                // Один символ
                write_raw(&current_char, 1);
                writeCount++;
            }
        }
        return writeCount;
}

// Чтение с распаковкой RLE
size_t RleFile::read(char* buf, size_t size) { // AAAAAAAAAAABC AA9BC
        char* orig_buf = new char[size];
        size_t decoded_size= read_raw(orig_buf, size);
        int i = 1;
        int encoded_size = 0; 
        if (decoded_size==1){
            buf[encoded_size++]=orig_buf[0];
        }
        while (i < decoded_size) {
            if (orig_buf[i-1]==orig_buf[i]){//AA9BC 
                for(int k=0; k<orig_buf[i+1]+2; k++){
                    buf[encoded_size++]=orig_buf[i];
                    if (encoded_size==size){
                        break;
                    }
                }
                i+=2;                
            }
            else{
                buf[encoded_size++]=orig_buf[i-1];
                if (encoded_size==size){
                    break;
                }
            }
            if(i==decoded_size-1) { //Запись последнего байта
                    buf[encoded_size++]=orig_buf[i];
                    if (encoded_size==size){
                        break;
                    }
            }
            i++;
        }
        delete[] orig_buf;
        return encoded_size;
}
void write_int(IFile &file, int n) {
    const char minus = '-';
    if (n < 0) {
        file.write(&minus, 1);
        n = -n;
    }
    if (n == 0) {
        char zero = '0';
        file.write(&zero, 1);
        return;
    }
    int divisor = 1;
    int tn = n;
    while (tn >= 10) {
        tn /= 10;
        divisor *= 10;
    }
    while (divisor > 0) {
        char ch = '0' + (n / divisor);
        file.write(&ch, 1);
        n = n % divisor;
        divisor /= 10;
    }
}
bool BaseFile::can_read() const {
    return readable;
}
bool BaseFile::can_write() const {
    return writable;};
size_t Base32File2::write(const void *buf, size_t n_bytes) {
    if (!target || n_bytes == 0) return 0;

    int encoded_size = encoded32_size(n_bytes); 
    char *encoded_data = new char[encoded_size + 1];
    
    if (encode32(reinterpret_cast<const char*>(buf), n_bytes, encoded_data) == 0) {
        target->write(encoded_data, encoded_size);
        delete[] encoded_data;
        return n_bytes; 
    }
    delete[] encoded_data;
    return 0;
}
size_t Base32File2::read(void *buf, size_t n_bytes) {
    if (!target) return 0;
    return target->read(buf, n_bytes);
}
int Base32File2::encode32(const char *raw_data, int raw_size, char *dst)
{
    // Проверка на корректный ввод
    if (!raw_data || raw_size <= 0 || !dst)
    {
        return 1;
    }

    int bit_buffer = 0;     // Буфер для битов - временной хранилище
    int bits_in_buffer = 0; // Биты в буффере
    int dst_index = 0;      // Индекс в выходном массиве

    // Накапливаем буффер
    for (int i = 0; i < raw_size; i++)
    {

        // Добавляем байт в буфер, сдвиг на 8 бит влево
        bit_buffer = (bit_buffer << 8) | (unsigned char)raw_data[i]; 
        // сдвигаем влево, чтобы освободить место для новых данных
        bits_in_buffer += 8;

        // Пока в буфере >=5 бит, извлекаем группы по 5 бит.
        while (bits_in_buffer >= 5)
        {
            int index = (bit_buffer >> (bits_in_buffer - 5)) & 0x1F; // Извлекаем старшие 5 бит, используя побитовую маску из 5 единиц
            dst[dst_index++] = encode_table[index];                  // Находим соответсвующий символ и записываем его в массив
            bits_in_buffer -= 5;
        }
    }

    // Если после всех байт осталось < 5 бит, добиваем нулями справа до 5 бит.
    if (bits_in_buffer > 0)
    {
        int index = (bit_buffer << (5 - bits_in_buffer)) & 0x1F;
        dst[dst_index++] = encode_table[index];
    }

    return 0;
}
int Base32File2::encoded32_size(int raw_size)
{
    return (raw_size * 8 + 4) / 5; //+4 округляет вверх
}
size_t RleFile2::read(void *buf, size_t n_bytes) {
    if (!target) return 0;
    return target->read(buf, n_bytes);
}
size_t RleFile2::write(const void* buf, size_t n_bytes) {
        size_t writeCount=0;
        size_t i = 0;
        const char* ptr = static_cast<const char*>(buf);
        while (i < n_bytes) {// AAAA AA4  AAA AA3 AA AA2 
            char current_char = ptr[i];
            unsigned short count = 0;

            // Считаем серию одинаковых байтов (макс 255)
            while (i < n_bytes && ptr[i] == current_char && count < 257) {
                count++;
                i++;
            }
            if (count>1){
                // Записываем пару в файл через базовый метод
                write_raw(&current_char, 1);
                write_raw(&current_char, 1);
                count-=2; // два у нас есть 
                write_raw(&count, 1);
                writeCount+=3;
            }
            else{
                // Один символ
                write_raw(&current_char, 1);
                writeCount++;
            }
        }
        return writeCount;
}
size_t RleFile2::write_raw(const void *buf, size_t n_bytes) {
    if (target) {
        return target->write(buf, n_bytes);
    }
    return 0;
}
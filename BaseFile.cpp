#include "BaseFile.hpp"
#include <cstdio>
#include <cstring>
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
void RleFile::write(const char* data, size_t size) {
        size_t i = 0;
        while (i < size) {
            char current_char = data[i];
            unsigned char count = 0;

            // Считаем серию одинаковых байтов (макс 255)
            while (i < size && data[i] == current_char && count < 255) {
                count++;
                i++;
            }

            // Записываем пару в файл через базовый метод
            write_raw((char*)&count, 1);
            write_raw(&current_char, 1);
        }
    }

    // Чтение с распаковкой RLE
    size_t RleFile::read(char* buffer, size_t size) {
        size_t total_read = 0;

        while (total_read < size) {
            // Если предыдущая серия символов закончилась, читаем новую пару из файла
            if (left_count == 0) {
                unsigned char next_count;
                char next_char;

                // Пытаемся прочитать заголовок серии (1 байт количества)
                if (read_raw((char*)&next_count, 1) < 1) break; 
                // Читаем сам символ
                if (read_raw(&next_char, 1) < 1) break;

                left_count = next_count;
                left_char = next_char;
            }

            // Заполняем буфер пользователя текущим символом
            while (left_count > 0 && total_read < size) {
                buffer[total_read++] = left_char;
                left_count--;
            }
        }
        return total_read;
        }

#include "../BaseFile.hpp"
#include <cassert> // системаня библиотека в <>
#include <iostream>
#include <cstring>

using namespace std;
int main(){
    const char* data = "Hello, World!";
    {
        Base32File myFile("test_Base32File.bin", "wb");     
        assert(myFile.is_open() && "is_open error");
        assert(myFile.can_write() && "can_write error");
        assert(!myFile.can_read() && "can_read error");
        size_t written = myFile.write(data, strlen(data));
        assert(written==21 && "written error");
        assert(myFile.tell()==21 && "tell error");
    }

    {
        Base32File myFile("test_Base32File.bin", "rb");
        assert(myFile.is_open() && "is_open error");
        assert(!myFile.can_write() && "can_write error");
        assert(myFile.can_read() && "can_read error");
        assert(myFile.seek(7) && "seek error");
        assert(myFile.tell()==7 && "tell error");
        assert(myFile.seek(0) && "seek error");
        assert(myFile.tell()==0 && "tell error");
        char buffer[30] = {0};
        size_t readCount = myFile.read(buffer, 30);
        assert(readCount==13 && "written error");
        assert(myFile.tell()==21 && "tell error");            
        for (size_t i=0;i<readCount;i++){
            assert(buffer[i]==data[i] && "read buffer error");
        }
    }
    cout << "Base32File tests PASSED" << endl;
    return 0;
}
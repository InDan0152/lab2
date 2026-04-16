#include "../BaseFile.hpp"
#include <cassert> // системаня библиотека в <>
#include <iostream>
#include <cstring>

using namespace std;
int main(){
    const char* data = "Hello, Wooooorld!";
    {
        RleFile2 myFile(new BaseFile("test_RleFile2.bin", "wb"));
        assert(myFile.is_open() && "is_open error");
        assert(myFile.can_write() && "can_write error");
        assert(!myFile.can_read() && "can_read error");
        size_t written = myFile.write(data, strlen(data));        
        assert(written==16 && "written error");
    }

    {
        RleFile2 myFile(new BaseFile("test_RleFile2.bin", "rb"));
        assert(myFile.is_open() && "is_open error");
        assert(!myFile.can_write() && "can_write error");
        assert(myFile.can_read() && "can_read error");
        char buffer[30] = {0};
        size_t readCount = myFile.read(buffer, 30);
        assert(readCount==17 && "written error");
        for (size_t i=0;i<readCount;i++){
            assert(buffer[i]==data[i] && "read buffer error");
        }
    }
    cout << "RleFile2 tests PASSED" << endl;
    return 0;
}
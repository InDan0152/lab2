
#include "Mystring.hpp"
#include <iostream>
#include <cstring>
using namespace std;

MyString::MyString(){
    s=nullptr;
    len=0;
}

MyString::MyString(const MyString& my) {
    len = my.getLen();
    s = new char[len + 1];
    for (int i = 0;i <= len;i++) {
        s[i]=my.get(i);
    }
}

MyString::MyString(const char* str){
    if(str){
        len=strlen(str);
        s = new char[len+1];
        memcpy(s, str, len + 1);
    }
    else{
        MyString();
    }
}

MyString::~MyString(){
    if (s) delete[] s;
}

char MyString::get(int i) const {
    if(i<len) {
      return s[i];
    }
    else{
      return 0;
    }
}

void MyString::set(int i, char c){
    if(i<len) {
      s[i]=c;
    }
}

void MyString::set_new_string(const char *str){
    this->~MyString();
    new (this) MyString(str);
}

void MyString::print(){
    cout << s << endl;
}

void MyString::read_line(){
    int cap = 10;
    int newLen = 0;
    char* buf = new char[cap];
    char c;

    while(cin.get(c) && c != '\n'){
        if(newLen +1 >= cap){
            cap += 10;
            char* newbuf = new char[cap];
            memcpy(newbuf,buf,newLen);
            delete[] buf;
            buf = newbuf;
        }
         buf[newLen++] = c;
    }
    buf[newLen] = '\0';
    if (s){
        delete[] s;
    }
    s=buf;
    len=newLen;
    
}

class MyString {
public:
	MyString();
	MyString(const MyString& my);
	MyString(const char* s);
	~MyString();
	char get(int i) const;
	int getLen() const { return len; };
	void set(int i, char c);
	void set_new_string(const char* str);
	void print();
	void read_line();
private:
	char* s;
	int len;
};
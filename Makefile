CXX = g++
CXXFLAGS = -g -Wall
SRC_DIR = $(CURDIR)
BUILD_DIR = build

# 3.5(Автоматизация)
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
# Превращаем список .cpp в список .o в папке build
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

TEST_DIR = tests
TEST_TARGETS = $(BUILD_DIR)/test_BaseFile.out \
                $(BUILD_DIR)/test_Base32File.out \
                $(BUILD_DIR)/test_Base32File2.out \
                $(BUILD_DIR)/test_RleFile.out \
                $(BUILD_DIR)/test_RleFile2.out
			   
$(BUILD_DIR)/test_BaseFile.out: $(TEST_DIR)/test_BaseFile.cpp $(BUILD_DIR)/BaseFile.o
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/test_Base32File.out: $(TEST_DIR)/test_Base32File.cpp $(BUILD_DIR)/BaseFile.o
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/test_RleFile.out: $(TEST_DIR)/test_RleFile.cpp $(BUILD_DIR)/BaseFile.o
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/test_RleFile2.out: $(TEST_DIR)/test_RleFile2.cpp $(BUILD_DIR)/BaseFile.o
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/test_Base32File2.out: $(TEST_DIR)/test_Base32File2.cpp $(BUILD_DIR)/BaseFile.o
	$(CXX) $(CXXFLAGS) -o $@ $^

# $(BUILD_DIR)/test_rect_operations.out: $(TEST_DIR)/test_rect_operations.cpp $(BUILD_DIR)/rect.o
# 	$(CXX) $(CXXFLAGS) -o $@ $^

# $(BUILD_DIR)/test_bounding_rect.out: $(TEST_DIR)/test_bounding_rect.cpp $(BUILD_DIR)/rect.o
# 	$(CXX) $(CXXFLAGS) -o $@ $^

#$@ — это имя цели, $^ — это все зависимости 
TARGET = $(BUILD_DIR)/debug.out
LEAKS_TARGET = $(BUILD_DIR)/leaks.out

# Правило для build
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

#из .cpp сделать .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Сборка
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

#3.6
# Сборка для утечек
$(LEAKS_TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(LEAKS_TARGET) $(OBJS)

#valgrind и подает файл input.txt на вход
leaks: $(LEAKS_TARGET)
	valgrind --leak-check=full ./$(LEAKS_TARGET) < input.txt

#цели
debug: $(TARGET)
	gdb ./$(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

test: $(TEST_TARGETS)
	@echo "Running test_BaseFile..."
	@./$(BUILD_DIR)/test_BaseFile.out
	@echo "Running test_Base32File..."
	@./$(BUILD_DIR)/test_Base32File.out
	@echo "Running test_RleFile..."
	@./$(BUILD_DIR)/test_RleFile.out
	@echo "Running test_Base32File2..."
	@./$(BUILD_DIR)/test_Base32File2.out
	@echo "Running test_RleFile2..."
	@./$(BUILD_DIR)/test_RleFile2.out

CXX = clang++
CXXFLAGS = -std=c++14 -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS
TARGET = Dlink.out
OBJECTS = main.o Dlink.o Lexer.o ParseStruct.o

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CXX) $(CXXFLAGS) -lLLVM -lOpenCL -o $@ $(OBJECTS)

clean : 
	rm -rf *.o $(TARGET)

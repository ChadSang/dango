all: Debug Debug/internet_engineering

CXXFLAGS:=$(CXXFLAGS) -std=c++11
SOURCES:=$(wildcard internet_engineering/*.cpp)
Debug:
	mkdir -p $@
Debug/internet_engineering: internet_engineering/main.cpp $(SOURCES)
	$(CXX) $(CXXFLAGS) -o$@ $^

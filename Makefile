CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wextra -Wall -Werror -Wno-sign-compare -Wno-unused-result -pedantic
NAME = lab05

all: ukkonen.o
	$(CXX) $(CXXFLAGS) ukkonen.o main.cpp -o $(NAME)
ukkonen.o: ukkonen.cpp ukkonen.hpp
	$(CXX) $(CXXFLAGS) -c ukkonen.cpp
clean:
	rm -f *.o $(NAME)
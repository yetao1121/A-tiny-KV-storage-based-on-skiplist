target=skiplist
lib=Node.hpp skiplist.hpp main.cpp
$(target):$(lib)
	$(CXX) $^ -o ./bin/main -std=c++11

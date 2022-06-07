
target=stress_test
lib=stress_test.cpp ../Node.hpp ../skiplist.hpp
$(target):$(lib)
	$(CXX) -std=c++11 $^ -o $@ -lpthread
clean:
	rm -f stress_test

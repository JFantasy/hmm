CXX = g++ -std=c++11

seg:main.o word_seg.o hmm.o
	$(CXX) $^ -o $@

main.o:main.cpp
	$(CXX) $^ -c

word_seg.o:word_seg.cpp
	$(CXX) $^ -c

hmm.o:hmm.cpp
	$(CXX) $^ -c

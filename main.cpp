#include "word_seg.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
  if (argc < 4) {
    std::cerr << "usage: ./seg <train_file> <input_file> <output_file>" << std::endl;
  } else {
    WordSeg word_seg;
    word_seg.Initialize(std::string(argv[1]));
    std::cout << "Initialize Done!!" << std::endl;

    std::wifstream fin(argv[2]);
    std::wstring str;
    fin.imbue(std::locale(""));
    fin >> str;
    std::vector<std::wstring> seg = word_seg.Segment(str);
    fin.close();

    std::wofstream fout(argv[3]);
    fout.imbue(std::locale(""));
    for (int i = 0; i < seg.size(); ++ i) fout << seg[i] << std::endl;
    fout.close();
  }
  return 0;
}

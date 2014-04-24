#include "word_seg.h"

#include <wchar.h>
#include <string.h>

#include <fstream>
#include <iostream>

WordSeg::WordSeg() {
}

WordSeg::~WordSeg() {
}

void WordSeg::Initialize(const std::string &file_name) {
  ClearAll();
  LoadData(file_name);
  hmm_.LoadPi(GetPi());
  hmm_.LoadIMatrix(GetIMatrix());
  hmm_.LoadOMatrix(GetOMatrix());
}

std::vector<std::wstring> WordSeg::Segment(const std::wstring &sentence) {
  std::vector<int> o_sequence(sentence.length());
  for (int position = 0; position < sentence.length(); ++ position) {
    o_sequence[position] = GetIdByCharacter(sentence[position]);
  }
  std::vector<int> i_sequence = hmm_.Viterbi(o_sequence);
  std::vector<std::wstring> word_seg;
  std::wstring current;
  for (int position = 0; position < sentence.length(); ++ position) {
    current += sentence[position];
    if (i_sequence[position] == kEnd || i_sequence[position] == kSingle) {
      word_seg.push_back(current);
      current = L"";
    }
  }
  if (current.length() > 0) word_seg.push_back(current);
  return word_seg;
}

void WordSeg::LoadData(const std::string &file_name) {
  std::wifstream fin(file_name.c_str());
  fin.imbue(std::locale(""));
  std::wstring line;
  while (getline(fin, line)) Statistic(line);
  fin.close();
}

void WordSeg::Statistic(std::wstring line) {
  std::vector<std::wstring> words;
  const wchar_t seps[] = L"“，。《》! ";
  wchar_t *state;
  wchar_t *token = wcstok(&line[0], seps, &state);
  while (token != NULL) {
    words.push_back(std::wstring(token));
    token = wcstok(NULL, seps, &state);
  }
  for (int index = 0; index < words.size(); ++ index) {
    if (words[index].length() == 1) StatSingle(words[index]);
    else StatWord(words[index]);
    if (index > 1) StatBigram(words[index - 1], words[index]);
  }
}

int WordSeg::GetIdByCharacter(const wchar_t &character) {
  if (wchar2int_.find(character) == wchar2int_.end()) {
    wchar2int_[character] = character_size_;
    int2wchar_[character_size_ ++] = character;
  }
  return wchar2int_[character];
}

void WordSeg::ClearAll() {
  hmm_.Initialize();
  wchar2int_.clear();
  int2wchar_.clear();
  count_tag_.clear();
  count_char_tag_.clear();
  character_size_ = 0;
}

void WordSeg::StatSingle(const std::wstring &word) {
  ++ count_tag_[kSingle];
  ++ count_char_tag_[std::make_pair(GetIdByCharacter(word[0]), kSingle)];
}

void WordSeg::StatWord(const std::wstring &word) {
  std::vector<CharacterTag> tags(word.length(), kMiddle);
  tags[0] = kBegin;
  tags[word.length() - 1] = kEnd;
  for (int position = 0; position < word.length(); ++ position) {
    ++ count_tag_[tags[position]];
    ++ count_char_tag_[std::make_pair(GetIdByCharacter(word[position]), tags[position])];
  }
  StatBigramInWord(word, tags);
}

void WordSeg::StatBigramInWord(const std::wstring &word, 
    const std::vector<CharacterTag> &tags) {
  for (int position = 1; position < word.length(); ++ position) {
    ++ count_bigram_[std::make_pair(tags[position - 1], tags[position])];
  }
}

void WordSeg::StatBigram(const std::wstring &last_word, 
    const std::wstring &current_word) {
  CharacterTag last_tag = last_word.length() == 1 ? kSingle : kEnd;
  CharacterTag current_tag = current_word.length() == 1 ? kSingle : kBegin;
  ++ count_bigram_[std::make_pair(last_tag, current_tag)];
}

std::vector<std::vector<double> > WordSeg::GetIMatrix() {
  std::vector<std::vector<double> > i_matrix = 
    std::vector<std::vector<double> >(kTagsize, std::vector<double>(kTagsize));
  for (int last_tag = kBegin; last_tag < kTagsize; ++ last_tag) {
    for (int current_tag = kBegin; current_tag < kTagsize; ++ current_tag) {
      i_matrix[last_tag][current_tag] = 
        count_bigram_[std::make_pair(static_cast<CharacterTag>(last_tag), 
            static_cast<CharacterTag>(current_tag))]  * 1.0 / 
            count_tag_[static_cast<CharacterTag>(last_tag)];
    }
  }
  return i_matrix;
}

std::vector<std::vector<double> > WordSeg::GetOMatrix() {
  std::vector<std::vector<double> > o_matrix = 
    std::vector<std::vector<double> >(kTagsize, std::vector<double>(character_size_));
  for (int tag = kBegin; tag < kTagsize; ++ tag) {
    for (int character = 0; character < character_size_; ++ character) {
      o_matrix[tag][character] = 
        count_char_tag_[std::make_pair(character, static_cast<CharacterTag>(tag))] 
        * 1.0 / count_tag_[static_cast<CharacterTag>(tag)];
    }
  }
  return o_matrix;
}

std::vector<double> WordSeg::GetPi() {
  std::vector<double> pi = std::vector<double>(kTagsize);
  int sum = count_tag_[kBegin] + count_tag_[kSingle];
  pi[kBegin] = count_tag_[kBegin] * 1.0 / sum;
  pi[kSingle] = count_tag_[kSingle] * 1.0 / sum;
  return pi;
}

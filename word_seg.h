#ifndef __WORD_SEG_H
#define __WORD_SEG_H

#include "hmm.h"

#include <vector>
#include <string>
#include <map>

class WordSeg {
  enum CharacterTag {
    kBegin = 0,
    kEnd = 1,
    kMiddle = 2,
    kSingle = 3
  };

  public:
    WordSeg();
    ~WordSeg();

    void Initialize(const std::string &file_name);
    std::vector<std::wstring> Segment(const std::wstring &sentence);

  private:
    void ClearAll();
    void LoadData(const std::string &file_name);
    void Statistic(std::wstring line);

    std::vector<std::vector<double> > GetIMatrix();
    std::vector<std::vector<double> > GetOMatrix();
    std::vector<double> GetPi();

    void StatSingle(const std::wstring &word);
    void StatWord(const std::wstring &word);
    void StatBigramInWord(const std::wstring &word, 
        const std::vector<CharacterTag> &tags);
    void StatBigram(const std::wstring &last_word, 
        const std::wstring &current_word);

    int GetIdByCharacter(const wchar_t &character);
    
    std::map<wchar_t, int> wchar2int_;
    std::map<int, wchar_t> int2wchar_;
    std::map<CharacterTag, int> count_tag_;
    std::map<std::pair<int, CharacterTag>, int> count_char_tag_;
    std::map<std::pair<CharacterTag, CharacterTag>, int> count_bigram_;
    HMM hmm_;
    int character_size_;

    static const int kTagsize = 4;
};

#endif

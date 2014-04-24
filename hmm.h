#ifndef __HMM_H
#define __HMM_H

#include <vector>

class HMM {
  public:
    HMM();
    ~HMM();

    void Initialize();
    void LoadPi(const std::vector<double> &pi);
    void LoadIMatrix(const std::vector<std::vector<double> > &matrix);
    void LoadOMatrix(const std::vector<std::vector<double> > &matrix);
    std::vector<int> Viterbi(const std::vector<int> &o_sequence);

  private:
    void Process(const std::vector<int> &o_sequence, 
        std::vector<std::vector<double> > &probability,
        std::vector<std::vector<int> > &from);
    std::vector<int> ComputeISequence(
        const std::vector<std::vector<double> > &probability, 
        const std::vector<std::vector<int> > &from);

    int i_node_size_, o_node_size_;
    std::vector<std::vector<double> > i_matrix_, o_matrix_;
    std::vector<double> pi_;

    static const int kNull = -1;
};

#endif

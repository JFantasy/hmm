#include "hmm.h"

#include <algorithm>
#include <iostream>

HMM::HMM() {
}

HMM::~HMM() {
}

void HMM::Initialize() {
  i_node_size_ = o_node_size_ = 0;
  std::vector<std::vector<double> >().swap(i_matrix_);
  std::vector<std::vector<double> >().swap(o_matrix_);
}

void HMM::LoadPi(const std::vector<double> &pi) {
  pi_ = pi;
}

void HMM::LoadIMatrix(const std::vector<std::vector<double> > &matrix) {
  i_node_size_ = matrix.size();
  i_matrix_ = matrix;
}

void HMM::LoadOMatrix(const std::vector<std::vector<double> > &matrix) {
  o_node_size_ = matrix.begin()->size();
  o_matrix_ = matrix;
}

std::vector<int> HMM::Viterbi(const std::vector<int> &o_sequence) {
  std::vector<std::vector<double> > probability(o_sequence.size(), 
      std::vector<double>(o_node_size_));
  std::vector<std::vector<int> > from(o_sequence.size(),
      std::vector<int>(i_node_size_));
  Process(o_sequence, probability, from);
  return ComputeISequence(probability, from);
}

void HMM::Process(const std::vector<int> &o_sequence, 
    std::vector<std::vector<double> > &probability,
    std::vector<std::vector<int> > &from) {
  for (int i_node = 0; i_node < i_node_size_; ++ i_node) {
    probability[0][i_node] = pi_[i_node] * o_matrix_[i_node][o_sequence[0]];
    from[0][i_node] = kNull;
  }
  for (int step = 1; step < o_sequence.size(); ++ step) {
    int o_node = o_sequence[step];
    for (int i_node = 0; i_node < i_node_size_; ++ i_node) {
      for (int last_node = 0; last_node < i_node_size_; ++ last_node) {
        probability[step][i_node] = std::max(probability[step][i_node], 
            probability[step - 1][last_node] * i_matrix_[last_node][i_node] 
            * o_matrix_[i_node][o_node]);
        int &best_node = from[step][i_node];
        if (probability[step - 1][last_node] * i_matrix_[last_node][i_node] >
            probability[step - 1][best_node] * i_matrix_[best_node][i_node]) {
          best_node = last_node;
        }
      }
    }
  }
}

std::vector<int> HMM::ComputeISequence(
    const std::vector<std::vector<double> > &probability, 
    const std::vector<std::vector<int> > &from) {
  std::vector<int> i_sequence(probability.size());
  int step = i_sequence.size() - 1;
  int best_node = 0;
  for (int i_node = 0; i_node < i_node_size_; ++ i_node) {
    if (probability[step][i_node] > probability[step][best_node]) {
      best_node = i_node;
    }
  }
  for ( ; step >= 0; -- step) {
    i_sequence[step] = best_node;
    best_node = from[step][best_node];
  }
  return i_sequence;
}

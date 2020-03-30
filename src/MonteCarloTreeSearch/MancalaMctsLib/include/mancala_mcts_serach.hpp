#ifndef MANCALA_MCTS_SEARCH
#define MANCALA_MCTS_SEARCH

#include <cmath>

#include "util.hpp"
#include "mancala_mcts_node.hpp"

class MonteCarloTreeSearch {
public:
  MonteCarloTreeSearch();
  MonteCarloTreeSearch(std::shared_ptr<MctsNode> root);

  void Search(std::shared_ptr<MctsNode> const & node);
  void Search();

  std::shared_ptr<MctsNode> Select(std::shared_ptr<MctsNode> const & node);
  std::shared_ptr<MctsNode> SelectBestChild(std::shared_ptr<MctsNode> const & node);
  bool ShouldExpand(std::shared_ptr<MctsNode> const & node);
  bool Expand(std::shared_ptr<MctsNode> const & node);
  Result Rollout(std::shared_ptr<MctsNode> const & leaf_node);
  void Backup(std::shared_ptr<MctsNode> const & leaf_node, const Result& result);
  double ComputeUCB1(std::shared_ptr<MctsNode> const & node, int num_all_visited);
  Result Evaluate(std::shared_ptr<State> state);

  // Getter
  std::shared_ptr<MctsNode> root() { return root_; }
  // Setter
  void set_random_seed(int seed) {
    xorshift.set_seed(seed);
  }

private:
  std::shared_ptr<MctsNode> root_;
  const int expand_num_visited_th_ = 5;
  const double ucb_const_c = 0.5;
  static XorShift xorshift;
};

#endif // MANCALA_MCTS_SEARCH
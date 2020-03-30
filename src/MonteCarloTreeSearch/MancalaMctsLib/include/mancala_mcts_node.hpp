#ifndef MANCALA_MCTS_NODE_H
#define MANCALA_MCTS_NODE_H

#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <memory>

#include "mancala_const.hpp"
#include "mancala_action.hpp"
#include "mancala_state.hpp"
#include "mancala_result.hpp"

class MctsNode {
public:
  MctsNode();

  MctsNode(std::shared_ptr<State> state);
  
  static std::shared_ptr<MctsNode> CreateAsRoot(std::shared_ptr<State> state) {
    std::shared_ptr<MctsNode> root = std::make_shared<MctsNode>(state);
    root->set_is_root(true);
    return root;
  }

  void AddChild(std::shared_ptr<MctsNode> child);
  // 勝敗だけ記録
  void Update(const Result& result);
  // 情報を出力
  void Show() const;

  // Getter
  bool is_root() const { return is_root_; }
  bool has_children() const { return has_children_; }
  int num_visited() const { return num_visited_; }
  int num_wins() const { return num_wins_; }
  std::shared_ptr<State> state() const { return state_; }
  const std::vector<std::shared_ptr<MctsNode>>& children() { return children_; }
  std::shared_ptr<MctsNode> parent() const { return parent_.lock(); }
  const Action& prev_action() const { return prev_action_; }

  // Setter
  void set_is_root(bool is_root) { is_root_ = is_root; }
  void set_num_visited(int num_visited) { num_visited_ = num_visited; }
  void set_parent(std::shared_ptr<MctsNode> parent) { parent_ = parent; }
  void set_prev_action(const Action& action) { prev_action_ = action; }

private:
  bool is_root_;
  bool has_children_;
  int num_visited_;
  int num_wins_;
  Action prev_action_;
  
  std::shared_ptr<State> state_;
  std::vector<std::shared_ptr<MctsNode>> children_;
  std::weak_ptr<MctsNode> parent_;
};

#endif // MANCALA_MCTS_NODE_H
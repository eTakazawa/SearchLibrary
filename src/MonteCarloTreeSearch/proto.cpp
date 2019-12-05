#include <bits/stdc++.h>

class Result {
public:
  int win_player;
  double value;
};

class Action {
public:
protected:
};

class State {
public:
  State() {}
  std::shared_ptr<State> GenerateNextState(const Action& action);
  std::shared_ptr<State> GenerateRandomNextState();
  std::vector<std::shared_ptr<State>> GenerateNextStates();
  Result Evaluate();
  Result Evaluate(std::shared_ptr<State> action_player_initial_state);

  // Getter
  int id() const { return id_; }
  int player_id() const { return player_id_; }
  bool is_terminal() const { return is_terminal_; }

protected:
  int id_;
  int player_id_;
  bool is_terminal_;
};

class MctsNode {
public:
  MctsNode(std::shared_ptr<State> state)
      : is_root_(false),
        has_children_(false),
        state_(state) {}

  void AddChild(std::shared_ptr<MctsNode> child) {
    children_.push_back(child);
  }
  void Update(const Result& result) {
    num_wins_ += result.value;
    num_visited_ += 1;
  }

  // Getter
  bool is_root() const { return is_root_; }
  bool has_children() const { return has_children_; }
  int num_visited() const { return num_visited_; }
  int num_wins() const { return num_wins_; }
  std::shared_ptr<State> state() const { return state_; }
  std::vector<std::shared_ptr<MctsNode>> children() const { return children_; }
  std::shared_ptr<MctsNode> parent() const { return parent_.lock(); }

private:
  bool is_root_;
  bool has_children_;
  int num_visited_;
  int num_wins_;
  
  std::shared_ptr<State> state_;
  std::vector<std::shared_ptr<MctsNode>> children_;
  std::weak_ptr<MctsNode> parent_;
};

class MonteCarloTreeSearch {
public:
  MonteCarloTreeSearch(std::shared_ptr<MctsNode> root) : root_(root) {}

  void Search(std::shared_ptr<MctsNode> const & node) {
    std::shared_ptr<MctsNode> leaf_node = Select(node);  // 次のノードの選択を行い，葉ノードを得る
    leaf_node = Expand(leaf_node);  // 展開できる場合は展開
    Result result = Rollout(node);   // 最後までプレイし結果を取得する
    Backup(node, result);
  }
  void Search() { Search(root_); }
  std::shared_ptr<MctsNode> Select(std::shared_ptr<MctsNode> const & node) {
    std::shared_ptr<MctsNode> now_node = node;
    while (now_node->has_children()) {
      now_node = SelectBestChild(now_node);  // 次のノードの選択を行い，探索を進める
    }
    return now_node;
  }

  std::shared_ptr<MctsNode> SelectBestChild(std::shared_ptr<MctsNode> const & node) {
    double best_value = -1.0;
    std::shared_ptr<MctsNode> best_child;
    for (auto child : node->children()) {
      // TODO 毎回計算する必要はない？（ノードの更新時のみ）
      double value = ComputeUCB1(child);
      if (value > best_value) {
        best_value = value;
        best_child = child;
      }
    }
    return best_child;
  }
  bool ShouldExpand(std::shared_ptr<MctsNode> const & node) {
    return (node->num_visited() > expand_num_visited_th_);
  }
  std::shared_ptr<MctsNode> Expand(std::shared_ptr<MctsNode> const & node) {
    if (!ShouldExpand(node)) return node;
    const std::shared_ptr<State> now_state = node->state();
    std::vector<std::shared_ptr<State>> next_states = now_state->GenerateNextStates();
    for (std::shared_ptr<State> next_state : next_states) {
      node->AddChild(std::make_shared<MctsNode>(next_state));
    }
    return node;
  }
  Result Rollout(std::shared_ptr<MctsNode> const & leaf_node) {
    std::shared_ptr<State> state = leaf_node->state();
    while (!state->is_terminal()) {
      state = state->GenerateRandomNextState();
    }
    return state->Evaluate(leaf_node->state());
  }
  void Backup(std::shared_ptr<MctsNode> const & leaf_node, const Result& result) {
    std::shared_ptr<MctsNode> node = leaf_node;
    while (!node->is_root()) {
      node->Update(result);
      node = node->parent();
    }
  }
  double ComputeUCB1(std::shared_ptr<MctsNode> const & node) {
    double ucb = 
      ((double)node->num_wins() / node->num_visited()) +
      ucb_const_c * 
      sqrt(log(node->parent()->num_visited()) / node->num_visited());
    return ucb;
  }

private:
  std::shared_ptr<MctsNode> root_;
  int expand_num_visited_th_;
  const double ucb_const_c = 0.5;
};

int main(void) {

  return 0;
}

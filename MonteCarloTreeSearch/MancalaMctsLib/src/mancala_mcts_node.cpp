#include "mancala_mcts_node.hpp"

MctsNode::MctsNode()
    : is_root_(true),
      has_children_(false),
      num_visited_(0),
      num_wins_(0),
      prev_action_(MancalaConst::NUM_CELLS) {
  state_ = std::make_shared<State>();
}

MctsNode::MctsNode(std::shared_ptr<State> state)
    : is_root_(false),
      has_children_(false),
      num_visited_(0),
      num_wins_(0),
      prev_action_(MancalaConst::NUM_CELLS),
      state_(state) {}

void MctsNode::AddChild(std::shared_ptr<MctsNode> child) {
  has_children_ = true;
  children_.push_back(child);
}

// 勝敗だけ記録
void MctsNode::Update(const Result& result) {
  // 親目線での勝率
  if (!is_root_) {
    int parent_player_id = parent_.lock()->state()->player_id();
    num_wins_ += (result.win_player == parent_player_id);
  }
  num_visited_ += 1;
}

// 情報を出力
void MctsNode::Show() const {
  printf("====================================\n");
  printf(
      "root:%d, num_children: %2d, prev_action: %d, [%2d / %2d]\n",
      is_root_,
      (int)children_.size(),
      prev_action_.selected_cell,
      num_wins_,
      num_visited_);
  state_->Show();
  printf("====================================\n");
}
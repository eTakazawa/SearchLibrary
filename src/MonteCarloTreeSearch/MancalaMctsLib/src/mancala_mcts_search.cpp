
#include "mancala_mcts_serach.hpp"


MonteCarloTreeSearch::MonteCarloTreeSearch() {
  root_ = std::make_shared<MctsNode>();
}
MonteCarloTreeSearch::MonteCarloTreeSearch(std::shared_ptr<MctsNode> root) :root_(root) {}

void MonteCarloTreeSearch::Search(std::shared_ptr<MctsNode> const & node) {
  std::shared_ptr<MctsNode> leaf_node = Select(node);  // 次のノードの選択を行い，葉ノードを得る
  bool is_expanded = Expand(leaf_node);  // 展開できる場合は展開
  if (is_expanded) leaf_node = Select(leaf_node);
  Result result = Rollout(leaf_node);   // 最後までプレイし結果を取得する
  Backup(leaf_node, result);
}

void MonteCarloTreeSearch::Search() { Search(root_); }

std::shared_ptr<MctsNode> MonteCarloTreeSearch::Select(std::shared_ptr<MctsNode> const & node) {
  std::shared_ptr<MctsNode> now_node = node;
  while (now_node->has_children()) {
    now_node = SelectBestChild(now_node);  // 次のノードの選択を行い，探索を進める
  }
  return now_node;
}

std::shared_ptr<MctsNode> MonteCarloTreeSearch::SelectBestChild(std::shared_ptr<MctsNode> const & node) {
  int num_all_visited = 0;
  for (const auto& child : node->children()) {
    num_all_visited += child->num_visited();
  }
  // ucb value最大のノードを選択
  double best_value = -std::numeric_limits<double>::max();
  std::vector<std::shared_ptr<MctsNode>> best_children;
  for (const auto& child : node->children()) {
    // TODO 毎回計算する必要はない（ノードの更新時のみ）
    double value = ComputeUCB1(child, num_all_visited);
    if (best_value == std::numeric_limits<double>::max() ||
        value > best_value) {
      best_value = value;
      best_children.push_back(child);
    }
  }
  return best_children.at(xorshift.rand() % best_children.size());
}

bool MonteCarloTreeSearch::ShouldExpand(std::shared_ptr<MctsNode> const & node) {
  if (node->has_children()) return false;
  if (node->is_root() && !node->has_children()) return true;
  return (node->num_visited() > expand_num_visited_th_);
}

bool MonteCarloTreeSearch::Expand(std::shared_ptr<MctsNode> const & node) {
  if (!ShouldExpand(node)) return false;
  assert(node->has_children() == 0);
  const std::shared_ptr<State> now_state = node->state();
  const auto& actions = now_state->GetAllAvailableActions();
  std::vector<std::shared_ptr<State>> next_states = now_state->GenerateNextStates(actions);
  for (const auto& action : actions) {
    auto child_node = std::make_shared<MctsNode>(now_state->GenerateNextState(action));
    node->AddChild(child_node);
    child_node->set_parent(node);
    child_node->set_prev_action(action);
  }
  return true;
}

Result MonteCarloTreeSearch::Rollout(std::shared_ptr<MctsNode> const & leaf_node) {
  assert(!leaf_node->has_children());
  std::shared_ptr<State> state = std::make_shared<State>(*leaf_node->state().get());
  while (!state->is_terminate()) {
    state->ActRandomly();
  }
  return Evaluate(state);
}

void MonteCarloTreeSearch::Backup(std::shared_ptr<MctsNode> const & leaf_node, const Result& result) {
  assert(leaf_node);
  std::shared_ptr<MctsNode> node = leaf_node;
  while (!node->is_root()) {
    node->Update(result);
    node = node->parent();
  }
  // rootノードのUpdate
  node->Update(result);
}

double MonteCarloTreeSearch::ComputeUCB1(std::shared_ptr<MctsNode> const & node, int num_all_visited) {
  if (node->num_visited() == 0) {
    return std::numeric_limits<double>::max();
  }
  assert(num_all_visited >= 1);
  double ucb = 
    ((double)node->num_wins() / node->num_visited()) +  // 評価値が高いものを優先
    ucb_const_c * 
    sqrt(log(num_all_visited) / node->num_visited());  // 未選択手を優先
  return ucb;
}

Result MonteCarloTreeSearch::Evaluate(std::shared_ptr<State> state) {
  assert(state->is_terminate());  // 必ず終了後に渡されなければならない
  if (state->game_state() == State::GameState::kAWin) {
    return Result(0);
  }
  if (state->game_state() == State::GameState::kALose) {
    return Result(1);
  }
  assert(0);
}

XorShift MonteCarloTreeSearch::xorshift(0);

#include <string>
#include <iostream>
#include <vector>
#include <limits>
#include <random>
#include <memory>
#include <cassert>

class XorShift {
public:
  XorShift(unsigned int seed = 123456789) {
    x = seed;
    y = 362436069;
    z = 521288629;
    w = 88675123;
  }
  unsigned int rand() {
    unsigned int t;
    t=(x^(x<<11));x=y;y=z;z=w;return( w=(w^(w>>19))^(t^(t>>8)) );
  }
  void set_seed(int seed) {
    x = seed;
  }
private:
  unsigned int x, y, z, w;
};

class MancalaConst {
public:
  static constexpr int NUM_CELLS = 8;  // 必ず偶数
  static constexpr int MIDDLE_CELL_A = NUM_CELLS / 2 - 1;
  static constexpr int MIDDLE_CELL_B = NUM_CELLS - 1;
  static constexpr int NUM_GOAL_EMPTY_CELL = NUM_CELLS / 2 - 1;
  static constexpr int NUM_CELLS_EACH_PLAYER = (NUM_CELLS - 2) / 2;
  static constexpr int NUM_FIRST_TOKENS = 3;
};

class Result {
public:
  Result(int w)
      : win_player(w) {};
  int win_player;
};

class Action {
public:
  Action(int selected_cell)
      : selected_cell(selected_cell) {};
  int selected_cell;
};

std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  std::string item;
  for (char ch: s) {
    if (ch == delim) {
      if (!item.empty())
        elems.push_back(item);
        item.clear();
    }
    else {
      item += ch;
    }
  }
  if (!item.empty())
    elems.push_back(item);
  return elems;
}

class State {
public:
  enum GameState {
    kContinue,
    kAWin,
    kALose,
    kDraw
  };

  State() {
    player_id_ = 0;
    game_state_ = kContinue;
    for (int i = 0; i < MancalaConst::NUM_CELLS; i++) {
      cells_.at(i) = MancalaConst::NUM_FIRST_TOKENS;
    }
    cells_.at(MancalaConst::MIDDLE_CELL_A) = 0;
    cells_.at(MancalaConst::MIDDLE_CELL_B) = 0;
  }

  void SetCells(std::string payload) {
    const auto& strs = split(payload, ',');
    assert(strs.size() == MancalaConst::NUM_CELLS + 1);
    for (int i = 0; i < MancalaConst::NUM_CELLS; i++) {
      cells_.at(i) = std::stoi(strs[i]);
    }
    player_id_ = std::stoi(strs[MancalaConst::NUM_CELLS]);
    game_state_ = kContinue;
  }

  // 取れる全ての手を返す
  std::vector<Action> GetAllAvailableActions() {
    int start, end;
    if (player_id_ == 0) {
      start = 0;
      end = MancalaConst::MIDDLE_CELL_A;
    }
    else if (player_id_ == 1) {
      start = MancalaConst::MIDDLE_CELL_A + 1;
      end = MancalaConst::MIDDLE_CELL_B;
    }
    else {
      assert(0);
    }
    std::vector<Action> actions;
    for (int i = start; i < end; i++) {
      if (cells_.at(i) > 0) actions.push_back(i);
    }
    return actions;
  }
  // ゲーム状態を取得する
  GameState GetGameState() {
    int zero_count;
    zero_count = 0;
    for (int i = 0; i < MancalaConst::MIDDLE_CELL_A; i++) {
      if (cells_.at(i) == 0) zero_count++;
    }
    if (zero_count == MancalaConst::NUM_GOAL_EMPTY_CELL) return kAWin;
    zero_count = 0;
    for (int i = MancalaConst::MIDDLE_CELL_A + 1; i < MancalaConst::MIDDLE_CELL_B; i++) {
      if (cells_.at(i) == 0) zero_count++;
    }
    if (zero_count == MancalaConst::NUM_GOAL_EMPTY_CELL) return kALose;
    return kContinue;
  }
  // actionを行い，状態を変化させる
  bool Act(const Action& action) {
    // TODO invalid moveの判定
    // 駒の移動
    int cell = action.selected_cell;
    int num_tokens = cells().at(cell);
    cells().at(cell) = 0;
    for (int i = 0; i < num_tokens; i++) {
      int moved = (cell + i + 1) % MancalaConst::NUM_CELLS;
      cells().at(moved)++;
    }
    // プレイヤの入れ替え
    int end_cell = (cell + num_tokens) % MancalaConst::NUM_CELLS;
    if (end_cell != MancalaConst::MIDDLE_CELL_A &&
        end_cell != MancalaConst::MIDDLE_CELL_B) {
      set_player_id(player_id_ ^ 1);
    }
    // 勝敗判定
    game_state_ = GetGameState();
    return true;
  }
  // ランダムに一手動かす
  bool ActRandomly() {
    const auto& actions = GetAllAvailableActions();
    int id = xorshift.rand() % actions.size();
    return Act(actions.at(id));
  }
  // 次の状態を返す
  std::shared_ptr<State> GenerateNextState(const Action& action) {
    std::shared_ptr<State> next_state = std::make_shared<State>(*this);
    next_state->Act(action);
    return next_state;
  }
  // 次の状態を連続で返す
  std::vector<std::shared_ptr<State>> GenerateNextStates(const std::vector<Action>& actions) {
    std::vector<std::shared_ptr<State>> next_states;
    for (const auto& action : actions) {
      next_states.push_back(GenerateNextState(action));
    }
    return next_states;
  }
  // 次に取りうる全ての状態を返す
  std::vector<std::shared_ptr<State>> GenerateAllNextStates() {
    std::vector<std::shared_ptr<State>> next_states;
    const auto& actions = GetAllAvailableActions();
    for (const auto& action : actions) {
      next_states.push_back(GenerateNextState(action));
    }
    return next_states;
  }
  // 状態の表示
  void Show() {
    std::string ub = "------------------------------------\n";
    std::cout << ub;
    printf("player_id: %d, GameState: %d, end %d\n", player_id_, game_state_, is_terminate());

    for (int i = 0; i < MancalaConst::NUM_CELLS_EACH_PLAYER; i++) printf("***");
    printf("********\n");

    // 後攻プレイヤーのセル
    printf("    ");
    for (int i = MancalaConst::MIDDLE_CELL_B - 1; i > MancalaConst::MIDDLE_CELL_A; i--) {
      printf("%2d ", cells_.at(i));
    }
    printf("\n");

    // 真ん中のセル
    printf(" %2d ", cells_.at(MancalaConst::MIDDLE_CELL_B));
    for (int i = 0; i < MancalaConst::NUM_CELLS_EACH_PLAYER; i++) printf("   ");
    printf(" %2d ", cells_.at(MancalaConst::MIDDLE_CELL_A));
    printf("\n");

    // 先攻プレイヤーのセル
    printf("    ");
    for (int i = 0; i < MancalaConst::MIDDLE_CELL_A; i++) {
      printf("%2d ", cells_.at(i));
    }
    printf("\n");

    for (int i = 0; i < MancalaConst::NUM_CELLS_EACH_PLAYER; i++) printf("***");
    printf("********\n");
    std::cout << ub;
  }

  // Getter
  int id() const { return id_; }
  int player_id() const { return player_id_; }
  bool is_terminate() const { return game_state_ != kContinue; }
  State::GameState game_state() const { return game_state_; }
  // Setter
  void set_player_id(int id) { player_id_ = id; }
  // Accessor
  std::array<int, MancalaConst::NUM_CELLS>& cells() { return cells_; }

protected:
  int id_;
  int player_id_;
  GameState game_state_;
  static XorShift xorshift;

  /**
   * NUM_CELLSが8の場合
   *   6 5 4
   * 7       3
   *   0 1 2 
   **/
  std::array<int, MancalaConst::NUM_CELLS> cells_;
};
XorShift State::xorshift(1);

void TestStateAct() {
  State state;
  state.Show();
  for (int i = 0; i < 10; i++) {
    state.ActRandomly();
    state.Show();
  }
}
void TestStateGenerateAllNextStates() {
  State state;
  state.Show();
  for (const auto& next : state.GenerateAllNextStates()) {
    // next->Show();
    for (const auto& next_next : next->GenerateAllNextStates()) {
      next_next->Show();
    }
  }
}

class MctsNode {
public:
  MctsNode()
      : is_root_(true),
        has_children_(false),
        is_expanded_(false),
        num_visited_(0),
        num_wins_(0),
        prev_action_(MancalaConst::NUM_CELLS) {
    state_ = std::make_shared<State>();
  }

  MctsNode(std::shared_ptr<State> state)
      : is_root_(false),
        has_children_(false),
        is_expanded_(false),
        num_visited_(0),
        num_wins_(0),
        prev_action_(MancalaConst::NUM_CELLS),
        state_(state) {}
  
  static std::shared_ptr<MctsNode> CreateAsRoot(std::shared_ptr<State> state) {
    std::shared_ptr<MctsNode> root = std::make_shared<MctsNode>(state);
    root->set_is_root(true);
    return root;
  }

  void AddChild(std::shared_ptr<MctsNode> child) {
    has_children_ = true;
    children_.push_back(child);
  }
  // 勝敗だけ記録
  void Update(const Result& result) {
    // 親目線での勝率
    if (!is_root_) {
      int parent_player_id = parent_.lock()->state()->player_id();
      num_wins_ += (result.win_player == parent_player_id);
    }
    num_visited_ += 1;
  }

  // 情報を出力
  void Show() const {
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
  bool is_expanded_;
  int num_visited_;
  int num_wins_;
  Action prev_action_;
  
  std::shared_ptr<State> state_;
  std::vector<std::shared_ptr<MctsNode>> children_;
  std::weak_ptr<MctsNode> parent_;
};

class MonteCarloTreeSearch {
public:
  MonteCarloTreeSearch() {
    root_ = std::make_shared<MctsNode>();
  }
  MonteCarloTreeSearch(std::shared_ptr<MctsNode> root) :root_(root) {}

  void Search(std::shared_ptr<MctsNode> const & node) {
    std::shared_ptr<MctsNode> leaf_node = Select(node);  // 次のノードの選択を行い，葉ノードを得る
    bool is_expanded = Expand(leaf_node);  // 展開できる場合は展開
    if (is_expanded) leaf_node = Select(leaf_node);
    Result result = Rollout(leaf_node);   // 最後までプレイし結果を取得する
    Backup(leaf_node, result);
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
  bool ShouldExpand(std::shared_ptr<MctsNode> const & node) {
    if (node->has_children()) return false;
    if (node->is_root() && !node->has_children()) return true;
    return (node->num_visited() > expand_num_visited_th_);
  }
  bool Expand(std::shared_ptr<MctsNode> const & node) {
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
  Result Rollout(std::shared_ptr<MctsNode> const & leaf_node) {
    assert(!leaf_node->has_children());
    std::shared_ptr<State> state = std::make_shared<State>(*leaf_node->state().get());
    while (!state->is_terminate()) {
      state->ActRandomly();
    }
    return Evaluate(state);
  }
  void Backup(std::shared_ptr<MctsNode> const & leaf_node, const Result& result) {
    assert(leaf_node);
    std::shared_ptr<MctsNode> node = leaf_node;
    while (!node->is_root()) {
      node->Update(result);
      node = node->parent();
    }
    // rootノードのUpdate
    node->Update(result);
  }
  double ComputeUCB1(std::shared_ptr<MctsNode> const & node, int num_all_visited) {
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
  Result Evaluate(std::shared_ptr<State> state) {
    assert(state->is_terminate());  // 必ず終了後に渡されなければならない
    if (state->game_state() == State::GameState::kAWin) {
      return Result(0);
    }
    if (state->game_state() == State::GameState::kALose) {
      return Result(1);
    }
    assert(0);
  }

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
XorShift MonteCarloTreeSearch::xorshift(1);

void TestMctsFunc() {
  std::shared_ptr<MctsNode> root = std::make_shared<MctsNode>();
  MonteCarloTreeSearch mcts(root);
  mcts.root()->set_num_visited(10);

  printf("\nExpand\n");
  mcts.Expand(mcts.root());
  mcts.root()->Show();
  for (const auto& child : mcts.root()->children()) {
    child->Show();
  }

  printf("\nSelect\n");
  const auto& leaf = mcts.Select(mcts.root());
  leaf->Show();

  printf("\nRollout\n");
  const auto& result = mcts.Rollout(leaf);

  printf("BackUp\n");
  mcts.Backup(leaf, result);
  mcts.root()->Show();
  for (const auto& child : mcts.root()->children()) {
    child->Show();
  }

  printf("\nSearch\n");
  mcts.Search();
  mcts.root()->Show();
  for (const auto& child : mcts.root()->children()) {
    child->Show();
  }
}
void TestMctsUpdate() {
  std::shared_ptr<MctsNode> root = std::make_shared<MctsNode>();
  MonteCarloTreeSearch mcts(root);
  const auto& result = mcts.Rollout(mcts.root());
  printf("win_player: %d\n", result.win_player);
  mcts.root()->Show();
  mcts.root()->Update(result);
  mcts.root()->Show();
}
void TestMctsSearch() {
  std::shared_ptr<MctsNode> root = std::make_shared<MctsNode>();
  MonteCarloTreeSearch mcts(root);
  for (int t = 0; t < 1000; t++) {
    mcts.Search();
  }
  mcts.root()->Show();
  for (const auto& child : mcts.root()->children()) {
    child->Show();
  }
}

void vsCPU() {
  std::shared_ptr<State> state = std::make_shared<State>();
  while (!state->is_terminate()) {
    if (state->player_id() == 1) {
      std::cout << "*********************************" << std::endl;
      std::cout << "Your Turn!" << std::endl;
      std::cout << "Please input: [0-2]" << std::endl;
      int selected_cell;
      std::cin >> selected_cell;
      state->Act(Action(selected_cell));
      state->Show();
      std::cout << "*********************************\n" << std::endl;
    }
    else {
      std::cout << "*********************************" << std::endl;
      std::cout << "Opponent Turn!" << std::endl;
      MonteCarloTreeSearch mcts(MctsNode::CreateAsRoot(state));
      for (int t = 0; t < 10000; t++) mcts.Search();
      double max_win = -1.0;
      Action max_action = MancalaConst::NUM_CELLS;
      for (const auto& child : mcts.root()->children()) {
        double win_ratio = 1.0 * child->num_wins() / child->num_visited();
        if (max_win < win_ratio) {
          max_win = win_ratio;
          max_action = child->prev_action();
        }
        printf("%d %.2f%%\n", child->prev_action().selected_cell, win_ratio * 100);
      }
      std::cout << "Act " << max_action.selected_cell << std::endl;
      state->Act(max_action);
      state->Show();
      std::cout << "*********************************\n" << std::endl;
    }
  }
  std::cout << "!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
  std::cout << "You " << (state->game_state() == State::GameState::kAWin ? "Win" : "Lose") << std::endl;
  std::cout << "!!!!!!!!!!!!!!!!!!!!!!!\n" << std::endl;
}

int main(void) {

  std::shared_ptr<State> state = std::make_shared<State>();
  state->SetCells("3,3,0,1,4,4,3,0,0");
  state->Show();
  // MonteCarloTreeSearch mcts(MctsNode::CreateAsRoot(state));
  // for (int t = 0; t < 10000; t++) mcts.Search();
  // TestStateAct();
  // TestStateGenerateAllNextStates();
  // TestMctsUpdate();
  // TestMctsFunc();
  // TestMctsSearch();
  // vsCPU();
  return 0;
}

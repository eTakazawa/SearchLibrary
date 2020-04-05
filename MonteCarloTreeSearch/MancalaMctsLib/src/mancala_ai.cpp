#include "mancala_ai.hpp"
#include <cstdio>

int GetBestAction(int player_id, const std::vector<int>& cells, int simulateNum) {
  
  assert(cells.size() == MancalaConst::NUM_CELLS);
  // 初期状態の生成
  std::shared_ptr<State> state = std::make_shared<State>(player_id, cells);

  if (state->GetGameState() != State::GameState::kContinue) return ERROR_ALREADY_END_GAME;

  // 初期状態をルートとしてMCTS
  MonteCarloTreeSearch mcts(MctsNode::CreateAsRoot(state));
  for (int t = 0; t < simulateNum; t++) mcts.Search();

  // 評価値最大のものを得る
  double max_win = -1.0;
  Action max_action = MancalaConst::NUM_CELLS;
  for (const auto& child : mcts.root()->children()) {
    double win_ratio = 1.0 * child->num_wins() / child->num_visited();
    if (max_win < win_ratio) {
      max_win = win_ratio;
      max_action = child->prev_action();
    }
#ifdef _DEBUG
    fprintf(stderr, "%d %.2f%%\n", child->prev_action().selected_cell, win_ratio * 100);
#endif
  }
  // state->Act(max_action.selected_cell);

  return max_action.selected_cell;
}

int GetGameState(int player_id, const std::vector<int>& cells, int simulateNum) {
  assert(cells.size() == MancalaConst::NUM_CELLS);
  // 初期状態の生成
  std::shared_ptr<State> state = std::make_shared<State>(player_id, cells);
  return state->GetGameState();
}
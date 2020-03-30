
#include <cassert>
#include <iostream>

#include "mancala_const.hpp"
#include "mancala_state.hpp"
#include "mancala_action.hpp"
#include "util.hpp"

State::State() {
  player_id_ = 0;
  game_state_ = State::kContinue;
  for (int i = 0; i < MancalaConst::NUM_CELLS; i++) {
    cells_.at(i) = MancalaConst::NUM_FIRST_TOKENS;
  }
  cells_.at(MancalaConst::MIDDLE_CELL_A) = 0;
  cells_.at(MancalaConst::MIDDLE_CELL_B) = 0;
}

State::State(int player_id, const std::vector<int>& cells) {
  player_id_ = player_id;
  game_state_ = State::kContinue;
  assert(MancalaConst::NUM_CELLS == cells.size());
  for (int i = 0; i < MancalaConst::NUM_CELLS; i++) {
    cells_.at(i) = cells[i];
  }
}

void State::SetCells(std::string payload) {
  const auto& strs = mancala::split(payload, ',');
  assert(strs.size() == MancalaConst::NUM_CELLS + 1);
  for (int i = 0; i < MancalaConst::NUM_CELLS; i++) {
    cells_.at(i) = std::stoi(strs[i]);
  }
  player_id_ = std::stoi(strs[MancalaConst::NUM_CELLS]);
  game_state_ = kContinue;
}

// 取れる全ての手を返す
std::vector<Action> State::GetAllAvailableActions() {
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
State::GameState State::GetGameState() {
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
bool State::Act(const Action& action) {
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
bool State::ActRandomly() {
  const auto& actions = GetAllAvailableActions();
  int id = xorshift.rand() % actions.size();
  return Act(actions.at(id));
}

// 次の状態を返す
std::shared_ptr<State> State::GenerateNextState(const Action& action) {
  std::shared_ptr<State> next_state = std::make_shared<State>(*this);
  next_state->Act(action);
  return next_state;
}

// 次の状態を連続で返す
std::vector<std::shared_ptr<State>> State::GenerateNextStates(const std::vector<Action>& actions) {
  std::vector<std::shared_ptr<State>> next_states;
  for (const auto& action : actions) {
    next_states.push_back(GenerateNextState(action));
  }
  return next_states;
}

// 次に取りうる全ての状態を返す
std::vector<std::shared_ptr<State>> State::GenerateAllNextStates() {
  std::vector<std::shared_ptr<State>> next_states;
  const auto& actions = GetAllAvailableActions();
  for (const auto& action : actions) {
    next_states.push_back(GenerateNextState(action));
  }
  return next_states;
}

// 状態の表示
void State::Show() {
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

XorShift State::xorshift(0);
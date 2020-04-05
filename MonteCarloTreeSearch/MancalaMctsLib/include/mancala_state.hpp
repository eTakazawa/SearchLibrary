#ifndef MANCALA_STATE_H
#define MANCALA_STATE_H
#include <vector>
#include <string>
#include <memory>
#include <array>

#include "util.hpp"
#include "mancala_action.hpp"

class State {
public:
  enum GameState {
    kContinue,
    kAWin,
    kALose,
    kDraw
  };

  State();
  State(int player_id, const std::vector<int>& cells);

  void SetCells(std::string payload);
  // 取れる全ての手を返す
  std::vector<Action> GetAllAvailableActions();
  // ゲーム状態を取得する
  GameState GetGameState();
  // actionを行い，状態を変化させる
  bool Act(const Action& action);
  // ランダムに一手動かす
  bool ActRandomly();
  // 次の状態を返す
  std::shared_ptr<State> GenerateNextState(const Action& action);
  // 次の状態を連続で返す
  std::vector<std::shared_ptr<State>> GenerateNextStates(const std::vector<Action>& actions);
  // 次に取りうる全ての状態を返す
  std::vector<std::shared_ptr<State>> GenerateAllNextStates();
  // 状態の表示
  void Show();

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

#endif // MANCALA_STATE_H
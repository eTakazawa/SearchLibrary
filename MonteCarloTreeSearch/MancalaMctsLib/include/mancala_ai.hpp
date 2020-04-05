#ifndef MANCALA_AI_H
#define MANCALA_AI_H

#include <string>
#include <iostream>
#include <vector>
#include <limits>
#include <random>
#include <memory>
#include <cassert>

#include "mancala_const.hpp"
#include "mancala_mcts_serach.hpp"

enum {
  ERROR_ALREADY_END_GAME = -1,
};

// 最善手を返す
int GetBestAction(int player_id, const std::vector<int>& cells, int simulateNum);

// ゲーム状態を返す
int GetGameState(int player_id, const std::vector<int>& cells);

#endif // MANCALA_AI_H
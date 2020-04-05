#ifndef MANCALA_CONST_H
#define MANCALA_CONST_H

class MancalaConst {
public:
  static constexpr int NUM_CELLS = 8;  // 必ず偶数
  static constexpr int MIDDLE_CELL_A = NUM_CELLS / 2 - 1;
  static constexpr int MIDDLE_CELL_B = NUM_CELLS - 1;
  static constexpr int NUM_GOAL_EMPTY_CELL = NUM_CELLS / 2 - 1;
  static constexpr int NUM_CELLS_EACH_PLAYER = (NUM_CELLS - 2) / 2;
  static constexpr int NUM_FIRST_TOKENS = 3;
};

#endif // MANCALA_CONST_H
#include <iostream>
#include <vector>
#include <tuple>
#include <cassert>

constexpr int BOARD_HW = 3;
enum GameResult {
  DRAW,
  WIN,
  LOSE,
  DOING
};
enum OX {
  E,
  O,
  X
};

/**
 * ゲームが終了したか返す． 
 * 終了している場合，勝敗を返す
 * (BOARD_HWが変数になっているが，対応しておらず)
 **/
GameResult IsEnd(OX board[BOARD_HW][BOARD_HW]) {
  for (int i = 0; i < BOARD_HW; i++) {
    /* 縦に揃う */
    if (board[i][0] == board[i][1] && board[i][1] == board[i][2]) {
      if (board[i][0] == O) return WIN;
      if (board[i][0] == X) return LOSE;
    }
    /* 横に揃う */
    if (board[0][i] == board[1][i] && board[1][i] == board[2][i]) {
      if (board[0][i] == O) return WIN;
      if (board[0][i] == X) return LOSE;
    }
  }
  /* 斜めに揃う */
  if (board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
    if (board[0][0] == O) return WIN;
    if (board[0][0] == X) return LOSE;
  }
  if (board[2][0] == board[1][1] && board[1][1] == board[0][2]) {
    if (board[2][0] == O) return WIN;
    if (board[2][0] == X) return LOSE;
  }
  /* 空きマスがまだあれば続行 */
  for (int i = 0; i < BOARD_HW; i++) {
    for (int j = 0; j < BOARD_HW; j++) {
      if (board[i][j] == E) return DOING;
    }
  }
  /* 全部埋まっているので引き分け */
  return DRAW;
}
/**
 * 空いているマスを返す．(r,c) 
 **/
std::vector<std::pair<int,int>> GetEmptyCell(OX board[BOARD_HW][BOARD_HW]) {
  std::vector<std::pair<int,int>> empty_cells;
  for (int i = 0; i < BOARD_HW; i++) {
    for (int j = 0; j < BOARD_HW; j++) {
      if (board[i][j] == E) empty_cells.emplace_back(i, j);
    }
  }
  return empty_cells;
}

/**
 * ランダムに手を選んでboardを埋める 
 **/
bool PutOnBoardRandomly(OX ox, OX board[BOARD_HW][BOARD_HW], const std::vector<std::pair<int,int>>& empty_cells) {
  int id = rand() % empty_cells.size();
  int r, c;
  std::tie(r, c) = empty_cells[id];
  if (board[r][c] != E) return false;
  board[r][c] = ox;
  return true;
}

/**
 * boardの描画
 **/
void ShowBoard(OX board[BOARD_HW][BOARD_HW]) {
  for (int i = 0; i < BOARD_HW; i++) {
    for (int j = 0; j < BOARD_HW; j++) {
      char c = '.';
      if (board[i][j] == O) c = 'O';
      if (board[i][j] == X) c = 'X';
      std::cout << c;
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

void SimulateUntilEnd(
    int visit_count[BOARD_HW * BOARD_HW],
    int win_count[BOARD_HW * BOARD_HW]) {

  int turn = 0;
  OX board[BOARD_HW][BOARD_HW] = {};

  /* 予め一手打っておく */
  int x = rand() % (BOARD_HW * BOARD_HW);
  int r = x / 3, c = x % 3;
  
  visit_count[x]++;
  board[r][c] = O;

  /* ゲーム終了までランダムに打つ */
  GameResult result;
  while ((result = IsEnd(board)) == DOING) {
    turn++;
    std::vector<std::pair<int,int>> empty_cells = GetEmptyCell(board);
    OX ox = (turn % 2 == 0 ? O : X);
    // ShowBoard(board);

    /* ランダムに一手うつ */
    assert(PutOnBoardRandomly(ox, board, empty_cells));
  }
  if (result == WIN) win_count[x]++;
}

int main(void) {
  /* 各手の選択回数と勝った回数 */
  int visit_count[BOARD_HW * BOARD_HW] = {};
  int win_count[BOARD_HW * BOARD_HW] = {};
  int iter_count = 100000;
  srand(time(0));

  /* iter_count回対戦をシミュレートして勝率を計算する */
  for (int iter = 0; iter < iter_count; iter++) {
    SimulateUntilEnd(visit_count, win_count);
  }

  /**
   * +-+-+-+
   * |0|1|2|
   * +-+-+-+
   * |3|4|5|
   * +-+-+-+
   * |6|7|8|
   * +-+-+-+
   * 盤面に上記のようなマス番号が振られている．
   * 各一手目の勝率を出力
   **/
  for (int i = 0; i < BOARD_HW * BOARD_HW; i++) {
    printf("[%2d]: %3.2f%% (%5d / %5d)\n", i, 100. * win_count[i] / visit_count[i], win_count[i], visit_count[i]);
  }

  return 0;
}
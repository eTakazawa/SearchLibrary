#ifndef MANCALA_RESULT_H
#define MANCALA_RESULT_H

class Result {
public:
  Result(int w)
      : win_player(w) {};
  int win_player;
};

#endif // MANCALA_RESULT_H
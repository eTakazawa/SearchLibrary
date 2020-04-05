#ifndef MANCALA_ACTION_H
#define MANCALA_ACTION_H

class Action {
public:
  Action(int selected_cell)
      : selected_cell(selected_cell) {};
  int selected_cell;
};

#endif // MANCALA_ACTION_H
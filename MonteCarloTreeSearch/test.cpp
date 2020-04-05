#include <string>
#include <iostream>
#include <vector>
#include <limits>
#include <random>
#include <memory>
#include <cassert>

#include "mancala_const.hpp"
#include "mancala_mcts_serach.hpp"
#include "mancala_ai.hpp"

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

void TestForWebApi() {
  std::shared_ptr<State> state = std::make_shared<State>();
  state->SetCells("3,3,0,1,4,4,3,0,0");
  state->Show();
  MonteCarloTreeSearch mcts(MctsNode::CreateAsRoot(state));
  for (int t = 0; t < 5000; t++) mcts.Search();

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
}

void TestGetBestAction() {
  {
    int player_id = 0;
    std::vector<int> cells({3, 3, 3, 0, 3, 3, 3, 0});
    std::cout << GetBestAction(player_id, cells, 10000) << std::endl;
  }
  {
    int player_id = 1;
    std::vector<int> cells({3, 3, 0, 1, 4, 4, 3, 0});
    std::cout << GetBestAction(player_id, cells, 10000) << std::endl;
  }
  {
    int player_id = 0;
    std::vector<int> cells({4, 4, 0, 1, 4, 0, 4, 1});
    std::cout << GetBestAction(player_id, cells, 10000) << std::endl;
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
  vsCPU();
  // TestGetBestAction();
  // TestForWebApi();
  // TestStateAct();
  // TestStateGenerateAllNextStates();
  // TestMctsUpdate();
  // TestMctsFunc();
  // TestMctsSearch();
  // vsCPU();
  return 0;
}

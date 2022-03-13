#pragma once

#include <ctime>
#include <string>
#include <vector>
#include <map>

class Minesweeper {
public:
    struct Cell {
        size_t x = 0;
        size_t y = 0;
    };

    enum class GameStatus {
        NOT_STARTED,
        IN_PROGRESS,
        VICTORY,
        DEFEAT,
    };

    using RenderedField = std::vector<std::string>;

    Minesweeper(size_t width, size_t height, size_t mines_count) : width_(width), height_(height) {
        if (width * height == mines_count) {
            count_to_find = 0;
        } else {
            count_to_find = width * height - mines_count;
        }

        cur_field_.resize(height_);
        CreatingRandomMines_(mines_count);
        CreatingEmptyField_();
        game_status_ = GameStatus::NOT_STARTED;
    };
    Minesweeper(size_t width, size_t height, const std::vector<Cell>& cells_with_mines) : width_(width), height_(height) {
        if (width * height == cells_with_mines.size()) {
            count_to_find = 0;
        } else {
            count_to_find = width * height - cells_with_mines.size();
        }

        cur_field_.resize(height);
        CreatingRandomMines_(cells_with_mines);
        CreatingEmptyField_();
        game_status_ = GameStatus::NOT_STARTED;
    };

    void NewGame(size_t width, size_t height, size_t mines_count) {
        if (width * height == mines_count) {
            count_to_find = 0;
        } else {
            count_to_find = width * height - mines_count;
        }

        width_ = width;
        height_ = height;

        cur_field_.clear();
        cur_field_.resize(height);
        CreatingRandomMines_(mines_count);
        CreatingEmptyField_();
        game_status_ = GameStatus::NOT_STARTED;
    };
    void NewGame(size_t width, size_t height, const std::vector<Cell>& cells_with_mines) {
        if (width * height == cells_with_mines.size()) {
            count_to_find = 0;
        } else {
            count_to_find = width * height - cells_with_mines.size();
        }

        width_ = width;
        height_ = height;
        cur_field_.clear();
        cur_field_.resize(height);
        CreatingRandomMines_(cells_with_mines);
        CreatingEmptyField_();
        game_status_ = GameStatus::NOT_STARTED;
    };

    void OpenCell(const Cell& cell) {
        if (game_status_ != GameStatus::DEFEAT && game_status_ != GameStatus::VICTORY) {
            if (time_from_start_ == 0) {
                time_from_start_ = std::time(nullptr);
                game_status_ = GameStatus::IN_PROGRESS;
            }

            if (cur_field_[cell.y][cell.x] != '?') {
                if (field_with_bombs_[cell.y + 1][cell.x + 1] == -1) {
                    time_end_game_ = std::time(nullptr);
                    game_status_ = GameStatus::DEFEAT;

                    for(size_t i = 1; i <= height_; ++i) {
                        for (size_t g = 1; g <= width_; ++g) {
                            if (field_with_bombs_[i][g] == -1) {
                                cur_field_[i - 1][g - 1] = '*';
                            } else if (field_with_bombs_[i][g] == 0) {
                                cur_field_[i - 1][g - 1] = '.';
                            } else {
                                cur_field_[i - 1][g - 1] = field_with_bombs_[i][g];
                            }
                        }
                    }
                } else if (cur_field_[cell.y][cell.x] != '.' && cur_field_[cell.y][cell.x] != '1' && cur_field_[cell.y][cell.x] != '2' && cur_field_[cell.y][cell.x] != '3' && cur_field_[cell.y][cell.x] != '4' && cur_field_[cell.y][cell.x] != '5' && cur_field_[cell.y][cell.x] != '6' && cur_field_[cell.y][cell.x] != '7' && cur_field_[cell.y][cell.x] != '8' && cur_field_[cell.y][cell.x] != '9'){
                    OpenCellAlgorithm_(cell.x, cell.y);
                }
            }

            if (game_status_ == GameStatus::IN_PROGRESS && count_of_founded_cells == count_to_find) {
                time_end_game_ = std::time(nullptr);
                game_status_ = GameStatus::VICTORY;
            }
        }
    };
    void MarkCell(const Cell& cell) {
        if (game_status_ != GameStatus::DEFEAT && game_status_ != GameStatus::VICTORY) {
            if (cur_field_[cell.y][cell.x] == '-') {
                cur_field_[cell.y][cell.x] = '?';
            } else if (cur_field_[cell.y][cell.x] == '?') {
                cur_field_[cell.y][cell.x] = '-';
            }
        }
    };
    GameStatus GetGameStatus() const {
        return game_status_;
    };
    time_t GetGameTime() const {
        return std::time(nullptr) - time_from_start_;
    };
    RenderedField RenderField() const {
        return cur_field_;
    };

private:
    void CreatingRandomMines_(size_t& mines_count) {
        field_with_bombs_.clear();
        field_with_bombs_.resize(height_ + 2, std::vector<int>(width_ + 2, 0)); // resizing fields, cuz it's creates safe zone from errors

        if (width_ * height_ == mines_count) {
            for (int i = 1; i <= height_; ++i) {
                for (int g = 1; g <= width_; ++g) {
                    field_with_bombs_[i][g] = -1;
                }
            }
        } else {
            std::vector<std::vector<bool>> been_field(height_ + 2, std::vector<bool>(width_ + 2, false));

            srand(time(NULL));  // creating random seed to initialize random

            size_t i = 0;
            while (i < mines_count) {
                int rand_val = rand() % (width_ * height_);
                int x = rand_val % width_ + 1;
                int y = rand_val / width_ + 1;

                if (been_field[y][x] == false) {
                    been_field[y][x] = true;
                    bombs_list_.push_back(std::make_pair(y, x));

                    field_with_bombs_[y + 1][x + 1] += 1;
                    field_with_bombs_[y][x + 1] += 1;
                    field_with_bombs_[y - 1][x + 1] += 1;
                    field_with_bombs_[y + 1][x - 1] += 1;
                    field_with_bombs_[y][x - 1] += 1;
                    field_with_bombs_[y - 1][x - 1] += 1;
                    field_with_bombs_[y + 1][x] += 1;
                    field_with_bombs_[y - 1][x] += 1;

                    ++i;
                }
            }

            for (const auto& el : bombs_list_) {  // clearing cells of bombs
                field_with_bombs_[el.first][el.second] = -1;
            }
        }
    };

    void CreatingRandomMines_(const std::vector<Cell>& cells_with_mines) {
        field_with_bombs_.clear();
        field_with_bombs_.resize(height_ + 2, std::vector<int>(width_ + 2, 0)); // resizing fields, cuz it's creates safe zone from errors

        if (cells_with_mines.size() == width_ * height_) {
            for(int i = 1; i <= width_; ++i) {
                for(int g = 1; g <= height_; ++g) {
                    field_with_bombs_[g][i] = -1;
                }
            }
        } else {
            for (size_t i = 0; i < cells_with_mines.size(); ++i) {
                size_t y = cells_with_mines[i].y + 1;
                size_t x = cells_with_mines[i].x + 1;

                bombs_list_.push_back(std::make_pair(y, x));

                field_with_bombs_[y + 1][x + 1] += 1;
                field_with_bombs_[y][x + 1] += 1;
                field_with_bombs_[y - 1][x + 1] += 1;
                field_with_bombs_[y + 1][x - 1] += 1;
                field_with_bombs_[y][x - 1] += 1;
                field_with_bombs_[y - 1][x - 1] += 1;
                field_with_bombs_[y + 1][x] += 1;
                field_with_bombs_[y - 1][x] += 1;
            }

            for (const auto& el : bombs_list_) {  // clearing cells of bombs
                field_with_bombs_[el.first][el.second] = -1;
            }
        }
    };

    void CreatingEmptyField_() { // creates empty fie led
        std::string s = "";
        cur_field_.resize(height_);

        for(size_t i = 0; i < width_; ++i) {
            s += "-";
        }

        for(size_t i = 0; i < height_; ++i) {
            cur_field_[i] = s;
        }
    }

    void OpenCellAlgorithm_(int x, int y) {
        if (field_with_bombs_[y + 1][x + 1] >= 0 && cur_field_[y][x] == '-') {
            cur_field_[y][x] = converting_map_from_int[field_with_bombs_[y + 1][x + 1]];
            ++count_of_founded_cells;

            if (x < width_ && y < height_ && field_with_bombs_[y + 1][x + 2] > -1 && field_with_bombs_[y][x + 2] > - 1 && field_with_bombs_[y + 2][x + 2] > -1 && field_with_bombs_[y][x + 1] > -1 && field_with_bombs_[y + 2][x + 1] > -1 && field_with_bombs_[y + 1][x] > -1 && field_with_bombs_[y][x] > -1 && field_with_bombs_[y + 2][x] > -1) {
                if (y > -1 && x + 1 < width_) {
                    OpenCellAlgorithm_(x + 1, y);
                }
                if (y - 1 > -1 && x + 1 < width_ && x + 1 > -1) {
                    OpenCellAlgorithm_(x + 1, y - 1);
                }
                if (y + 1 < height_ && x + 1 < width_) {
                    OpenCellAlgorithm_(x + 1, y + 1);
                }
                if (y - 1 > -1 && x > -1) {
                    OpenCellAlgorithm_(x, y - 1);
                }
                if (y + 1 < height_ && x > -1) {
                    OpenCellAlgorithm_(x, y + 1);
                }
                if (y > -1 && x - 1 > -1) {
                    OpenCellAlgorithm_(x - 1, y);
                }
                if (y - 1 > -1 && x - 1 > -1) {
                    OpenCellAlgorithm_(x - 1, y - 1);
                }
                if (y + 1 < height_ && x - 1 > -1) {
                    OpenCellAlgorithm_(x - 1, y + 1);
                }
            }
        }
    }

    int width_;
    int height_;

    int count_of_founded_cells = 0;
    int count_to_find;

    time_t time_from_start_ = 0;
    time_t time_end_game_;
    
    std::vector<std::vector<int>> field_with_bombs_;
    std::vector<std::pair<int, int>> bombs_list_;
    RenderedField cur_field_;
    GameStatus game_status_ = GameStatus::NOT_STARTED;

    std::map<int, char> converting_map_from_int = {
        {0, '.'},
        {1, '1'},
        {2, '2'},
        {3, '3'},
        {4, '4'},
        {5, '5'},
        {6, '6'},
        {7, '7'},
        {8, '8'},
        {9, '9'},
    };
};

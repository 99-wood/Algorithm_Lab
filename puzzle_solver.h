#ifndef PUZZLE_SOLVER_H
#define PUZZLE_SOLVER_H
#include <cstdio>
#include <memory.h>
#include <cstring>
#include "sha256.h"
#include <string>
#include <vector>
namespace guess {
    using Clue = std::vector<int>;

    class PuzzleSolver {
    public:
        PuzzleSolver(const std::string &targetHash, const std::vector<Clue> &clues);
        std::string ans;
        int tries = 0;
        void solve();

        // 新增：从 JSON 文件中加载数据（封装）
        static std::pair<std::string, std::vector<Clue>> loadPuzzleData(const std::string &filename);

        static std::string sha256(const std::string &input);

    private:
        std::string targetHash;
        std::vector<Clue> clues;

        bool isValid(const std::vector<int> &password);

        bool isPrime(int n);


        bool backtrack(std::vector<int> &password, int index);
    };
}
#endif //PUZZLE_SOLVER_H
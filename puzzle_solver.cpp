#include "puzzle_solver.h"
#include "sha256.h"
#include "nlohmann/json.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <unordered_set>

using json = nlohmann::json;
namespace guess {
// 构造函数
    PuzzleSolver::PuzzleSolver(const std::string &targetHash, const std::vector<Clue> &clues)
            : targetHash(targetHash), clues(clues) {}

// 入口函数：回溯查找有效密码
    void PuzzleSolver::solve() {
        std::vector<int> password(3, -1);
        if (backtrack(password, 0)) {
            ans = std::to_string(password[0]) + std::to_string(password[1]) + std::to_string(password[2]);
//            return std::to_string(password[0]) + std::to_string(password[1]) + std::to_string(password[2]);
        }
//        return "";
    }

// 回溯搜索所有可能三位数
    bool PuzzleSolver::backtrack(std::vector<int> &password, int index) {
        if (index == 3) {
            if (!isValid(password)) return false;

            std::string guess = std::to_string(password[0]) + std::to_string(password[1]) + std::to_string(password[2]);
            std::string hash = sha256(guess);
            // std::cout << hash << " " << guess << std::endl;
            ++tries;
            return hash == targetHash;
        }

        for (int d = 0; d <= 9; ++d) {
            password[index] = d;
            if (backtrack(password, index + 1)) return true;
            password[index] = -1;
        }
        return false;
    }

// 判断密码是否满足线索条件
    bool PuzzleSolver::isValid(const std::vector<int> &password) {
        for (const auto &clue: clues) {
            if (clue == Clue{-1, -1}) {
                std::unordered_set<int> digits(password.begin(), password.end());
                for (int d: password) {
                    if (!isPrime(d)) return false;
                }
                if (digits.size() != 3) return false;
            } else if (clue.size() == 2) {
                int pos = clue[0] - 1;
                int type = clue[1];
                if (pos < 0 || pos >= 3) return false;
                if (type == 0 && password[pos] % 2 != 0) return false; // 期望偶数
                if (type == 1 && password[pos] % 2 != 1) return false; // 期望奇数
            } else if (clue.size() == 3) {
                for (int i = 0; i < 3; ++i) {
                    if (clue[i] != -1 && password[i] != clue[i])
                        return false;
                }
            }
        }
        return true;
    }

// 判断是否是小于10的素数
    bool PuzzleSolver::isPrime(int n) {
        static std::unordered_set<int> primes = {2, 3, 5, 7};
        return primes.count(n);
    }
    BYTE salt[] = {
            0xB2, 0x53, 0x22, 0x65, 0x7D, 0xDF, 0xB0, 0xFE,
            0x9C, 0xDE, 0xDE, 0xFE, 0xF3, 0x1D, 0xDC, 0x3E
    };
// 利用 sha256.c 实现的哈希函数
    std::string PuzzleSolver::sha256(const std::string &input) {
        BYTE hash[SHA256_BLOCK_SIZE];
        SHA256_CTX ctx;
        sha256_init(&ctx);
        sha256_update(&ctx, salt, sizeof(salt));
        sha256_update(&ctx, reinterpret_cast<const BYTE *>(input.c_str()), input.length());
        sha256_final(&ctx, hash);

        std::stringstream ss;
        for (int i = 0; i < SHA256_BLOCK_SIZE; ++i) {
            int x = hash[i] / 16, y = hash[i] % 16;
            ss<< (char)(x >= 10 ? 'a' + x - 10 : '0' + x) << (char)(y >= 10 ? 'a' + y - 10 : '0' + y);
        }
//        std::cout << ss.str() << " " << input <<std::endl;
        return ss.str();
    }

// 从 input.json 中加载哈希值和线索
    std::pair<std::string, std::vector<Clue>> PuzzleSolver::loadPuzzleData(const std::string &filename) {
        std::ifstream fin(filename);
        if (!fin.is_open()) {
            std::cerr << "无法打开文件: " << filename << std::endl;
            std::exit(1);
        }

        json data;
        fin >> data;

        if (!data.contains("L") || !data["L"].is_string()) {
            std::cerr << "JSON中缺少 \"L\" 字段\n";
            std::exit(2);
        }

        if (!data.contains("C") || !data["C"].is_array()) {
            std::cerr << "JSON中缺少 \"C\" 字段\n";
            std::exit(3);
        }

        std::string hash = data["L"];
        std::vector<Clue> clues;

        for (const auto &arr: data["C"]) {
            Clue clue;
            for (const auto &val: arr) {
                clue.push_back(val.get<int>());
            }
            clues.push_back(clue);
        }

        return {hash, clues};
    }
}
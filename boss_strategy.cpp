#include "boss_strategy.h"
#include <queue>
#include <cmath>
#include <climits>
#include <fstream>
#include <iostream>
#include <cassert>
#include <unordered_set>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace game {

    void BossStrategy::init(const std::vector<Skill>& skills, const std::vector<int>& bossHPs) {
        skillSet = skills;
        initialBossHPs = bossHPs;
    }

    int BossStrategy::lowerBound(const std::vector<int>& bossHPs, int currentBossIndex) {
        int totalRemaining = 0;
        for (size_t i = currentBossIndex; i < bossHPs.size(); ++i) {
            totalRemaining += bossHPs[i];
        }

        double avgDPS = 0.0;
        for (const Skill& s : skillSet) {
            avgDPS += static_cast<double>(s.damage) / (s.cooldown + 1);
        }
        if (avgDPS <= 0.1) avgDPS = 1.0;

        return static_cast<int>(std::ceil(totalRemaining / avgDPS));
    }

    std::string hashState(const std::vector<int>& hp, int idx, const std::vector<int>& cd) {
        std::ostringstream oss;
        for (int h : hp) oss << h << '-';
        oss << "|" << idx << "|";
        for (int c : cd) oss << c << '-';
        return oss.str();
    }

    std::vector<std::string> BossStrategy::findOptimalSequence(bool verbose) {
        struct State {
            std::vector<int> bossHPs;
            int currentBossIndex;
            int turn;
            std::vector<int> cooldownStatus;
            std::vector<std::string> skillSequence;

            bool operator<(const State& other) const {
                return turn > other.turn;
            }
        };

        std::priority_queue<State> pq;
        std::unordered_set<std::string> visited;

        State init = {
                initialBossHPs,
                0,
                0,
                std::vector<int>(skillSet.size(), 0),
                {}
        };
        pq.push(init);

        int bestTurn = INT_MAX;
        std::vector<std::string> bestPlan;

        int expandedNodes = 0;
        int prunedBranches = 0;

        while (!pq.empty()) {
            State current = pq.top(); pq.pop();

            std::string stateKey = hashState(current.bossHPs, current.currentBossIndex, current.cooldownStatus);
            if (visited.count(stateKey)) {
                if (verbose) std::cout << "  [Skip] Already visited: " << stateKey << "\n";
                continue;
            }
            visited.insert(stateKey);

            expandedNodes++;
            int depth = current.skillSequence.size();
            std::string indent(depth, ' ');

            if (current.currentBossIndex >= static_cast<int>(current.bossHPs.size())) {
                if (current.turn < bestTurn) {
                    bestTurn = current.turn;
                    bestPlan = current.skillSequence;
                    if (verbose) {
                        std::cout << indent << " Found new best solution with " << bestTurn << " turns\n";
                    }
                }
                continue;
            }

            int lb = lowerBound(current.bossHPs, current.currentBossIndex);
            int bound = current.turn + lb;

            if (bound >= bestTurn) {
                prunedBranches++;
                if (verbose) {
                    std::cout << indent << " [Pruned] Estimated cost (" << bound
                              << ") >= Best found (" << bestTurn << ")\n\n";
                }
                continue;
            }

            bool hasAvailableSkill = false;

            for (size_t i = 0; i < skillSet.size(); ++i) {
                if (current.cooldownStatus[i] > 0) continue;

                hasAvailableSkill = true;
                const Skill& skill = skillSet[i];
                State next = current;
                next.turn++;
                next.bossHPs[next.currentBossIndex] -= skill.damage;
                if (next.bossHPs[next.currentBossIndex] <= 0) {
                    next.bossHPs[next.currentBossIndex] = 0;
                    next.currentBossIndex++;
                }

                for (size_t j = 0; j < next.cooldownStatus.size(); ++j) {
                    if (next.cooldownStatus[j] > 0) next.cooldownStatus[j]--;
                }
                next.cooldownStatus[i] = skill.cooldown;

                next.skillSequence.push_back("Turn " + std::to_string(next.turn) + ": " + skill.name);
                pq.push(next);
            }

            if (!hasAvailableSkill) {
                State next = current;
                next.turn++;
                for (size_t j = 0; j < next.cooldownStatus.size(); ++j) {
                    if (next.cooldownStatus[j] > 0) next.cooldownStatus[j]--;
                }
                next.skillSequence.push_back("Turn " + std::to_string(next.turn) + ": Wait");
                pq.push(next);
            }
        }

        if (verbose) {
            std::cout << "=== Branch and Bound Summary ===\n";
            std::cout << "Expanded Nodes: " << expandedNodes << "\n";
            std::cout << "Pruned Branches: " << prunedBranches << "\n";
            std::cout << "Best Solution Turn Count: " << bestTurn << "\n";
        }

        return bestPlan;
    }

    std::pair<std::vector<int>, std::vector<Skill>> loadBossBattleData(const std::string& fileName) {
        std::ifstream fin(fileName);
        if (!fin.is_open()) {
            std::cerr << "鏃犳硶鎵撳紑鏂囦欢 " << fileName << std::endl;
            std::exit(1);
        }

        nlohmann::json data;
        fin >> data;

        if (!data.contains("B") || !data["B"].is_array()) {
            std::cerr << "JSON涓己澶?\"B\" 瀛楁鎴栨牸寮忛敊璇痋n";
            std::exit(1);
        }
        std::vector<int> bossHPs = data["B"].get<std::vector<int>>();

        if (!data.contains("PlayerSkills") || !data["PlayerSkills"].is_array()) {
            std::cerr << "JSON涓己澶?\"PlayerSkills\" 瀛楁鎴栨牸寮忛敊璇痋n";
            std::exit(1);
        }

        std::vector<Skill> skills;
        int index = 0;
        for (const auto& skillArr : data["PlayerSkills"]) {
            if (!skillArr.is_array() || skillArr.size() != 2) {
                std::cerr << "鎶€鑳芥牸寮忛敊璇細搴斾负浜岀淮鏁扮粍[[a0,b0],[a1,b1],...]\n";
                std::exit(1);
            }
            int damage = skillArr[0].get<int>();
            int cooldown = skillArr[1].get<int>();
            skills.push_back({ "Skill" + std::to_string(index++), damage, cooldown });
        }

        return { bossHPs, skills };
    }

} // namespace MazeGame
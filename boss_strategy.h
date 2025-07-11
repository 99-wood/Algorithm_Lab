#ifndef BOSS_STRATEGY_H
#define BOSS_STRATEGY_H

#include <vector>
#include <string>

namespace boss {

    struct Skill {
        int id;
        int damage;
        int cooldown;
    };

    class BossStrategy {
    public:
        void init(const std::vector<Skill>& skills, const std::vector<int>& bossHPs);
        std::vector<int> findOptimalSequence(bool verbose = false);
    private:
        std::vector<Skill> skillSet;
        std::vector<int> initialBossHPs;
        int lowerBound(const std::vector<int>& bossHPs, int currentBossIndex);
    };

    //  在类外添加此函数声明
    std::pair<std::vector<int>, std::vector<Skill>> loadBossBattleData(const std::string& fileName);
    int loadMinNum(const std::string& fileName);

} // namespace MazeGame

#endif // BOSS_STRATEGY_H

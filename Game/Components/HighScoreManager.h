#pragma once
#include <string>
#include <vector>

namespace dae
{
    class HighScoreManager final
    {
    public:
        struct Entry
        {
            std::string name;
            int score;
        };

        static std::vector<Entry> Load();
        static void Save(const std::vector<Entry>& entries);
        static bool Qualifies(int score);
        static std::vector<Entry> Insert(const std::string& name, int score);

    private:
        static constexpr int k_maxEntries = 10;
    };
}

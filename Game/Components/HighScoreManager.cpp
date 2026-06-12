#include "HighScoreManager.h"
#include "ResourceManager.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <algorithm>

using json = nlohmann::json;

namespace
{
    std::string GetFilePath()
    {
        return (dae::ResourceManager::GetInstance().GetDataPath() / "highscores.json").string();
    }
}

std::vector<dae::HighScoreManager::Entry> dae::HighScoreManager::Load()
{
    std::vector<Entry> entries;

    std::ifstream file(GetFilePath());
    if (!file.is_open()) return entries;

    json data;
    file >> data;

    for (const auto& item : data)
        entries.push_back(Entry{ item.value("name", std::string("AAA")), item.value("score", 0) });

    return entries;
}

void dae::HighScoreManager::Save(const std::vector<Entry>& entries)
{
    json data = json::array();
    for (const auto& entry : entries)
        data.push_back({ {"name", entry.name}, {"score", entry.score} });

    std::ofstream file(GetFilePath());
    file << data.dump(4);
}

bool dae::HighScoreManager::Qualifies(int score)
{
    auto entries = Load();
    if (static_cast<int>(entries.size()) < k_maxEntries) return true;

    const auto lowest = std::min_element(entries.begin(), entries.end(),
        [](const Entry& a, const Entry& b) { return a.score < b.score; });

    return score > lowest->score;
}

std::vector<dae::HighScoreManager::Entry> dae::HighScoreManager::Insert(const std::string& name, int score)
{
    auto entries = Load();
    entries.push_back(Entry{ name, score });

    std::sort(entries.begin(), entries.end(),
        [](const Entry& a, const Entry& b) { return a.score > b.score; });

    if (static_cast<int>(entries.size()) > k_maxEntries)
        entries.resize(k_maxEntries);

    Save(entries);
    return entries;
}

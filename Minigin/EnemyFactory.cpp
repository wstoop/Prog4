#include "EnemyFactory.h"
#include "GameObject.h"

std::unordered_map<char, dae::EnemyFactory::Creator> dae::EnemyFactory::m_Creators;
void dae::EnemyFactory::Register(char type, Creator creator)
{
    m_Creators[type] = creator;
}

std::unique_ptr<dae::GameObject> dae::EnemyFactory::Create(char type)
{
    auto it = m_Creators.find(type);
    if (it != m_Creators.end())
        return it->second();

    return nullptr;
}
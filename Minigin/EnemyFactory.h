#include <memory>
#include <unordered_map>
#include <functional>
#include "GameObject.h"

namespace dae
{
    
    class EnemyFactory
    { 
    public:
        using Creator = std::function<std::unique_ptr<dae::GameObject>()>;

        static void Register(char type, Creator creator);
        static std::unique_ptr<dae::GameObject> Create(char type);
    private:
        static std::unordered_map<char, Creator> m_Creators;
    };
}
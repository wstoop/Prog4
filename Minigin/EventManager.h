#pragma once
#include <unordered_map>
#include <vector>
#include <queue>
#include <memory>
#include "Singleton.h"

using EventId = unsigned int;

template <int length> struct sdbm_hash
{
    consteval static unsigned int _calculate(const char* const text, unsigned int& value) {
        const unsigned int character = sdbm_hash<length - 1>::_calculate(text, value);
        value = character + (value << 6) + (value << 16) - value;
        return text[length - 1];
    }
    consteval static unsigned int calculate(const char* const text) {
        unsigned int value = 0;
        const auto character = _calculate(text, value);
        return character + (value << 6) + (value << 16) - value;
    }
};
template <> struct sdbm_hash<1> {
    consteval static int _calculate(const char* const text, unsigned int&) { return text[0]; }
};
template <size_t N> consteval unsigned int make_sdbm_hash(const char(&text)[N]) {
    return sdbm_hash<N - 1>::calculate(text);
}


struct Event
{
    const EventId id;
    explicit Event(EventId _id) : id{ _id } {}
    virtual ~Event() = default;
};

template<typename T>
struct DataEvent final : public Event
{
    T data;
    DataEvent(EventId _id, T _data)
        : Event(_id), data(std::move(_data)) {
    }
};

class EventHandler
{
public:
    virtual ~EventHandler();
    virtual void HandleEvent(const Event* pEvent) = 0;
};


class EventManager final : public dae::Singleton<EventManager>
{
public:
    static bool IsDestroyed() { return s_destroyed; }
    ~EventManager() { s_destroyed = true; }
    void AttachEvent(EventId id, EventHandler* handler)
    {
        m_Listeners[id].push_back(handler);
    }

    void DetachEvent(EventId id, EventHandler* handler)
    {
        auto it = m_Listeners.find(id);
        if (it == m_Listeners.end()) return;

        auto& vec = it->second;
        vec.erase(std::remove(vec.begin(), vec.end(), handler), vec.end());
    }

    // Remove a handler from ALL events (call in destructor of any listener)
    void DetachAll(EventHandler* handler)
    {
        for (auto& [id, vec] : m_Listeners)
            vec.erase(std::remove(vec.begin(), vec.end(), handler), vec.end());
    }

    void SendEvent(std::unique_ptr<Event> event)
    {
        m_EventQueue.push(std::move(event));
    }

    void SendEvent(EventId id)
    {
        m_EventQueue.push(std::make_unique<Event>(id));
    }

    void ClearQueue()
    {
        m_EventQueue = {};
    }

    void Update()
    {
        std::queue<std::unique_ptr<Event>> processing;
        std::swap(processing, m_EventQueue);

        while (!processing.empty())
        {
            auto ev = std::move(processing.front());
            processing.pop();

            auto it = m_Listeners.find(ev->id);
            if (it == m_Listeners.end()) continue;

            const auto listeners = it->second;
            for (auto* handler : listeners)
                handler->HandleEvent(ev.get());
        }
    }

private:
    friend class dae::Singleton<EventManager>;
    EventManager() = default;

    std::unordered_map<EventId, std::vector<EventHandler*>> m_Listeners;
    std::queue<std::unique_ptr<Event>> m_EventQueue;
	inline static bool s_destroyed;
};

inline EventHandler::~EventHandler()
{
    if (!EventManager::IsDestroyed())
        EventManager::GetInstance().DetachAll(this);
}
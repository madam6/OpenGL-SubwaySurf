#include "EventSystem.h"

void EventSystem::Subscribe(const std::string& eventName, IObserver* observer)
{
	m_Observers[eventName].push_back(observer);
}

void EventSystem::Unsubscribe(const std::string& eventName, IObserver* observer)
{
    auto it = m_Observers.find(eventName);
    if (it != m_Observers.end())
    {
        auto& obsList = it->second;
        obsList.erase(std::remove(obsList.begin(), obsList.end(), observer), obsList.end());
    }
}
void EventSystem::BroadcastEvent(const std::string& eventName, const EventData& data)
{
	for (auto* obs : m_Observers[eventName])
	{
		obs->OnEvent(eventName, data);
	}
}

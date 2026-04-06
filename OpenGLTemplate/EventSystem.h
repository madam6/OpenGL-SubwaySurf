#pragma once
#include "Common.h"
class Entity;

struct EventData 
{
	std::any payload;
};

struct CollisionPayload 
{
	Entity* entityA;
	Entity* entityB;
};

class IObserver 
{
public:
	virtual ~IObserver() = default;
	virtual void OnEvent(const std::string& eventName, const EventData& eventData) = 0;
};

class EventSystem
{
public:
	static EventSystem& Instance()
	{
		static EventSystem instance;
		return instance;
	}

	void Subscribe(const std::string& eventName, IObserver* observer);

	void Unsubscribe(const std::string& eventName, IObserver* observer);

	void BroadcastEvent(const std::string& eventName, const EventData& data);

private:
	std::unordered_map<std::string, std::vector<IObserver*>> m_Observers;
};
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

// Simple implementation of the Observer pattern to enable event system withing the engine
// Derive your clarss/component from IObserver and register it through Subscribe() function
// It is recommended to unregister itself on destruction using Unsubsribe
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
	
	// Broadcast any event to all registered observers
	void BroadcastEvent(const std::string& eventName, const EventData& data);

private:
	std::unordered_map<std::string, std::vector<IObserver*>> m_Observers;
};
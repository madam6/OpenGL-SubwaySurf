#pragma once
#include "Component.h"
#include "EventSystem.h"
#include "CollectibleComponent.h"
#include "CatmullRom.h"
#include "Heart.h"

class PlayerTrackMovementComponent;

class CurrencyManagerComponent : public Component, public IObserver
{
public:
    void Init() override;
    void Update(float dt) override;
    void OnEvent(const std::string& eventName, const EventData& data) override;
    void Apply(const PropertyMap& props);
    ~CurrencyManagerComponent() { EventSystem::Instance().Unsubscribe("OnCollision", this); }
    void AddRenderData(std::vector<RenderData>& renderQueue) override;
    int GetScore() const { return m_Score; }
    int GetHealth() const { return m_Health; }
private:
    void RespawnAll();

    std::shared_ptr<PlayerTrackMovementComponent> m_PlayerRef;
    std::shared_ptr<CCatmullRom> m_TrackRef;
    std::vector<std::shared_ptr<CollectibleComponent>> m_Crystals;
    static std::shared_ptr<CCrystal> s_SharedCrystal;

    std::vector<std::shared_ptr<CollectibleComponent>> m_Hearts;
    static std::shared_ptr<CHeart> s_SharedHeart;
    std::string m_HeartBaseName = "Heart";
    int m_MaxHearts = 10;
    int m_Health = 3;

    std::vector<std::shared_ptr<CollectibleComponent>> m_Fences;
    std::string m_FenceBaseName = "Fence";
    int m_MaxFences = 15;

    std::vector<std::shared_ptr<CollectibleComponent>> m_Bananas;
    std::string m_BananaBaseName = "Banana";
    int m_MaxBananas = 15;

    static std::shared_ptr<COpenAssetImportMesh> s_SharedFence;
    static std::shared_ptr<COpenAssetImportMesh> s_SharedBanana;

    int m_CurrentLap = 0;
    int m_Score = 0;
    int m_MinBatches = 2;
    int m_MaxBatches = 5;
    int m_MinPerBatch = 3;
    int m_MaxPerBatch = 6;
    int m_SpacingInBatch = 10;
    std::string m_CurrencyBaseName{};
};

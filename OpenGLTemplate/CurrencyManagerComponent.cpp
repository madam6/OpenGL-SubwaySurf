#include "CurrencyManagerComponent.h"
#include "Entity.h"
#include "Game.h"
#include "CatmulRomComponent.h"
#include "PlayerTrackMovementComponent.h"
#include "ComponentRegistry.h"
#include "MaterialComponent.h"
#include "Crystal.h"
#include "Camera.h"

std::shared_ptr<CCrystal> CurrencyManagerComponent::s_SharedCrystal = nullptr;
std::shared_ptr<CHeart> CurrencyManagerComponent::s_SharedHeart = nullptr;

namespace
{
    bool registered = []()
        {
            ComponentRegistry::Instance().Register("CurrencyManagerComponent",
                [](Entity& e, const PropertyMap& props)
                {
                    auto& c = e.AddComponent<CurrencyManagerComponent>();
                    c.Apply(props);
                });

            return true;
        }();
}

void CurrencyManagerComponent::Init()
{
    if (auto owner = GetOwner()) m_PlayerRef = owner->FindComponent<PlayerTrackMovementComponent>();

    if (auto trackEntity = Game::GetInstance().FetchEntityByName("CatmullRomTrack")) 
    {
        m_TrackRef = trackEntity->FindComponent<CatmullRomComponent>()->m_track;
    }

    if (auto templateCrystal = Game::GetInstance().FetchEntityByName(m_CurrencyBaseName))
    {
        m_Crystals.push_back(templateCrystal->FindComponent<CollectibleComponent>());
    }

    int maxPoolSize = m_MaxBatches * m_MaxPerBatch;

    for (int i = 1; i < maxPoolSize; i++)
    {
        auto newCrystal = Game::GetInstance().SpawnEntityFromTemplate(m_CurrencyBaseName);
        if (newCrystal)
        {
            newCrystal->SetName(m_CurrencyBaseName + "_" + std::to_string(i));
            newCrystal->Init();
            m_Crystals.push_back(newCrystal->FindComponent<CollectibleComponent>());
        }
    }

    if (!s_SharedCrystal) 
    {
        s_SharedCrystal = std::make_shared<CCrystal>();
        s_SharedCrystal->Create("resources\\textures\\", "crystalTexture.jpg");
    }

    EventSystem::Instance().Subscribe("OnCollision", this);

    for (int i = 0; i < m_MaxHearts; i++)
    {
        auto newHeart = Game::GetInstance().SpawnEntityFromTemplate(m_HeartBaseName);
        if (newHeart)
        {
            newHeart->SetName(m_HeartBaseName + "_" + std::to_string(i));
            newHeart->Init();
            m_Hearts.push_back(newHeart->FindComponent<CollectibleComponent>());
        }
    }
    
    for (int i = 0; i < m_MaxFences; i++)
    {
        auto newFence = Game::GetInstance().SpawnEntityFromTemplate(m_FenceBaseName);
        if (newFence)
        {
            newFence->SetName(m_FenceBaseName + "_" + std::to_string(i));
            newFence->Init();
            m_Fences.push_back(newFence->FindComponent<CollectibleComponent>());
        }
    }

    if (!s_SharedHeart)
    {
        s_SharedHeart = std::make_shared<CHeart>();
        s_SharedHeart->Create("resources\\textures\\", "crystalTexture.jpg");
    }

    RespawnAll();

   
}

void CurrencyManagerComponent::Update(float dt)
{
    if (m_PlayerRef && m_TrackRef)
    {
        float currentDist = m_PlayerRef->GetCurrentDistance();
        int currentLap = m_TrackRef->CurrentLap(currentDist);

        if (currentLap > m_CurrentLap)
        {
            m_CurrentLap = currentLap;

            RespawnAll();

            DEBUG_MSG("Lap %d completed!", m_CurrentLap);
        }
    }
}

void CurrencyManagerComponent::AddRenderData(std::vector<RenderData>& renderQueue)
{
    if (m_Crystals.empty() || !s_SharedCrystal) return;

    std::vector<glm::mat4> activeMatrices;
    for (auto& crystal : m_Crystals)
    {
        if (crystal && !crystal->IsCollected())
        {
            auto mv = crystal->GetOwner()->FindComponent<ModelViewComponent>();
            if (mv) 
            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), mv->GetPosition());
                model *= mv->GetOrientation();
                model = glm::scale(model, mv->GetScale());
                activeMatrices.push_back(model);
            }
        }
    }

    if (!m_Hearts.empty() && s_SharedHeart)
    {
        std::vector<glm::mat4> activeHeartMatrices;

        glm::vec3 matAm(1), matDi(1), matSp(0); float matSh = 1;

        for (auto& heart : m_Hearts)
        {
            if (heart && !heart->IsCollected())
            {
                auto owner = heart->GetOwner();
                auto mv = owner->FindComponent<ModelViewComponent>();
                if (mv)
                {
                    glm::mat4 model = glm::translate(glm::mat4(1.0f), mv->GetPosition());
                    model *= mv->GetOrientation();
                    model = glm::scale(model, mv->GetScale());
                    activeHeartMatrices.push_back(model);
                }

                auto mat = owner->FindComponent<MaterialComponent>();
                if (mat)
                {
                    matAm = mat->GetMa(); matDi = mat->GetMd();
                    matSp = mat->GetMs(); matSh = mat->GetShiny();
                }
            }
        }

        if (!activeHeartMatrices.empty())
        {
            RenderData outlineBatch;
            outlineBatch.mesh = s_SharedHeart;
            outlineBatch.shader = Game::GetInstance().GetShader("outlineShader");
            outlineBatch.isInstanced = true;
            outlineBatch.instanceMatrices = activeHeartMatrices;
            outlineBatch.isOutline = true;
            renderQueue.push_back(outlineBatch);

            RenderData hBatch;
            hBatch.mesh = s_SharedHeart;
            // TODO: Get shader from components?
            hBatch.shader = Game::GetInstance().GetShader("toonShader");
            hBatch.isInstanced = true;
            hBatch.instanceMatrices = activeHeartMatrices;
            hBatch.useTexture = false;
            hBatch.Ma = matAm;
            hBatch.Md = matDi;
            hBatch.Ms = matSp;
            hBatch.shininess = matSh;
            renderQueue.push_back(hBatch);
        }
    }
    if (activeMatrices.empty()) return;

    RenderData batch;
    batch.mesh = s_SharedCrystal;
    // TODO: Get shader from components?
    batch.shader = Game::GetInstance().GetShader("CrystalShader");
    batch.isInstanced = true;
    batch.instanceMatrices = activeMatrices;
    batch.useTexture = true;

    renderQueue.push_back(batch);
}

void CurrencyManagerComponent::RespawnAll()
{
    for (auto& crystal : m_Crystals) crystal->Collect();
    for (auto& heart : m_Hearts) heart->Collect();
    for (auto& fence : m_Fences) fence->Collect();

    int numBatches = m_MinBatches + (rand() % (m_MaxBatches - m_MinBatches + 1));
    int crystalIndex = 0;
    int heartIndex = 0;
    int fenceIndex = 0;

    float currentBatchDist = m_PlayerRef->GetCurrentDistance() + 50.0f + (rand() % 50);

    for (int b = 0; b < numBatches; b++)
    {
        int crystalsInThisBatch = m_MinPerBatch + (rand() % (m_MaxPerBatch - m_MinPerBatch + 1));
        int currentLane = (rand() % 3) - 1;

        for (int i = 0; i < crystalsInThisBatch; i++)
        {
            if (crystalIndex >= m_Crystals.size()) break;

            float dist = currentBatchDist + (i * m_SpacingInBatch);
            glm::vec3 p, up, forwardVec;
            m_TrackRef->Sample(dist, p, up);
            m_TrackRef->Sample(dist + 1.0f, forwardVec);

            glm::vec3 forward = glm::normalize(forwardVec - p);
            glm::vec3 safeUp = glm::length(up) < 0.001f ? glm::vec3(0.0f, 1.0f, 0.0f) : up;
            glm::vec3 right = glm::normalize(glm::cross(forward, safeUp));
            glm::vec3 realUp = glm::normalize(glm::cross(right, forward));

            glm::vec3 finalPos = p + (right * (currentLane * 10.0f)) + (realUp * 5.0f);
            glm::mat4 orientation = glm::mat4(glm::vec4(right, 0.0f), glm::vec4(realUp, 0.0f), glm::vec4(forward, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

            m_Crystals[crystalIndex]->SpawnAt(finalPos, orientation);
            crystalIndex++;
        }

        currentBatchDist += (crystalsInThisBatch * m_SpacingInBatch);

        if (heartIndex < m_Hearts.size() && (rand() % 100 < 50))
        {
            float heartDist = currentBatchDist + 15.0f;
            int heartLane = (rand() % 3) - 1;

            glm::vec3 p, up, forwardVec;
            m_TrackRef->Sample(heartDist, p, up);
            m_TrackRef->Sample(heartDist + 1.0f, forwardVec);

            glm::vec3 forward = glm::normalize(forwardVec - p);
            glm::vec3 safeUp = glm::length(up) < 0.001f ? glm::vec3(0.0f, 1.0f, 0.0f) : up;
            glm::vec3 right = glm::normalize(glm::cross(forward, safeUp));
            glm::vec3 realUp = glm::normalize(glm::cross(right, forward));

            glm::vec3 finalPos = p + (right * (heartLane * 10.0f)) + (realUp * 5.0f);
            glm::mat4 orientation = glm::mat4(glm::vec4(right, 0.0f), glm::vec4(realUp, 0.0f), glm::vec4(forward, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

            m_Hearts[heartIndex]->SpawnAt(finalPos, orientation);
            heartIndex++;
        }

        if (fenceIndex < m_Fences.size() && (rand() % 100 < 80))
        {
            float fenceDist = currentBatchDist + 30.0f;
            int fenceLane = (rand() % 3) - 1;

            glm::vec3 p, up, forwardVec;
            m_TrackRef->Sample(fenceDist, p, up);
            m_TrackRef->Sample(fenceDist + 1.0f, forwardVec);

            glm::vec3 forward = glm::normalize(forwardVec - p);
            glm::vec3 safeUp = glm::length(up) < 0.001f ? glm::vec3(0.0f, 1.0f, 0.0f) : up;
            glm::vec3 right = glm::normalize(glm::cross(forward, safeUp));
            glm::vec3 realUp = glm::normalize(glm::cross(right, forward));

            glm::vec3 finalPos = p + (right * (fenceLane * 10.0f)) + (realUp * 3.9f);

            glm::mat4 orientation = glm::mat4(glm::vec4(right, 0.0f), glm::vec4(realUp, 0.0f), glm::vec4(forward, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
            orientation = glm::rotate(orientation, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            m_Fences[fenceIndex]->SpawnAt(finalPos, orientation);
            fenceIndex++;
        }

        currentBatchDist += 150.0f + (rand() % 101);
    }
}

void CurrencyManagerComponent::OnEvent(const std::string& eventName, const EventData& data) 
{
    if (eventName == "OnCollision")
    {
        if (m_PlayerRef && m_PlayerRef->IsRecovering()) return;
        CollisionPayload payload = std::any_cast<CollisionPayload>(data.payload);
        Entity* hitItem = nullptr;

        if (payload.entityA->GetName() == "MC" &&
            (payload.entityB->GetName().find(m_CurrencyBaseName) != std::string::npos ||
                payload.entityB->GetName().find(m_HeartBaseName) != std::string::npos ||
                payload.entityB->GetName().find(m_FenceBaseName) != std::string::npos))
        {
            hitItem = payload.entityB;
        }
        else if (payload.entityB->GetName() == "MC" &&
            (payload.entityA->GetName().find(m_CurrencyBaseName) != std::string::npos ||
                payload.entityA->GetName().find(m_HeartBaseName) != std::string::npos ||
                payload.entityA->GetName().find(m_FenceBaseName) != std::string::npos))
        {
            hitItem = payload.entityA;
        }

        if (hitItem)
        {
            auto collectible = hitItem->FindComponent<CollectibleComponent>();
            if (collectible && !collectible->IsCollected())
            {
                // To avoid collecting fences
                if (hitItem->GetName().find(m_FenceBaseName) == std::string::npos)
                {
                    collectible->Collect();
                }

                if (hitItem->GetName().find(m_CurrencyBaseName) != std::string::npos)
                {
                    m_Score += 1;
                    DEBUG_MSG("Collected Crystal! Score: %d", m_Score);
                }
                else if (hitItem->GetName().find(m_HeartBaseName) != std::string::npos)
                {
                    m_Health += 1;
                    DEBUG_MSG("Collected Heart! Health: %d", m_Health);
                }
                else if (hitItem->GetName().find(m_FenceBaseName) != std::string::npos)
                {
                    m_Health -= 1;
                    DEBUG_MSG("Hit a Fence! Lost a heart! Health: %d", m_Health);
                    if (m_PlayerRef) m_PlayerRef->StartRecovery();
                }
            }
        }
    }
}

void CurrencyManagerComponent::Apply(const PropertyMap& props)
{
    auto it = props.find("base_currency");
    if (it != props.end()) m_CurrencyBaseName = it->second;

    it = props.find("min_batches");
    if (it != props.end()) m_MinBatches = std::stoi(it->second);

    it = props.find("max_batches");
    if (it != props.end()) m_MaxBatches = std::stoi(it->second);

    it = props.find("min_per_batch");
    if (it != props.end()) m_MinPerBatch = std::stoi(it->second);

    it = props.find("max_per_batch");
    if (it != props.end()) m_MaxPerBatch = std::stoi(it->second);

    it = props.find("spacing_in_batch");
    if (it != props.end()) m_SpacingInBatch = std::stoi(it->second);

    it = props.find("max_hearts");
    if (it != props.end()) m_MaxHearts = std::stoi(it->second);
    
    it = props.find("health");
    if (it != props.end()) m_Health = std::stoi(it->second);
}

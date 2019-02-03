#include <memory>

#include "Scene.h"

#include "scene/Actor.h"
#include "scene/ActorComponent.h"
#include "window/Window.h"

milk::Scene::Scene()
        : ended_(false)
{
}

milk::Scene::~Scene() = default;

milk::Actor* milk::Scene::spawnActor(const std::string& name)
{
    int id = idGenerator_.popId();

    auto pActor = std::make_unique<Actor>(*this, id, name, Vector2d(0, 0));

    auto pActorRaw = pActor.get();

    if (ended_)
        return pActorRaw;

    actorsToSpawn_.emplace_back(std::move(pActor));

    return pActorRaw;
}

bool milk::Scene::destroyActor(int id)
{
    auto foundActor = actorsById_.find(id);

    if (foundActor == actorsById_.end())
        return false;

    actorsToDestroy_.emplace_back(id);

    return true;
}

milk::Actor* milk::Scene::findActor(const std::string& name) const
{
    // TODO: Find a better solution than brute force implementation.
    for (auto& itr : actorsToSpawn_)
    {
        if (itr->name() == name)
            return itr.get();
    }

    for (auto& it : actorsById_)
    {
        if (it.second->name() == name)
            return it.second.get();
    }

    return nullptr;
}

milk::Camera& milk::Scene::camera()
{
    return camera_;
}

milk::Tilemap& milk::Scene::tilemap()
{
    return tilemap_;
}

milk::Rectangle milk::Scene::bounds() const
{
    return { 0, 0, tilemap_.width, tilemap_.height };
}

void milk::Scene::end()
{
    ended_ = true;
}

milk::Actor* milk::Scene::pollSpawned()
{
    if (actorsToSpawn_.empty())
        return nullptr;

    auto& pSpawned = actorsToSpawn_.back();

    auto pSpawnedRaw = pSpawned.get();

    actorsById_.insert(std::make_pair(pSpawned->id(), std::move(pSpawned)));

    actorsToSpawn_.pop_back();

    return pSpawnedRaw;
}

milk::Actor* milk::Scene::pollDestroyed()
{
    static int lastPolledId = -1;

    if (actorsToDestroy_.empty())
    {
        lastPolledId = -1;
        return nullptr;
    }

    if (lastPolledId > -1)
        actorsById_.erase(lastPolledId);

    int destroyedId = actorsToDestroy_.back();

    lastPolledId = destroyedId;

    actorsToDestroy_.pop_back();

    return actorsById_.at(destroyedId).get();
}

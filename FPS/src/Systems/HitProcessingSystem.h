#pragma once
#include <GLFW/glfw3.h>

#include <ECS.h>
#include <Components/MovementComponent.h>
#include <Components/PlayerComponent.h>
#include <Components/ObjectComponent.h>
#include <Events/HitEvent.h>

using namespace ECS;

// 根据键盘输入完成行走、跳等操作
class HitProcessingSystem : public EntitySystem, public EventSubscriber<HitEvent> {
public:

    float speed_multiples;
    bool forward, backward, left, right, speed_up, jump;

    virtual void configure(class World* world) override
    {
        world->subscribe<HitEvent>(this);
    }

    virtual void unconfigure(class World* world) override
    {
        world->unsubscribeAll(this);
    }

    virtual void receive(class World* world, const HitEvent& event) override
    {
        Entity* hit_entity = event.hit_entity;
        glm::vec3 hit_position = event.hit_position;
        printf("receive hit event\n");
        printf("----------------------\n");
        printf("ray hit body(entity id: %d)\n", hit_entity->getEntityId());
        printf("hit position: (%f, %f, %f)\n", hit_position.x, hit_position.y, hit_position.z);
        printf("----------------------\n");
    }

    //virtual void tick(class World* world, float deltaTime) override {

    //}
};
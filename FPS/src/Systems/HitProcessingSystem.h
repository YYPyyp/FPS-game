#pragma once
#include <GLFW/glfw3.h>

#include <ECS.h>
#include <Components/MovementComponent.h>
#include <Components/PlayerComponent.h>
#include <Components/ObjectComponent.h>
#include <Systems/ParticleSystem.h>

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
		glm::vec3 hit_direction = event.hit_direction;

		auto windowCHandle = world->getSingletonComponent<WindowInfoSingletonComponent>();
		ComponentHandle<HPComponent> HPCHandle = hit_entity->get<HPComponent>();

		if (HPCHandle.isValid()) {
			cout << HPCHandle->HP << endl;
			HPCHandle->HP -= 20;
			ParticleSystem::simulateBlood(world, hit_position, hit_direction);

			if (HPCHandle->HP < 0) {
				hit_entity->removeAll();
				windowCHandle->score += 5;

				string tmp;
				ostringstream osstream;
				osstream << "score: " << windowCHandle->score;

				world->emit<TextChangeEvent>({ "score", osstream.str() });

				// destroye animation
				ParticleSystem::simulateDisappearing(world, hit_position);
			}

		}
		else {
			ParticleSystem::simulateSmoke(world, hit_position, hit_direction);
		}

        //printf("receive hit event\n");
        //printf("----------------------\n");
        //printf("ray hit body(entity id: %d)\n", hit_entity->getEntityId());
        //printf("hit position: (%f, %f, %f)\n", hit_position.x, hit_position.y, hit_position.z);
        //printf("hit direction: (%f, %f, %f)\n", hit_direction.x, hit_direction.y, hit_direction.z);
        //printf("----------------------\n");
    }

    //virtual void tick(class World* world, float deltaTime) override {

    //}
};
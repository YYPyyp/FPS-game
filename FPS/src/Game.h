#pragma once
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

#include <Winbase.h>
#include <Shlwapi.h>


#include <Resource.h>
#include <ECS.h>
#include <Components/CollisionComponent.h>
#include <Components/MovementComponent.h>
#include <Components/PositionComponent.h>
#include <Components/TextComponent.h>
#include <Components/ObjectComponent.h>
#include <Components/TextComponent.h>
#include <Components/WindowInfoSingletonComponent.h>
#include <Components/LightingInfoSingletonComponent.h>
#include <Components/SkyboxInfoSingletonComponent.h>
#include <Components/PostComponent.h>
#include <Components/BoneObjectComponent.h>
#include <Components/PlayerComponent.h>
#include <Components/CameraComponent.h>
#include <Components/TransformComponent.h>
#include <Systems/RenderSystem/RenderSystem.h>
#include <Systems/KeyPressingSystem.h>
#include <Systems/MouseMovingSystem.h>
#include <Systems/GUISystem.h>
#include <Systems/MovementSystem.h>
#include <Systems/RecoilSystem.h>
#include <Systems/PlayerActionSystem.h>
#include <Systems/ParticleSystem.h>
#include <Systems/CollisionSystem.h>
#include <Systems/HitProcessingSystem.h>


class Game {
public:
	string gameRootPath;

	World* world;
	GLFWwindow* window;

	Game(GLFWwindow* _window) {
		init(_window);
	}

	void init(GLFWwindow* _window) {
		world = World::createWorld();
		window = _window;

		char exeFullPath[MAX_PATH]; // Full path
		GetModuleFileName(NULL, exeFullPath, MAX_PATH);
		PathRemoveFileSpec(exeFullPath);
		PathRemoveFileSpec(exeFullPath);  // remove bin/FPS.exe
		gameRootPath = (string)exeFullPath;
		std::replace(gameRootPath.begin(), gameRootPath.end(), '\\', '/');

        // Get the data
		Resource resource = Resource(gameRootPath + "/resources/");
		Resource::SkyBoxResource skybox_resource;
		skybox_resource.init();
		
		// Systems
		world->registerSystem(new KeyPressingSystem());
		world->registerSystem(new MouseMovingSystem());
		//world->registerSystem(new CameraMovingSystem(cameraPos));
		world->registerSystem(new RecoilSystem());
		world->registerSystem(new PlayerActionSystem());
        world->registerSystem(new MovementSystem());
        world->registerSystem(new CollisionSystem()); // Must place after movement system
        world->registerSystem(new HitProcessingSystem());
        world->registerSystem(new ParticleSystem());
		world->registerSystem(new RenderSystem(gameRootPath + "/src/Shaders/"));
		world->registerSystem(new GUISystem());  // Must place after render system


		// Singleton components
		world->createSingletonComponent<LightingInfoSingletonComponent>();
		world->createSingletonComponent<WindowInfoSingletonComponent>(window);
		world->createSingletonComponent<SkyboxInfoSingletonComponent>(skybox_resource.vertices, skybox_resource.indices, skybox_resource.textures);

		// Entities
		Entity* wall_a = world->create();
		Entity* wall_b = world->create();
		Entity* player = world->create();
		Entity* ground = world->create();
		Entity* text = world->create();
		Entity* test_post = world->create();  // 以后 post 赋给 gun 的 entity，现在只是测试
		Entity* skeleton_model = world->create();

		Entity* gun = world->create();
		Entity* monster = world->create();

		Entity* hitParticles = world->create();	// 子弹击中物体的粒子效果
		Entity* gunFire = world->create();	// 枪口开火的粒子效果
		Entity* disappear = world->create();	// 怪物消失的粒子效果

		// Load texture resource
		Resource::TextureResource textureResource;
		textureResource.init();

		// Assign the components to entities
		Resource::CubeResource wall_resource, ground_resource;
        wall_resource.init(20.0f, 20.0f, 10.0f, textureResource.container_diffuse, textureResource.container_specular);
        wall_a->assign<ObjectComponent>(wall_resource.vertices, wall_resource.indices, wall_resource.textures);
        wall_a->assign<PositionComponent>(glm::vec3(30.0f, 10.0f, 0.0f));
        wall_a->assign<CollisionComponent>(20.0f, 20.0f, 10.0f);

		wall_resource.init(5.0f, 5.0f, 5.0f, textureResource.container_diffuse, textureResource.container_specular);
		wall_b->assign<ObjectComponent>(wall_resource.vertices, wall_resource.indices, wall_resource.textures);
		wall_b->assign<PositionComponent>(glm::vec3(45.0f, 2.5f, 5.0f));
		wall_b->assign<CollisionComponent>(5.0f, 5.0f, 5.0f);

		ground_resource.init(500.0f, 500.0f, 1.0f, textureResource.ground_diffuse, textureResource.ground_specular);
		ground->assign<ObjectComponent>(ground_resource.vertices, ground_resource.indices, ground_resource.textures);
        ground->assign<PositionComponent>(glm::vec3(0.0f, 0.0f, 0.0f));
        ground->assign<CollisionComponent>(500.0f, 500.0f, 1.0f);

		player->assign<ObjectComponent>(gameRootPath + "/resources/objects/gun/Ak-74.obj", "player");
		player->assign<PositionComponent>(glm::vec3(0.0f, 1.0f, 0.0f));
		player->assign<MovementComponent>(glm::vec3(0.0f, -0.1f, 0.0f), glm::vec3(0.0f, -60.0f, 0.0f));  // 碰撞检测需要，要给个小一点向下的初速度；避免一开始检测不到碰撞掉下去
		player->assign<PlayerComponent>();
		player->assign<TransformComponent>(glm::vec3(2.3f, 2.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.1f,0.1f));
		player->assign<CameraComponent>(glm::vec3(3.0f, 1.0f, 1.0f));
        player->assign<CollisionComponent>(-4.0f, 4.0f, 0.0f, 16.0f, -1.5f, 1.5f);

		text->assign<TextComponent>("bullet_info", "", 20.0f, 12.0f, 0.5f, glm::vec3(0.5, 0.8f, 0.2f), gameRootPath + "/resources/fonts/");

		test_post->assign<PostComponent>(glm::vec3(0.0f, 1.0f, 0.0f), 0.025f);
		



		//gun->assign<ObjectComponent>(gameRootPath + "/resources/bone/AK47/AK47.obj");
		//gun->assign<PositionComponent>(glm::vec3(3.0f, 8.0f, 0.0f));

		gun->assign<ObjectComponent>(gameRootPath + "/resources/objects/gun/Ak-74.obj");
		gun->assign<PositionComponent>(glm::vec3(-20.0f, 10.0f, -10.0f));

		monster->assign<ObjectComponent>(gameRootPath + "/resources/objects/Etin/Etin.obj");
		monster->assign<PositionComponent>(glm::vec3(-4.0f, 0.0f, 0.0f));


		//skeleton_model->assign<BoneObjectComponent>(gameRootPath + "/resources/bone/boblampclean.md5mesh");
		//skeleton_model->assign<PositionComponent>(glm::vec3(10.0f, 10.0f, 0.0f));

		hitParticles->assign<ParticleComponent>(gameRootPath + "/resources/textures/", 1);
        hitParticles->assign<PositionComponent>(glm::vec3());
		gunFire->assign<ParticleComponent>(gameRootPath + "/resources/textures/", 2);
        gunFire->assign<PositionComponent>(glm::vec3());
		disappear->assign<ParticleComponent>(gameRootPath + "/resources/textures/", 3);
        disappear->assign<PositionComponent>(glm::vec3());
	}
};

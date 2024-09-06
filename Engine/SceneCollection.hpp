#include "Scene.h"
#include "ResourceManager.h"

class TestScene : public Scene
{
public:
	TestScene* ConstructScene(ResourceManager rm)
	{
        //Model backpack = rm.CreateResource<Model>("models/backpack/backpack.obj");
        Model* cube = rm.CreateResource<Model>("models/Cube.glb");
        cube->meshes[0].textures.push_back(*rm.CreateResource<Texture>("../Common/resources/wall.jpg"));
        cube->meshes[0].scale = glm::vec3(100, 1, 100);
        //backpack.position = glm::vec3(10, 2, 0);
        cube->position = glm::vec3(0, -10, 0);
        //.SceneObjects["Backpack"] = new GameObject(&backpack);
       
        SceneObjects["Crash"] = new GameObject(new Model("models/crash/crash.obj"), "Crash");
        SceneObjects["Crash"]->model->position = glm::vec3(0, 0, 0);
        SceneObjects["Crash"]->rb->velocity = glm::vec3(1.f, 0, 0);
        SceneObjects["Crash"]->rb->mass = 1000;
        // 
        //SceneObjects["Crash2"] = new GameObject(new Model("models/crash/crash.obj"), "Crash2");
        //GameObject* crash = SceneObjects["Crash2"];
        //for (int i = 0; i < crash->model->meshes.size(); i++)
        //{
        //    crash->model->meshes[i].scale = glm::vec3(10, 1, 5);
        //}
        //SceneObjects["Crash2"]->rb->position = glm::vec3(0, 0, 0);
        //SceneObjects["Crash2"]->rb->velocity = glm::vec3(1.f, 0, 0);
        //SceneObjects["Crash2"]->rb->mass = 1000;
        
        //SceneObjects["Mechabandicoot"] = new GameObject(new Model("models/mechabandicoot/mech.obj"), "Mechacoot");
        //SceneObjects["Mechabandicoot"]->rb->position = glm::vec3(0, 0, 0);
        //SceneObjects["Mechabandicoot"]->rb->mass = 200000;
        //SceneObjects["Mechabandicoot"]->rb->isStatic = true;

        SceneObjects["Floor"] = new GameObject(cube, "Floor");
        SceneObjects["Floor"]->rb->isStatic = true;
        for (auto& i : SceneObjects)
        {
            i.second->rb->collisionModel = i.second->rb->GenerateCollisionShape(i.second->model);
            std::cout << "LOADED: " << i.first << std::endl;
        }
        return this;
	}

    Scene* DestroyScene()
    {
        delete this;
    }
};
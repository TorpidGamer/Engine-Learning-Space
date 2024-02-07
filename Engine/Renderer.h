#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <string>
#include "shader.h"
#include "model.h"
#include "camera.h"
#include <map>
class Renderer
{
public:
	void DrawScene(Shader shader, Camera camera, std::vector<Model*> sceneObjs);
};
#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <string>
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include <map>
class Renderer
{
public:
	void DrawScene(Shader shader, Camera camera, std::vector<Model*> sceneObjs);
};
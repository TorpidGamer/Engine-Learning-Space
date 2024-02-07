#include"Renderer.h"

void Renderer::DrawScene(Shader shader, Camera camera, std::vector<Model*> sceneObjs)
{
    std::map<float, Model*> sortedTransparencies;

    for (int i = 0; i < sceneObjs.size(); i++)
    {
        if (sceneObjs[i]->texturesLoaded[i].channels == 4)
        {
            float distance = glm::length(camera.Position - sceneObjs[i]->meshes[0].position);
            sortedTransparencies[distance] = sceneObjs[i];
        }
        else
        {
            sceneObjs[i]->Draw(shader);
        }
    }
    for (std::map<float, Model*>::reverse_iterator i = sortedTransparencies.rbegin(); i != sortedTransparencies.rend(); ++i)
    {
        i->second->Draw(shader);
    }
}
#pragma once
#include "Model.h"

class ResourceManager
{
public:
    template <typename Resource> Resource* CreateResource(std::string path);
    template <> Model* CreateResource(std::string path);
    template <> Texture* CreateResource(std::string path);
};

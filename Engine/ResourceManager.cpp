#include "ResourceManager.h"

template<typename Resource> Resource ResourceManager::CreateResource(std::string path)
{
	std::cout << "Incompatible type loaded, Seek medical help" << std::endl;
}

template<> Model ResourceManager::CreateResource(std::string path)
{
	return Model(path);
}

template<> Texture ResourceManager::CreateResource(std::string path)
{
	return Texture(path);
}
#include "pch.h"
#include "GameObject.h"


using namespace std;

void GameObject::Begin()
{

}

void GameObject::Tick()
{

}

void GameObject::End()
{

}

std::shared_ptr<spdlog::logger> GameObject::GetLogger()
{
	return GlobalLinkData::Logger;
}

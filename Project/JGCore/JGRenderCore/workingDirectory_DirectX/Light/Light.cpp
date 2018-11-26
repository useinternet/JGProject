#include"Light.h"
#include"SceneRenderSystem/SRSShadowPass.h"
using namespace JGRC;


real Light::ShadowWidth = 512.0f;
real Light::ShadowHeight = 512.0f;
real Light::NearZ = 0.1f;
real Light::FarZ = 1000.0f;
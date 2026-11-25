#pragma once

#include "GameObject.h"
#include "Rendering/Camera.h"

class CameraObject : public GameObject
{
public:
    Camera camera;
};
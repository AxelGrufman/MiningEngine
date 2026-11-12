#pragma once

#include <vector>
#include "Rendering/Renderable.h"

class Scene
{
public:
    void addRenderable(const Renderable& renderable);

    const std::vector<Renderable>& getRenderables() const;

private:
    std::vector<Renderable> renderables;
};
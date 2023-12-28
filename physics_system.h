#pragma once

#include "glm/gtx/string_cast.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "position.h"

template <typename SysSet>
void add_physics_system(SysSet& sys)
{
    sys.template add_system<Position>("physics", [](double dt, auto& parray, auto& logger)
    {
        for (auto& ac : parray)
        {
            if (auto* vcomp = ac.template sib<Velocity>())
            {
                ac->position += vcomp->cmp.velocity*float(dt);
                logger.out_log << ac.entity_id << ": " << glm::to_string(ac->position) << std::endl;
            }
        }
    });
}

template <typename SysSet>
void add_gravity_system(SysSet& sys)
{
    sys.template add_system<Position>("gravity", [](double dt, auto& parray, auto& logger)
    {
        for (auto& ac : parray)
        {
            if (auto* vcomp = ac.template sib<Velocity>())
            {
                vcomp->cmp.velocity += glm::vec3(0.0,0.0,-9.81)*float(dt);
            }
        }
    });
}

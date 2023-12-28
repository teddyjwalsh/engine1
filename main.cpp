#include <iostream>
#include "system_set.h"
#include "component_set.h"
#include "time_system.h"
#include "physics_system.h"
#include "position.h"

struct A 
{
    int x;
    int y;
    float z;
};

int main()
{
    using CS = ComponentSet<
        float, 
        double, 
        A, 
        Time,
        Position,
        Velocity
    >;
    CS x;
    SystemSet<CS> sys;

    x.add_entity<Time>();
    x.add_entity<Position,Velocity>();
    x.add_entity<Position,Velocity>();

    auto fdata = x.get_array<double>();
    for (auto& d : fdata)
    {
        auto* dsib = d.sib<A>();
        std::cout << dsib->cmp.x << std::endl;
    }

    sys.update(0.1, &x.arrays);

    add_time_system(sys);
    add_gravity_system(sys);
    add_physics_system(sys);

    while (1)
    {
        sys.update(0.1, &x.arrays);
    }
}

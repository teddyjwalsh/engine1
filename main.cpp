#include <iostream>
#include <chrono>
#include "system_set.h"
#include "component_set.h"
#include "time_system.h"
#include "physics_system.h"
#include "position.h"
#include "octree.h"
#include "aabb.h"

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
        Velocity,
        AABB,
        Octree<AABB>
    >;
    CS x;
    SystemSet<CS> sys;

    x.add_entity<Time>();
    x.add_entity<Position,Velocity,AABB>();
    x.add_entity<Position,Velocity,AABB>();

    Octree<AABB> test_oct(glm::vec3(0,0,0), glm::vec3(1,1,1));
    test_oct.insert_data(AABB{glm::vec3(0.09,0.18,0.25), glm::vec3(0.1,0.2,0.3)}, AABB{glm::vec3(0.09,0.18,0.25), glm::vec3(0.1,0.2,0.3)});

    AABB ab{glm::vec3(-0.5,-0.5,-0.5), glm::vec3(1.5,1.5,1.5)};
    test_oct.insert_data(ab, ab);

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
    add_octree_system(sys);

    auto t_start = std::chrono::high_resolution_clock::now();

    while (1)
    {
        auto t_now = std::chrono::high_resolution_clock::now();
        double elapsed_time = std::chrono::duration<double, std::micro>(t_now-t_start).count()/1.0e6;
        t_start = t_now;
        sys.update(elapsed_time, &x.arrays);
    }
}

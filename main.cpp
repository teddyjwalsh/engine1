#include <iostream>
#include "system_set.h"
#include "component_set.h"
#include "time_system.h"

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
        Time
    >;
    CS x;
    SystemSet<CS> sys;

    x.add_entity<float,A>();

    auto fdata = x.get_array<double>();
    for (auto& d : fdata)
    {
        auto* dsib = d.sib<A>();
        std::cout << dsib->cmp.x << std::endl;
    }

    sys.update(0.1, &x.arrays);

    add_time_system(sys);

    while (1)
    {
        sys.update(0.1, &x.arrays);
    }
}

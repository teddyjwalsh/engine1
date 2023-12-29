#pragma once

struct Time
{
    double t;
};

template <typename SysSet>
void add_time_system(SysSet& sys)
{
    sys.template add_system<Time>("time", [](double dt, auto& aarray, auto& logger)
    {
        for (auto& ac : aarray)
        {
            ac->t += dt;
            logger.out_log << "t: " << ac->t << std::endl;
        }
    });
}

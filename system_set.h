#pragma once

#include <functional>
#include <fstream>
#include <vector>
#include <map>

//#include "component_set2.h"

struct Logger
{
    std::ofstream out_log;

    Logger(std::string in_name):
        out_log(in_name + ".log")
    {
    }
};

template <typename ComponentSetType>
struct SystemSet
{

    std::map<std::string, std::function<void(double, typename ComponentSetType::ArraysType*, Logger&)>> systems;
    std::unordered_map<std::string, Logger> loggers;

    //template <typename FirstType>
    //void add_system(const std::function<void(std::vector<ComponentSetType::Component<FirstType>>&)>& in_system)
    template <typename FirstType>
    void add_system(const std::string& in_name, const std::function<void(double, std::vector<typename ComponentSetType::template Component<FirstType>>&, Logger& logger)>& in_system)
    {
        systems[in_name] = [in_system](double dt, typename ComponentSetType::ArraysType* arrays, Logger& logger)
        {
            in_system(dt, std::get<typename ComponentSetType::template Array<FirstType>>(*arrays).data, logger);
        };
        loggers.insert(std::make_pair(in_name, Logger(in_name)));
    }


    void update(double dt, typename ComponentSetType::ArraysType* arrays)
    {
        for (auto& [n,s] : systems)
        {
            loggers.at(n).out_log << "Starting " << n << std::endl;
            s(dt, arrays, loggers.at(n));
        }
    }
};

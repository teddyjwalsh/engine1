#pragma once

#include <cstddef>
#include <vector>
#include <unordered_map>
#include <tuple>

#include "time_system.h"

using EntityId = int;
using ComponentIndex = size_t;

template <typename... Types>
struct ComponentSet
{
    template <typename C>
    struct Array;

    using ArraysType = std::tuple<Array<Types>...>;
    EntityId id_counter = 0;

    template <typename C>
    struct Component
    {
    private:
        ArraysType* arrays;

    public:
        EntityId entity_id;
        C cmp;
        Component(EntityId in_id, ArraysType& in_arrays):
            entity_id(in_id),
            arrays(&in_arrays)
        {
            
        }

        C* operator->()
        {
            return &cmp;
        }

        template <typename K>
        Component<K>* sib()
        {
            auto& arr = std::get<Array<K>>(*arrays);
            if (arr.entity_to_component.find(entity_id) == arr.entity_to_component.end())
            {
                return nullptr;
            }
            return &arr.data[arr.entity_to_component[entity_id]];
        }

        double get_time()
        {
            return std::get<Array<Time>>(*arrays).data[0].t;
        }
    };

    template <typename C>
    struct Array
    {
        std::vector<Component<C>> data;
        std::unordered_map<EntityId, ComponentIndex> entity_to_component;
        std::unordered_map<ComponentIndex, EntityId> component_to_entity;

        ComponentIndex add_component(EntityId entity_id, ArraysType& in_arrays)
        {
            data.push_back(Component<C>(entity_id, in_arrays));
            entity_to_component[entity_id] = data.size() - 1;
            component_to_entity[data.size() - 1] = entity_id;
            return data.size() - 1;
        }

        void delete_component(EntityId entity_id)
        {
            if (entity_to_component.find(entity_id) == entity_to_component.end())
            {
                return;
            }
            auto idx = entity_to_component[entity_id];
            data[idx] = data.back();
            data.resize(data.size() - 1);
            entity_to_component.erase(entity_id);
            component_to_entity.erase(idx);
        }
    };

    ArraysType arrays;

    template <typename... EntityTypes>
    EntityId add_entity()
    {
        (std::get<Array<EntityTypes>>(arrays).add_component(id_counter, arrays), ...);
        id_counter++;
    }

    void delete_entity(EntityId eid)
    {
        (std::get<Array<Types>>(arrays).delete_component(eid), ...);
    }

    template <typename C>
    void delete_component(EntityId eid)
    {
        std::get<Array<C>>(arrays).delete_component(eid);
    }
    
    template <typename C>
    std::vector<Component<C>>& get_array()
    {
        return std::get<Array<C>>(arrays).data;
    }
};

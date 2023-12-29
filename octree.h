#pragma once

#include <unordered_set>
#include "glm/glm.hpp"
#include "aabb.h"

using NodeIndex = size_t;
using DataIndex = size_t;
using ChildIndex = int;
constexpr NodeIndex InvalidNodeIndex = 0;
constexpr DataIndex InvalidDataIndex = 0;
constexpr int NumDataPerNode = 8;

bool aabb_intersect(const glm::vec3& amin, const glm::vec3& amax, const glm::vec3& bmin, const glm::vec3& bmax)
{
    return (amin.x <= bmax.x && amax.x >= bmin.x) &&
           (amin.y <= bmax.y && amax.y >= bmin.y) &&
           (amin.z <= bmax.z && amax.z >= bmin.z);
}

bool point_in_aabb(const glm::vec3& amin, const glm::vec3& amax, const glm::vec3& point)
{
    return (point.x <= amax.x && point.y <= amax.y && point.z <= amax.z &&
            point.x >= amin.x && point.y >= amin.y && point.z >= amin.z);
}

glm::vec3 child_to_vec(ChildIndex in_child)
{
    return glm::vec3(int(bool(in_child & 1)), int(bool(in_child & 2)), int(bool(in_child & 4)));
}

ChildIndex vec_to_child(const glm::vec3& in_vec)
{
    return int(in_vec.x > 0) + int(in_vec.y > 0)*2 + int(in_vec.z > 0)*4;
}

struct OctreeNode
{
    std::array<NodeIndex, 8> children;
    std::array<DataIndex, NumDataPerNode> data;
    int num_data;

    OctreeNode()
    {
        children.fill(InvalidNodeIndex);
        data.fill(InvalidDataIndex);
        num_data = 0;
    }

    void put_data(DataIndex in_data)
    {
        if (num_data >= NumDataPerNode)
        {
        }
        data[num_data] = in_data;
        num_data++;
    }
};

template <typename DataType>
struct Octree
{
    std::vector<OctreeNode> nodes;
    std::vector<DataType> data; 
    std::vector<AABB> data_boxes;
    NodeIndex root;
    glm::vec3 tree_loc;
    glm::vec3 tree_size;

    Octree(glm::vec3 in_loc, glm::vec3 in_size):
        tree_loc(in_loc),
        tree_size(in_size)
    {
        nodes.push_back(OctreeNode());
        root = 0;
    }

    DataIndex add_data(const DataType& in_data, const AABB& in_box)
    {
        data.push_back(in_data);
        data_boxes.push_back(in_box);
        return data.size() - 1;
    }

    NodeIndex add_node()
    {
        nodes.push_back(OctreeNode());
        return nodes.size() - 1;
    }

    void insert_data(const DataType& in_data, AABB data_box)
    {
        auto idx = add_data(in_data, data_box);
        std::cout << "P1 " << glm::to_string(data_box.bmax) << ", " << glm::to_string(data_box.bmin) << ", " << glm::length(data_box.bmax - data_box.bmin) << std::endl;
        bool intersects_root = false;
        while (!intersects_root)
        {
            intersects_root = point_in_aabb(tree_loc, tree_loc + tree_size, data_box.bmin);
            if (!intersects_root)
            {
                nodes.push_back(OctreeNode()); 
                nodes.back() = nodes[0];
                auto up_child   = vec_to_child(tree_loc - data_box.bmin);
                auto down_child = vec_to_child(data_box.bmin - tree_loc);
                nodes[0] = OctreeNode();
                tree_loc = tree_loc - child_to_vec(up_child)*tree_size;
                tree_size = tree_size*2.0f;
                nodes[0].children[up_child] = nodes.size() - 1;
                std::cout << "Made bigger " << glm::to_string(tree_loc) << ", " << glm::to_string(tree_size) << std::endl;
            }
        }
        bool inserted = insert_data_recursive(root, tree_loc, tree_size, idx, data_box, glm::length(data_box.bmax - data_box.bmin));
    }

    bool insert_data_recursive(NodeIndex node, const glm::vec3& node_loc, const glm::vec3& node_size, DataIndex in_data_index, const AABB& data_box, float data_size)
    {
        if (glm::length(node_size)*0.5 < data_size)
        {
            std::cout << "Put data " << glm::length(node_size) << ", " << data_size << std::endl;
            nodes[node].put_data(in_data_index);
            return false;
        }
        auto child_size = node_size*0.5f;
        bool intersected = false;
        for (ChildIndex child = 0; child < 8; ++child)
        {
            auto child_loc = node_loc + child_size*child_to_vec(child);
            auto does_intersect = aabb_intersect(child_loc, child_loc + child_size, data_box.bmin, data_box.bmax);
            if (does_intersect)
            {
                if (nodes[node].children[child] == InvalidNodeIndex)
                {
                    std::cout << "Doing child" << std::endl;
                    nodes[node].children[child] = add_node();
                }
                insert_data_recursive(nodes[node].children[child], child_loc, child_size, in_data_index, data_box, data_size);
                intersected = true;
            }
        }
        return intersected;
    }

    std::vector<DataType> get_data_values(AABB data_box)
    {
        std::unordered_set<DataIndex> out_data;
        get_data(root, tree_loc, tree_size, data_box, out_data);
        return std::vector<DataType>(out_data.begin(), out_data.end());
    }

    void get_data(NodeIndex node, const glm::vec3& node_loc, const glm::vec3& node_size, const AABB& data_box, std::unordered_set<DataIndex>& out_data)
    {
        auto does_intersect = aabb_intersect(node_loc, node_loc + node_size, data_box.bmin, data_box.bmax);
        if (!does_intersect)
        {
            return; 
        }
        for (int i = 0; i < nodes[node].num_data; ++i)
        {
            auto& dbox = data_boxes[nodes[node].data[i]];
            if (aabb_intersect(data_box.bmin, data_box.bmax, dbox.bmin, dbox.bmax))
            {
                out_data.insert(nodes[node].data[i]);
            }
        }
        auto child_size = node_size*0.5f;
        for (ChildIndex child = 0; child < 8; ++child)
        {
            if (nodes[node].children[child] != InvalidNodeIndex)
            {
                auto child_loc = node_loc + child_size*child_to_vec(child);
                get_data(nodes[node].children[child], child_loc, child_size, data_box, out_data);
            }
        }
    }
};

template <typename SysSet>
void add_octree_system(SysSet& sys)
{
    sys.template add_system<AABB>("octree", [](double dt, auto& parray, auto& logger)
    {
        for (auto& ac : parray)
        {
            if (auto* vcomp = ac.template sib<Position>())
            {
                logger.out_log << ac.entity_id << ": " << glm::to_string(vcomp->cmp.position) << std::endl;
            }
        }
    });
}

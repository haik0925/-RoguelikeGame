#ifndef HANDLE_H
#define HANDLE_H
#include "definitions.h"
#include <unordered_map>

struct Handle
{
    u16 gen;
    u16 index;

    bool operator==(Handle rhs) const
    {
        return((gen == rhs.gen) && (index == rhs.index));
    }
};

namespace std
{
    template<>
    struct hash < Handle >
    {
        size_t operator()(Handle h) const
        {
            return (hash<u16>()(h.gen) ^ hash<u16>()(h.index));
        }
    };
}

/*Component Manager base class template.*/
template<typename Component>
struct ComponentManager
{
    static const int Max_Count = 1000;
    Component data[Max_Count] = {};
    Handle owner[Max_Count] = {};
    int current_count = 0;
    std::unordered_map<Handle, int> map;//Handle, Component index

    ComponentManager() = default;
    virtual ~ComponentManager() = default;
    ComponentManager(const ComponentManager&) = delete;
    ComponentManager& operator=(const ComponentManager&) = delete;

    Component& Get(Handle h)
    {
        auto found = map.find(h);
        ASSERT(found != map.end());//The entity doesn't have this component

        return data[found->second];
    }

    Component& Create(Handle h)
    {
        auto found = map.find(h);
        ASSERT(current_count < Max_Count);
        ASSERT(found == map.end());
        map.emplace(h, current_count);
        owner[current_count] = h;
        return data[current_count++];
    }

    void Destroy(Handle h)
    {
        auto found = map.find(h);
        if (found != map.end())
        {
            int component_index = found->second;
            int last_index = current_count - 1;
            data[component_index] = data[last_index];
            owner[component_index] = owner[last_index];
            map[owner[component_index]] = component_index;
            --current_count;

            map.erase(h);
        }
    }

};

struct HandleManager
{
    static const int Max_Count = 1000;

    Handle  handles[Max_Count];
    u16     gens[Max_Count] = {};
    i32     current_count = 0;
    u16     free_indices[Max_Count] = {};
    i32     free_count = 0;

    Handle Create()
    {
        u16 index = 0;
        if (free_count > 0)
        {
            index = free_indices[free_count - 1];
            --free_count;
        }
        else
        {
            ASSERT(current_count < Max_Count);
            index = static_cast<u16>(current_count);
            ++current_count;
        }
        return MakeHandle(index);
    }

    Handle MakeHandle(u16 index)
    {
        handles[index].gen = gens[index];
        handles[index].index = index;
        return handles[index];
    }

    bool Alive(Handle h)
    {
        return (gens[h.index] == h.gen);
    }

    void Destroy(Handle h)
    {
        if (Alive(h))
        {
            ++gens[h.index];
            free_indices[free_count++] = h.index;
        }
    }
};

// data  .......
// count .......

#endif//HANDLE_H

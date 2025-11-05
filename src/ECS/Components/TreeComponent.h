//
// Created by Radnom on 3/11/2025.
//

#pragma once
#include "IComponent.h"
#include <vector>

namespace ECS
{
    // This is a Component that connects this Component to other Components in a hierarchy.
    // This doesn't have any access to the other components, so we need to ensure we don't do anything stupid
    // ( like add itself to its children, or create an inheritance loop) in the Controller level.
    class TreeComponent : public IComponent<TreeComponent>
    {
    public:

        int parentId = -1;
	    std::vector<int> childrenIds;

        inline bool HasChild(int _id)
        {
            if (!childrenIds.empty())
            {
                for (int & childrenId : childrenIds)
                {
                    if (childrenId == _id)
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        inline void RemoveChild(int _id)
        {
            if (!childrenIds.empty())
            {
                auto it = childrenIds.begin();
                while (it != childrenIds.end())
                {
                    if ((*it) == _id)
                    {
                        it = childrenIds.erase(it);
                    }
                    else
                    {
                        ++it;
                    }
                }
            }
        }

        inline void AddChild(int _id)
        {
            if (!childrenIds.empty())
            {
                // Ensure we don't add it twice
                for (int & childrenId : childrenIds)
                {
                    if (childrenId == _id)
                    {
                        return;
                    }
                }
            }
            childrenIds.push_back(_id);
        }

        static const char *GetName() { return "TreeComponent"; }
        static bool CanEntityHaveMultiple() { return false; }

    private:
        inline void ClearInternal() override
        {
            parentId = -1;
		    childrenIds.clear();
        }
    };
} // ECS
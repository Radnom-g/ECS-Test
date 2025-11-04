//
// Created by Radnom on 3/11/2025.
//

#pragma once
#include <memory>

namespace ECS
{
    // IComponent uses the 'Curiously Recurring Template Pattern' so that I can get a static Name out of the classes.

    // AbstractComponent Base class has a pure virtual function for cloning.
    // We can also use AbstractComponent when storing our components in memory without needing to declare template,
    // i.e. std::vector<AbstractComponent> components;
    // instead of: std::vector<IComponent<DepthComponent>> components;
    class AbstractComponent
    {
    public:
        virtual ~AbstractComponent () = default;
        virtual std::unique_ptr<AbstractComponent> clone() const = 0;

        // Components will be reused, so they need a Clear function.
        // Derived classes should override ClearInternal for any extra functionality.
        void Clear()
        {
            ClearInternal();
            componentId = -1;
            entityId = -1;
            active = false;
        }

        // Unique ID of this Entity.
        int componentId = -1;

        // The entity this Component belongs to.
        int entityId = -1;

        // Whether this component is currently Active.
        bool active = false;

    protected:
        virtual void ClearInternal() = 0;

    };

    // A Component is a plain datatype with no behaviour.
    template <typename Derived>
    class IComponent : public AbstractComponent
    {
    public:
        ~IComponent() override = default;
    // protected:
    //     // IComponent class needs to be inherited
    //     // Derived classes can move these to public
    //     IComponent() = default;
    //     IComponent(const IComponent&) = default;
    //     IComponent(IComponent&&) = default;
    //     IComponent& operator=(const IComponent&) = default;

    public:
        std::unique_ptr<AbstractComponent> clone() const override
        {
            return std::make_unique<Derived>(static_cast<Derived const&>(*this));
        }

    public:
        static const char* GetName() { return Derived::GetName(); }
        // derived classes MUST implement public 'static const char* GetName() { return "ComponentName";}'
    };
} // ECS
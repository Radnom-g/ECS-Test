# ECS-Test
Creating an Entity-Component-System framework in C++ to learn more about the approach. Using the SFML for rendering and other requirements. Intended for personal research.



Entity:
- The EntityManager has a few big arrays of data which represent Entities.
- Just the index, a bool 'Active', and a string name. 
- Entities don't do anything and can be reused.


Component:
- A class that stores data as simple types in arrays. 
- Each entry in the array is related to an Entity.
- Components can be unique (one Transform max per entity) or multiple (i.e. multiple Sprites on one Entity)
- Ideally, the data arrays would be set to the correct size (determined through playtesting) that it would not need resizing dynamically, but it does support resizing at runtime.
- Ideally, just data - no processing.
- 'Tag' Components are just derived from IComponent but with no extra data. 


System:
- Something that processes the data in Components.
- RenderingSystem grabs the Transform and Sprite components to determine where to render Sprites, and in what order.


World:
- This is a 'scene' of the game, like a level, the menu, etc.
- It creates the systems and components and is a link between them.
- You could have multiple Worlds running at once. 


I wanted to try and get a system working where the physics is on a fixed time step, but the rendering can occur more often and lerp between the previous physics position and the next.



TODO: 
- Add a CollisionComponent
	- a way to say 'use SpriteComponent for collision'? 
- TreeSystem that maintains tree structure and adds/removes children, move this logic out of TreeComponent
- high level ISystem enum state so that systems know if they are early-tick/tick/late-tick/render state and bool for if they have ticked this frame?


FUTURE TODO:
- Have World create all the Components that make sense (ECS World creates Tree/Timer/etc, ECS_SFML World creates Transform, Velocity, Render components/systems)
- Have a System registry
- Systems can say they want to process before/after other Systems, to ensure TransformSystem runs before RenderSystem for example 

- JSON system for reading config (entity capacity values) 
- JSON for serialisation 


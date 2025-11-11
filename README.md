# ECS-Test
Creating an Entity-Component-System framework in C++ to learn more about the approach. Using SFML3 (Simple Fast Media Library) for rendering and other requirements. Intended for personal research.

Goals:
- To learn more about Entity Component System and storing data in contiguous arrays ('structs of arrays') instead of in polymorphic data structures ('arrays of structs')
- To implement a fixed physics timestep with interpolated visuals using the method I read about so many years ago (https://gafferongames.com/post/fix_your_timestep/) 
- To get many entities on-screen that can change their behaviour by adding and removing components on the fly without calling new/delete.


To build, you will need to run CMake, which if you have git installed should grab the required SFML3 libraries. Visual Studio and CLion should both run this automatically if you "Open Folder". 




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
- TransformSystem caches world locations of entities and updates the world transforms of child entities 
- MovementSystem applies acceleration and velocity and repositions actors.
- CollisionSystem checks the world for overlaps. 


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
- Perhaps 'teleportedThisFrame' could be a boolean on TransformSystem (stores if the transform has teleported and should NOT lerp visuals)
- Add a DebugSystem that renders extra debug info! text about entity/comp count, renders collision volumes, etc.
- Add a Tileset Component and System with simple grid collision 

- do a quick test of IndexList::remove_matches

FUTURE TODO:
- Have World create all the Components that make sense (ECS World creates Tree/Timer/etc, ECS_SFML World creates Transform, Velocity, Render components/systems)
- Have a System registry
- Systems can say they want to process before/after other Systems, to ensure TransformSystem runs before RenderSystem for example 
- Perhaps Components should only be accessed by friend systems? Makes it hard to expand though

- JSON system for reading config (entity capacity values) 
- JSON for serialisation 

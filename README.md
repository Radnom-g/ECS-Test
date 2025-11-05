# ECS-Test
Creating an Entity-Component-System framework in C++ to learn more about the approach. Using the SFML for rendering and other requirements. Intended for personal research.



Rendering:
- Entities must have a 'Renderable' component. This has a Depth value which determines the rendering order.
- A single Entity can only render at one depth. If you want it to render at multiple depths, split it into multiple Entities.


- The RenderingSystem has a map of Depth (integer) to a list of Entities. 
- It will attempt to render sprites of a single depth all at once, i.e. if there are thirty entities at depth 40 with a Sprite component of 'Tree' then it will render them all in one go. 


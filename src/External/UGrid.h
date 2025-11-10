/*
     This code was originally written by a StackOverflow user (now deleted) called 'Dragon Energy',
     then found in a github repository here: https://github.com/terrybrash/dragon-space /
     and then modified by Sean Flannigan.

     DragonEnergy said the following about the license:
     "Again feel free to use it however you like, change it, sell stuff you make with it, whatever you want."
     More information can be found here: https://github.com/terrybrash/dragon-space
*/

// The purpose of this Uniform Grid is to provide a quick way to compare collisions of actors in an area
// if the actors are roughly the same size.

// If we have a radius of our largest actor, then we can search in the grid for double that radius and quickly find all
// overlapping entities even if that other entity is stored as a single point in the grid.

// So this grid is really only appropriate for a world of similar-sized entities!
// See 'https://github.com/terrybrash/dragon-space/blob/main/grid-loose.cpp' for another approach
// that uses a coarse and fine grid approach to better handle entities of different sizes.

#pragma once

#include "SmallList.h"

namespace Grid
{
    // Something in the Grid.
    // Single-linked list with the element stores as an int ID.
    struct UGridElement
    {
        // Stores the next element in the cell.
        int next;

        // Stores the ID of the element. This can be used to associate external
        // data to the element.
        int id;

        // Stores the center position of the uniformly-sized element.
        float mx, my;
    };

    struct UGridRow
    {
        // Stores all the elements in the grid row.
        FreeList<UGridElement> elements;

        // Stores all the cells in the row. Each cell stores an index pointing to
        // the first element in that cell, or -1 if the cell is empty.
        int* cells;

        // Stores the number of elements in the row.
        int num_elements;
    };

    struct UGrid
    {
        // Stores all the rows in the grid.
        UGridRow* rows;

        // Stores the number of columns, rows, and cells in the grid.
        int num_cols, num_rows, num_cells;

        // Stores the inverse size of a cell.
        float inv_cell_w, inv_cell_h;

        // Stores the half-size of all elements stored in the grid.
        float element_radius;

        // Stores the upper-left corner of the grid.
        float x, y;

        // Stores the size of the grid.
        float w, h;
    };

    // Returns a new grid storing elements that have a uniform upper-bound size. Because
    // all elements are treated uniformly-sized for the sake of search queries, each one
    // can be stored as a single point in the grid.
    UGrid* ugrid_create(float element_radius, float cell_w, float cell_h,
                        float left, float top, float right, float bottom);

    // Destroys the grid.
    void ugrid_destroy(UGrid* grid);

    // Returns the grid cell X index for the specified position.
    int ugrid_cell_x(const UGrid* grid, float x);

    // Returns the grid cell Y index for the specified position.
    int ugrid_cell_y(const UGrid* grid, float y);

    // Inserts an element to the grid. returns cell index.
    int ugrid_insert(UGrid* grid, int id, float mx, float my);

    // Removes an element from the grid.
    void ugrid_remove(UGrid* grid, int id, float mx, float my);

    // Moves an element in the grid from the former position to the new one.
    void ugrid_move(UGrid* grid, int id, float prev_mx, float prev_my, float mx, float my);

    // Returns all the element IDs that intersect the specified rectangle excluding
    // elements with the specified ID to omit.
    SmallList<int> ugrid_query(const UGrid* grid, float mx, float my, float element_radius, int omit_id);

    // Returns true if the specified element position is inside the grid boundaries.
    bool ugrid_in_bounds(const UGrid* grid, float mx, float my);

    // Returns the position and size of the grid cell associated with a world position.
    // Generally for debug.
    void ugrid_get_rect(UGrid* grid, float mx, float my, float &x, float &y, float &w, float &h );

    // Intended for Debug
    // get the x,y, and contents for cells with contents.
    void ugrid_get_contents(UGrid* grid, SmallList<int>& _outContents, SmallList<int>& _outGridX, SmallList<int>& _outGridY);

    // Optimizes the grid, rearranging the memory of the grid to allow cache-friendly
    // cell traversal.
    // Can be called at the end of the frame.
    void ugrid_optimize(UGrid* grid);
}
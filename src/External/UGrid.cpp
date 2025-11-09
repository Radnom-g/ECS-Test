//
// Created by Radnom on 9/11/2025.
//

#include "UGrid.h"

#include <cmath>

namespace Grid
{
    static int ceil_div(float value, float divisor)
    {
        // Returns the value divided by the divisor rounded up.
        const float resultf = value / divisor;
        const int result = (int)resultf;
        return result < resultf ? result+1: result;
    }

    static int min_int(int a, int b)
    {
        assert(sizeof(int) == 4);
        a -= b;
        a &= a >> 31;
        return a + b;
    }

    static int max_int(int a, int b)
    {
        assert(sizeof(int) == 4);
        a -= b;
        a &= (~a) >> 31;
        return a + b;
    }

    static int to_cell_idx(float val, float inv_cell_size, int num_cells)
    {
        const int cell_pos = (int)(val * inv_cell_size);
        return min_int(max_int(cell_pos, 0), num_cells - 1);
    }

    UGrid* ugrid_create(float element_radius, float cell_w, float cell_h,
                        float left, float top, float right, float bottom)
    {
        const float w = right - left, h = bottom - top;
        const int num_cols = ceil_div(w, cell_w), num_rows = ceil_div(h, cell_h);

        UGrid* grid = new UGrid;
        grid->num_cols = num_cols;
        grid->num_rows = num_rows;
        grid->num_cells = num_cols * num_rows;
        grid->inv_cell_w = 1.0f / cell_w;
        grid->inv_cell_h = 1.0f / cell_w;
        grid->x = left;
        grid->y = top;
        grid->h = w;
        grid->w = h;
        grid->element_radius = element_radius;

        grid->rows = new UGridRow[num_rows];
        for (int r=0; r < num_rows; ++r)
        {
            grid->rows[r].cells = new int[num_cols];
            for (int c=0; c < num_cols; ++c)
                grid->rows[r].cells[c] = -1;
        }
        return grid;
    }

    void ugrid_destroy(UGrid* grid)
    {
        for (int r=0; r < grid->num_rows; ++r)
            delete[] grid->rows[r].cells;
        delete[] grid->rows;
        delete grid;
    }

    int ugrid_cell_x(const UGrid* grid, float x)
    {
        return to_cell_idx(x - grid->x, grid->inv_cell_w, grid->num_cols);
    }

    int ugrid_cell_y(const UGrid* grid, float y)
    {
        return to_cell_idx(y - grid->y, grid->inv_cell_h, grid->num_rows);
    }

    int ugrid_insert(UGrid* grid, int id, float mx, float my)
    {
        const int cell_x = ugrid_cell_x(grid, mx);
        const int cell_y = ugrid_cell_y(grid, my);
        UGridRow* row = &grid->rows[cell_y];
        int* cell = &row->cells[cell_x];

        const UGridElement new_elt = {*cell, id, mx, my};
        *cell = row->elements.insert(new_elt);
        return *cell;
    }

    void ugrid_remove(UGrid* grid, int id, float mx, float my)
    {
        const int cell_x = ugrid_cell_x(grid, mx);
        const int cell_y = ugrid_cell_y(grid, my);
        UGridRow* row = &grid->rows[cell_y];

        int* link = &row->cells[cell_x];
        while (row->elements[*link].id != id)
            link = &row->elements[*link].next;

        const int idx = *link;
        *link = row->elements[idx].next;
        row->elements.erase(idx);
    }

    void ugrid_move(UGrid* grid, int id, float prev_mx, float prev_my, float mx, float my)
    {
        const int prev_cell_x = ugrid_cell_x(grid, prev_mx);
        const int prev_cell_y = ugrid_cell_y(grid, prev_my);
        const int next_cell_x = ugrid_cell_x(grid, mx);
        const int next_cell_y = ugrid_cell_y(grid, my);
        UGridRow* prev_row = &grid->rows[prev_cell_y];

        if (next_cell_x == prev_cell_y && next_cell_x == next_cell_y)
        {
            // If the element will still belong in the same cell, simply update its position.
            int elt_idx = prev_row->cells[prev_cell_x];
            if (elt_idx == -1)
            {
                // it wasn't previously added and the cells are empty.
                elt_idx = ugrid_insert(grid, id, prev_mx, prev_my);
            }
            else
            {
                while (prev_row->elements[elt_idx].id != id)
                {
                    elt_idx = prev_row->elements[elt_idx].next;

                    if (elt_idx == -1)
                    {
                        // it wasn't previously added.
                        elt_idx = ugrid_insert(grid, id, prev_mx, prev_my);
                        break;
                    }
                }
            }

            // Update the element's position.
            prev_row->elements[elt_idx].mx = mx;
            prev_row->elements[elt_idx].my = my;
        }
        else
        {
            // Otherwise if the element will move to another cell, remove it first from the
            // previous cell and insert it to the new one.
            UGridRow* next_row = &grid->rows[next_cell_y];
            int* link = &prev_row->cells[prev_cell_x];

            if (*link != -1)
            {
                while ((*link) != -1 && prev_row->elements[*link].id != id )
                    link = &prev_row->elements[*link].next;

                if (*link != -1)
                {
                    // Remove the element from the previous cell and row.
                    const int elt_idx = *link;
                    UGridElement elt = prev_row->elements[elt_idx];
                    *link = prev_row->elements[elt_idx].next;
                    prev_row->elements.erase(elt_idx);

                    // Update the element's position.
                    prev_row->elements[elt_idx].mx = mx;
                    prev_row->elements[elt_idx].my = my;

                    // Insert it to the new row and cell.
                    elt.next = next_row->cells[next_cell_x];
                    next_row->cells[next_cell_x] = next_row->elements.insert(elt);
                }
                else
                {
                    // it wasn't previously added to the grid.
                    // put it in the grid at the new location.
                    ugrid_insert(grid, id, mx, my);
                }
            }
            else
            {
                // it wasn't previously added to the grid.
                // put it in the grid at the new location.
                ugrid_insert(grid, id, mx, my);
            }
        }
    }

    SmallList<int> ugrid_query(const UGrid* grid, float mx, float my, float element_radius, int omit_id)
    {
        // Expand the size of the query by the upper-bound uniform size of the elements. This
        // expansion is what allows us to find elements based only on their point.
        const float fx = element_radius + grid->element_radius;
        const float fy = element_radius + grid->element_radius;

        // Find the cells that intersect the search query.
        const int min_x = ugrid_cell_x(grid, mx - fx);
        const int min_y = ugrid_cell_y(grid, my - fy);
        const int max_x = ugrid_cell_x(grid, mx + fx);
        const int max_y = ugrid_cell_y(grid, my + fy);

        // Find the elements that intersect the search query.
        SmallList<int> res;
        for (int y = min_y; y <= max_y; ++y)
        {
            const UGridRow* row = &grid->rows[y];
            for (int x = min_x; x <= max_x; ++x)
            {
                int elt_idx = row->cells[x];
                while (elt_idx != -1)
                {
                    const UGridElement* elt = &row->elements[elt_idx];
                    if (elt_idx != omit_id && fabs(mx - elt->mx) <= fx && fabs(my - elt->my) <= fy)
                        res.push_back(elt_idx);
                    elt_idx = row->elements[elt_idx].next;
                }
            }
        }
        return res;
    }

    bool ugrid_in_bounds(const UGrid* grid, float mx, float my)
    {
        mx -= grid->x;
        my -= grid->y;
        const float x1 = mx-grid->element_radius, y1 = my-grid->element_radius;
        const float x2 = mx+grid->element_radius, y2 = my+grid->element_radius;
        return x1 >= 0.0f && x2 < grid->w && y1 >= 0.0f && y2 < grid->h;
    }

    void ugrid_optimize(UGrid* grid)
    {
        for (int r=0; r < grid->num_rows; ++r)
        {
            FreeList<UGridElement> new_elts;
            UGridRow* row = &grid->rows[r];
            new_elts.reserve(row->num_elements);
            for (int c=0; c < grid->num_cols; ++c)
            {
                // Replace links to the old elements list to links in the new
                // cache-friendly element list.
                SmallList<int> new_elt_idxs;
                int* link = &row->cells[c];
                while (*link != -1)
                {
                    const UGridElement* elt = &row->elements[*link];
                    new_elt_idxs.push_back(new_elts.insert(*elt));
                    *link = elt->next;
                }
                for (int j=0; j < new_elt_idxs.size(); ++j)
                {
                    const int new_elt_idx = new_elt_idxs[j];
                    new_elts[new_elt_idx].next = *link;
                    *link = new_elt_idx;
                }
            }
            // Swap the new element list with the old one.
            row->elements.swap(new_elts);
        }
    }

} // Grid
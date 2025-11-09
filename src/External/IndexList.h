//
// Created by Radnom on 9/11/2025.
//
//
// This code is based on 'IndexList' from a StackOverflow user (now deleted) called 'Dragon Energy'.
// DragonEnergy said the following about the license:
// "Again feel free to use it however you like, change it, sell stuff you make with it, whatever you want."
// More information can be found here: https://github.com/terrybrash/dragon-space
//
// I have modified the list to remove the template type and return -1 if an index is out of bounds.
//
//

#pragma once

#include <cstdlib>

/// Stores a random-access sequence of elements similar to vector, but avoids
/// heap allocations for small lists.
/// It also just returns -1 for invalid access.

class IndexList
{
private:

    class Iterator
    {
    private:
        int index = 0;
        const IndexList* const list;
    public:
        explicit Iterator(const IndexList* _ref, int _index) : index(_index), list(_ref) {}
        int operator*() const { return (*list)[index]; }
        Iterator& operator++()
        {
            index++;
            return *this;
        }
        bool operator !=(const Iterator& other) const
        {
            return index != other.index;
        }

    };

public:
    // Creates an empty list.
    inline IndexList() = default;

    // Creates a copy of the specified list.
    inline IndexList(const IndexList& other);

    // Copies the specified list.
    inline IndexList& operator=(const IndexList& other);

    // Destroys the list.
    inline ~IndexList();

    // Returns the number of agents in the list.
    [[nodiscard]] inline int size() const;

    // Returns the nth element in the list.
    inline int operator[](int n) const;

    inline Iterator begin() const;
    inline Iterator end() const;

    // Returns an index to a matching element in the list or -1
    // if the element is not found.
    [[nodiscard]] inline int find_index(int element) const;

    // removes all matching elements in the list.
    // returns number of matches found+removed.
    inline int remove_matches(int element);

    // Clears the list.
    inline void clear();

    // Reserves space for n elements.
    inline void reserve(int n);

    // Inserts an element to the back of the list.
    inline void push_back(int element);

    /// Pops an element off the back of the list.
    inline int pop_back();

    // Swaps the contents of this list with the other.
    inline void swap(IndexList& other);

    // Returns a pointer to the underlying buffer.
    inline int* data();

    // Returns a pointer to the underlying buffer.
    [[nodiscard]] inline const int* data() const;

    inline void ToVector(std::vector<int>& vec) const;

private:
    enum {fixed_cap = 256};
    struct ListData
    {
        ListData(): data(buf), num(0), cap(fixed_cap){}

        int buf[fixed_cap]{};
        int* data;
        int num;
        int cap;
    };
    ListData ld;
};

namespace IndexListConstants
{
    const IndexList EmptyList{};
}

// ---------------------------------------------------------------------------------
// IndexList Implementation
// ---------------------------------------------------------------------------------

IndexList::IndexList(const IndexList& other)
{
    if (other.ld.cap == fixed_cap)
    {
        ld = other.ld;
        ld.data = ld.buf;
    }
    else
    {
        reserve(other.ld.num);
        for (int j=0; j < other.size(); ++j)
            ld.data[j] = other.ld.data[j];
        ld.num = other.ld.num;
        ld.cap = other.ld.cap;
    }
}

IndexList& IndexList::operator=(const IndexList& other)
{
    IndexList(other).swap(*this);
    return *this;
}

IndexList::~IndexList()
{
    if (ld.data != ld.buf)
        free(ld.data);
}

int IndexList::size() const
{
    return ld.num;
}

int IndexList::operator[](int n) const
{
    if (n >= 0 && n < ld.num)
        return ld.data[n];
    return -1;
}

inline IndexList::Iterator IndexList::begin() const
{
    if (ld.num == 0)
        return Iterator(this, -1);
    return Iterator(this, 0);
}

inline IndexList::Iterator IndexList::end() const
{
    if (ld.num == 0)
        return Iterator(this, -1);
    return Iterator(this, ld.num);
}

int IndexList::find_index(int element) const
{
    for (int j=0; j < ld.num; ++j)
    {
        if (ld.data[j] == element)
            return j;
    }
    return -1;
}

int IndexList::remove_matches(int element)
{
    int matches = 0;
    for (int j=0; j < ld.num - matches; ++j)
    {
        if (matches > 0 && (j+matches < ld.num))
        {
            ld.data[j] = ld.data[j+matches];
        }
        while (ld.data[j] == element)
        {
            matches++;
            if (j+matches < ld.num)
            {
                ld.data[j] = ld.data[j+matches];
            }
            else
            {
                break;
            }
        }
    }
    ld.num -= matches;
    return matches;
}

void IndexList::clear()
{
    ld.num = 0;
}

void IndexList::reserve(int n)
{
    enum {type_size = sizeof(int)};
    if (n > ld.cap)
    {
        if (ld.cap == fixed_cap)
        {
            ld.data = static_cast<int*>(malloc(n * type_size));
            memcpy(ld.data, ld.buf, sizeof(ld.buf));
        }
        else
            ld.data = static_cast<int*>(realloc(ld.data, n * type_size));
        ld.cap = n;
    }
}

void IndexList::push_back(int element)
{
    if (ld.num >= ld.cap)
        reserve(ld.cap * 2);
    ld.data[ld.num++] = element;
}

int IndexList::pop_back()
{
    return ld.data[--ld.num];
}

void IndexList::swap(IndexList& other)
{
    ListData& ld1 = ld;
    ListData& ld2 = other.ld;

    const int use_fixed1 = ld1.data == ld1.buf;
    const int use_fixed2 = ld2.data == ld2.buf;

    const ListData temp = ld1;
    ld1 = ld2;
    ld2 = temp;

    if (use_fixed1)
        ld2.data = ld2.buf;
    if (use_fixed2)
        ld1.data = ld1.buf;
}

int* IndexList::data()
{
    return ld.data;
}

const int* IndexList::data() const
{
    return ld.data;
}

inline void IndexList::ToVector(std::vector<int> &vec) const
{
    vec.assign(data(), data() + size());
}

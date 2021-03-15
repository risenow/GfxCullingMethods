#pragma once

template<class T>
struct object_distance_bytes
{
    static const size_t object_distance = sizeof(T) % alignof(T) == 0 ? sizeof(T) : sizeof(T) + (alignof(T) - sizeof(T) % alignof(T));
};
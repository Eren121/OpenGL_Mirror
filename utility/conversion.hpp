#pragma once

/// @brief Convert between any 3D vector types.
template<typename T>
T convert_vec3(const auto& in)
{
    T ret;
    ret.x = in.x;
    ret.y = in.y;
    ret.z = in.z;
    
    return ret;
}

template<typename T>
T convert_vec2(const auto& in)
{
    T ret;
    ret.x = in.x;
    ret.y = in.y;
    
    return ret;
}
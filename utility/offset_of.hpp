#pragma once

#include <cstddef>
#include <cstdint>

/// @brief C++ compatible offsetof(), because offsetof() does not work with templates.
template<typename Class, typename FieldType>
std::size_t offset_of(FieldType(Class::*field))
{
    const Class* ptr{nullptr};

    auto offset = reinterpret_cast<std::size_t>(&(ptr->*field));
    return offset;
}
#pragma once

#include <glm/vec2.hpp>

/// @brief Utility for iterating 2D grids.
/// @details
/// Having a fixed count of column, each increment go to the next position, in row-major order by default.
/// The count of rows is infinite.
/// Example:
///     GridPosition line(3);   // *line == {0, 0};
///     line++;                 // *line == {0, 1}
///     line++;                 // *line == {0, 2}
///     line++;                 // *line == {1, 0}
///     line++;                 // *line == {1, 1}
///     // etc...
///
class GridPosition
{
public:
    /// @brief Initialize the position to zero.
    explicit GridPosition(int columns = 0);
   
    /// @brief Increment the position.
    GridPosition& operator++();
    
    /// @name
    /// @brief Get the position.
    /// @remarks You can assign to a value out of the grid. If do so, undefined behavior when calling
    /// methods of this class.
    /// @{
    const glm::ivec2* operator->() const { return &m_pos; }
    glm::ivec2* operator->() { return &m_pos; }
    const glm::ivec2& operator*() const { return m_pos; }
    glm::ivec2& operator*() { return m_pos; }
    /// @}
    
    /// @brief Get the index of the position.
    int getIndex() const;
    
private:
    glm::ivec2 m_pos{0};
    int m_columns;
};


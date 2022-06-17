#include "GridPosition.hpp"

GridPosition::GridPosition(int columns)
    : m_columns(columns)
{
}

GridPosition& GridPosition::operator++()
{
    if(m_pos.x == m_columns - 1)
    {
        // Last column
        // Go to next row
        
        m_pos.x = 0;
        m_pos.y++;
    }
    else
    {
        // Go to next column
        m_pos.x++;
    }
    
    return *this;
}

int GridPosition::getIndex() const
{
    return m_pos.y * m_columns + m_pos.x;
}
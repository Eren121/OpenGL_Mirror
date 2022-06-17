#pragma once

#include "Clock.hpp"

class Timer
{
public:
    Timer(Time amount = {});
    
    bool elapsed() const;
    
private:
    Time m_end;
};


// == Implementation reglimit.cc ==

// -- Own header --

#include "reglimit.hh"

// -- Boost headers --

// -- Standard headers --

// == Implementation ==

namespace multovl {

bool RegLimit::operator<(const RegLimit& other) const
{
    if (this->this_pos() < other.this_pos())
        return true;
    else if (this->this_pos() == other.this_pos())
        return (this->is_first() && !(other.is_first()));
    else
        return false;
}
    
}   // namespace multovl

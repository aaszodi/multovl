// == MODULE wbregion.cc ==

// -- Standard headers --

// -- Boost headers --

// -- Own header --

#include "wbregion.hh"

// == Implementation ==

namespace multovl {

WbRegion::WbRegion(): Region() {}

WbRegion::WbRegion(unsigned int f, unsigned int l,
    char s, const std::string& nm
): 
    Region(f, l, s, nm)
{}

}   // namespace multovl

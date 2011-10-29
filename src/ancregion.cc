// == MODULE ancregion.cc ==

// -- Standard headers --

#include <vector>
#include <sstream>

// -- Boost headers --

#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string.hpp"

// -- Own header --

#include "ancregion.hh"

// == Implementation ==

namespace multovl {

// -- AncestorRegion methods --
    
unsigned int AncestorRegion::track_id(unsigned int trackid)
{
    unsigned int oldid = _trackid;
    _trackid = trackid;
    return oldid;
}

bool AncestorRegion::operator==(const AncestorRegion& rhs) const
{
    return (static_cast<Region>(*this) == static_cast<Region>(rhs) &&
        track_id() == rhs.track_id());
}

bool AncestorRegion::operator<(const AncestorRegion& rhs) const 
{
    return track_id() < rhs.track_id()
        ||
        (track_id() == rhs.track_id() && static_cast<Region>(*this) < static_cast<Region>(rhs));
}

std::string AncestorRegion::to_attrstring() const
{
    std::ostringstream ostr;
    ostr << track_id() << ':' << name() << ':' << strand()
        << ':' << first() << '-' << last();
    return ostr.str();
}

}   // namespace multovl

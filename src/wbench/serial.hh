#ifndef MULTOVL_WBENCH_SERIAL_HEADER
#define MULTOVL_WBENCH_SERIAL_HEADER

// == HEADER serial.hh ==

/** \file 
 * \brief External serialization methods.
 * \author Andras Aszodi
 * \date 2011-10-30.
 */

// -- Own header --

#include "region.hh"

// -- Boost headers --

#include "boost/serialization/string.hpp"
#include "boost/serialization/version.hpp"
#include "boost/serialization/split_free.hpp"

// -- Serialization of various Multovl classes --

namespace boost {
namespace serialization {

template <class Archive>
void save(Archive & ar, const multovl::Region& reg, const unsigned int version)
{
    ar << reg.first() << reg.last() << reg.strand() << reg.name();
}

template <class Archive>
void load(Archive & ar, multovl::Region& reg, const unsigned int version)
{
    unsigned int first, last;
    ar >> first >> last;
    reg.set_coords(first, last);
    char strd;
    ar >> strd;
    reg.strand(strd);
    std::string nm;
    ar >> nm;
    reg.name(nm);
}

}   // namespace serialization
}   // namespace boost

BOOST_SERIALIZATION_SPLIT_FREE(multovl::Region)

// dummy function to give the serial module a body
// more to come later...

namespace multovl {

void dummy_function_do_not_use();

}   // namespace multovl

#endif  // MULTOVL_WBENCH_SERIAL_HEADER

#ifndef MULTOVL_WBREGION_HEADER
#define MULTOVL_WBREGION_HEADER

// == HEADER wbregion.hh ==

/** \file 
 * \brief Genomic regions with coordinates and a name to be used with the Multovl "workbench".
 * \author Andras Aszodi
 * \date 2011-10-30.
 */

// -- Own header --

#include "region.hh"

// -- Boost headers --

#include "boost/serialization/string.hpp"
#include "boost/serialization/version.hpp"
#include "boost/serialization/split_member.hpp"

namespace multovl {

class WbRegion: public Region
{
    public:
    
    /// Inits to empty (makes sense when using as input buffer)
    WbRegion();
    
    /**
     * Inits to contain the genomic region with positions [f..l],
     * strand /s/, name /nm/.
     * Enforces the invariance f<=l
     * If f>l, then the values are swapped silently.
     * \param f the first position of the region
     * \param l the last position of the region
     * \param s the strand indicator. Anything else than '+' or '-' will be
     * converted to '.'
     * \param nm the feature name for the region
     */
    WbRegion(unsigned int f, unsigned int l,
        char s, const std::string& nm);

    private:
    
    friend class boost::serialization::access;
    template <class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
        ar << first() << last() << strand() << name();
    }
    
    friend class boost::serialization::access;
    template <class Archive>
    void load(Archive & ar, const unsigned int version)
    {
        unsigned int first, last;
        ar >> first >> last;
        set_coords(first, last);
        char strd;
        ar >> strd;
        strand(strd);
        std::string nm;
        ar >> nm;
        name(nm);
    }

    template<class Archive>                                         
    void serialize(Archive &ar, const unsigned int file_version)
    {                                                              
        boost::serialization::split_member(ar, *this, file_version);
    }

};

}   // namespace multovl

#endif MULTOVL_SERIALREGION_HEADER

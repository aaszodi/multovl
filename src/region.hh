#ifndef MULTOVL_REGION_HEADER
#define MULTOVL_REGION_HEADER

// == HEADER region.hh ==

/** \file 
 * \brief Genomic regions with coordinates and a name.
 * \author Andras Aszodi
 * \date 2011-05-28.
 */

// -- Standard headers --

#include <string>
#include <iostream>

// -- Boost headers --

#include "boost/operators.hpp"

namespace multovl {

/**
 * \brief Instances of the Region class represent regions on a sequence.
 * They have first and last coordinates, strand information and a name.
 */
class Region:
    boost::less_than_comparable< Region,
    boost::equality_comparable<Region> >
{
    public:
    
    /// Inits to empty (makes sense when using as input buffer)
    Region(): 
        _first(0), _last(0), _length(0),
        _strand(0), _name("")
    {}
    
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
     * Note: the parameters f,l,s will be fixed for the lifetime of the object.
     */
    Region(unsigned int f, unsigned int l,
        char s, const std::string& nm);
    
    /// empty virtual dtor
    virtual ~Region() {}
    
    // -- Setters --
    
    /// Sets the coordinates.
    /// Enforces f<=l. Sets the length as well.
    void set_coords(unsigned int f, unsigned int l);
    
    /// Sets the strand
    void strand(char s);
    
    /// Changes the name of the region
    /// \return old name.
    std::string name(const std::string& nm);
    
    // -- Getters --
    
    /// Returns the first coordinate.
    unsigned int first() const { return _first; }
    
    /// Returns the last coordinate.
    unsigned int last() const { return _last; }
    
    /// Returns the length.
    unsigned int length() const { return _length; }
    
    /// Zero-length regions are considered empty.
    bool is_empty() const { return (_length == 0); }
    
    /// Returns the strand information.
    char strand() const { return _strand; }
    
    /// Returns the name of the region.
    const std::string& name() const { return _name; }
    
    /// \return true if the positions of /*this/ and /other/
    /// are the same, strand information is not considered.
    bool equal_pos(const Region& rhs) const
    {
        return (this->first() == rhs.first() && this->last() == rhs.last());
    }
    
    /// Equality. All fields must be equal.
    /// Equality. Positions and strands must be equal.
    bool operator==(const Region& rhs) const
    {
        return (
            equal_pos(rhs) && 
            this->strand() == rhs.strand() &&
            this->name() == rhs.name()
        );
    }
        
    /// Ordering.
    /// Sorted on first,last coords, strand ('+' < '-' < '.') and the name (lexicographically).
    bool operator<(const Region& rhs) const;
    
    private:
    
    // data
    unsigned int _first, _last, _length;
    char _strand;
    std::string _name;
    
};  // class Region

}   // namespace multovl

#endif  // MULTOVL_REGION_HEADER
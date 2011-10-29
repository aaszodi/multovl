// == MODULE region.cc ==

// -- Own header --

#include "region.hh"

// -- Implementation --

namespace multovl {

// -- Public methods

Region::Region(unsigned int f, unsigned int l,
    char s, const std::string& nm): _name(nm)
{
    set_coords(f, l);
    strand(s);
}

std::string Region::name(const std::string& nm)
{
    std::string oldname = _name;
    _name = nm;
    return oldname;
}

bool Region::operator<(const Region& rhs) const
{
    if (this->first() < rhs.first()) return true;
    if (this->first() == rhs.first())
    {
        if (this->last() > rhs.last()) return true;
        if (this->last() == rhs.last())
        {
            if (this->strand() < rhs.strand()) return true;
            if (this->strand() == rhs.strand())
            {
                return (this->name() < rhs.name());
            }
            else return false;
        }
        else return false;
    }
    return false;
}

// -- Protected methods

void Region::set_coords(unsigned int f, unsigned int l)
{
    if (f <= l)
    {
        _first = f;
        _last = l;
    }
    else
    {
        // swap silently
        _first = l;
        _last = f;
    }
    _length = _last - _first + 1;
}

void Region::strand(char s)
{
    if (s == '+' or s == '-')
        _strand = s;
    else
        _strand = '.';
}

}   // namespace multovl

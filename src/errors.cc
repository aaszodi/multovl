// == MODULE errors.cc ==

// -- Own header --

#include "errors.hh"

// == Implementation ==

namespace multovl {

Errors::Errors(
    const std::string& errprefix,
    const std::string& warnprefix
):
    _errors(), _warnings(),
    _errprefix(errprefix + ": "),
    _warnprefix(warnprefix + ": ")
{
}

void Errors::add_error(const std::string& msg)
{
    if (msg != "")
        _errors.push_back(_errprefix + msg);
}

void Errors::add_warning(const std::string& msg)
{
    if (msg != "")
        _warnings.push_back(_warnprefix + msg);
}

Errors& Errors::operator+=(const Errors& another)
{
    _errors.insert(
        _errors.end(),
        another._errors.begin(),
        another._errors.end()
    );
    _warnings.insert(
        _warnings.end(),
        another._warnings.begin(),
        another._warnings.end()
    );
    return *this;
}

std::ostream& Errors::print(std::ostream& outf, bool warnings) const
{
    std::vector<std::string>::const_iterator it;
    for (it = _errors.begin(); it != _errors.end(); ++it)
        outf << (*it) << std::endl;
    if (error_count() > 0)
        outf << "Error count: " << error_count() << std::endl;
    
    if (warnings)
    {
        for (it = _warnings.begin(); it != _warnings.end(); ++it)
            outf << (*it) << std::endl;
        if (warning_count() > 0)
            outf << "Warning count: " << warning_count() << std::endl;
    }
    return outf;
}

}   // namespace multovl


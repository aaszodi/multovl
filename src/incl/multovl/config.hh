#ifndef CONFIG_HEADER
#define CONFIG_HEADER

// == Header config.hh ==

/// \file config.hh
/// Functions describing the build configuration of Multovl.
/// \date 2024-06-05
/// \author Andras Aszodi

// -- Standard headers --

#include <string>
#include <vector>
#include <map>
#include <variant>

// == Functions ==

namespace multovl {
namespace config {

/// \return "Multovl version X.Y.Z"
std::string versioninfo();

/// \return A multi-line string with all the configuration/version details.
std::string detailed_versioninfo();

/// Checks the version of a third-party library package
/// in a friendly manner (i.e. not throwing exceptions if not found).
/// \param pkgname The name of the library package
/// \return The version string in the format "X.Y.Z" (major-minor-patch numbers)
///     or the empty string "" if the package is not supported.
std::string library_version(const std::string& pkgname);

/// \return A string describing the build type: "Debug" or "Release".
std::string build_type();

/// \return A string specifying the name and version of the C++ compiler.
std::string build_compiler();

/// \return A string describing the system: OS, version and processor architecture.
std::string build_system();

/// \return The number of hardware threads available
unsigned int thread_no();

/// \return A string providing information about the number of threads
std::string num_threads();

/// \return The full Git revision SHA1 checksum or "(unknown)" if not available.
std::string git_full_checksum();

/// \return The short 7-character Git revision SHA1 checksum or "(unknown)" if not available.
std::string git_short_checksum();

/// \return The Git revision date like "2015-01-02 22:32:52 +0100".
/// If Git was not available, then the CMake configuration timestamp is returned.
std::string git_revision_date();

/// \return The number of Git commits since project start
unsigned int git_revision_count();

// == Classes ==

/// Object for storing version information of a particular package.
class Version {
    public:
    
    /// Version string/number
    /// Each of the "major", "minor" and "patch" components
    /// may be a string or an int
    typedef std::variant<std::string, int> ver_t;
        
    /// Set up a Version object when the version numbers are provided.
    /// \param maj The major version string/number
    /// \param min The minor version string/number
    /// \param pat The patch string/number, optional, default empty (ignored)
    explicit Version(
        const ver_t& maj,
        const ver_t& min,
        const ver_t& pat = ""
    );
    
    /// Set up a Version object when the version string is provided
    explicit Version(const std::string& ver): _str{ ver } {}
    
    /// Get the version string
    std::string str() const { return _str; }
    
    private:
    
    static
    std::string _tostr(const ver_t& m);
    
    std::string _str;    ///< format is "major.minor" or "major.minor.patch"

};

/// Stores version information about Multovl and its supporting third-party libraries.
class VersionInfo {
    public:
    
    /// Init VersionInfo.
    VersionInfo();
    
    /// Look up the version for a given package.
    /// \param pkgname The name of the package
    /// \return a Version object with the package details.
    /// \throw std::out_of_range if the package is not known.
    const Version& get(const std::string& pkgname) const { return _vers.at(pkgname); }
    
    /// Construct a full version string
    /// \param pkgname The name of the package
    /// \return a full version string like "pkg version X.Y.Z"
    /// \throw std::out_of_range if the package is not known.
    std::string str(const std::string& pkgname) const;
    
    private:
    
    void add_inplace(
        const std::string& pkgname,
        const Version::ver_t& maj,
        const Version::ver_t& min,
        const Version::ver_t& pat = ""
    );
    void add_inplace(
        const std::string& pkgname,
        const std::string& versionstr
    );
    
    std::map<std::string, Version> _vers;
};

}   // namespace config
}   // namespace multovl

#endif  // CONFIG_HEADER


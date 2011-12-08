#ifndef MULTOVL_PG_PIPELINE_HEADER
#define MULTOVL_PG_PIPELINE_HEADER

// == Header pgpipeline.hh ==

// -- Own headers --

#include "pipeline.hh"
#include "pgmultovlopts.hh"

namespace multovl {

class MultovlOpts;

// -- Classes --

/// The PgPipeline implements the "Postgres-based" MULTOVL pipeline.
/// The inputs are tracks (set of regions) read from a PostgreSQL database,
/// the overlap calculations are serial/single core,
/// the output gets written back to the same DB with a new track name.
/// Database-specific settings are read from a config file.
class PgPipeline: public Pipeline
{
    public:
    
    /// Inits the pipeline with the command-line arguments.
    /// These will be parsed inside and the program exits with an error message
    /// if parsing goes wrong.
    PgPipeline(int argc, char* argv[]);
    
    /// Destructor
    ~PgPipeline();
    
    protected:
    
    /// Reads the input tracks from the database.
    /// The track names are supplied from the command line when the constructor runs.
    /// \return the number of tracks successfully read, 0 on error.
    virtual
    unsigned int read_input();
    
    // The default 1-CPU implementation is used.
    // virtual
    // unsigned int detect_overlaps();
    
    /// Writes the results back to the database.
    virtual
    bool write_output();
    
    /// Base-class sliced access to the option-handling object
    /// which is of type PgMultovlOpts.
    virtual
    MultovlOptbase* opt_ptr() { return _optp; }

    private:
    
    bool read_config();
    std::string inputs_to_anctrack() const;
    static
    std::string ancestors_to_pgarray(const MultiRegion& mr);
    static
    std::string login_name();
    
    // data
    PgMultovlOpts* _optp;
    std::string _rconnstr, _wconnstr,   // read-only and read-write connection strings
        _download_sql, _trackupload_sql, _regionupload_sql;
};

}   // namespace multovl

#endif  // MULTOVL_PG_PIPELINE_HEADER

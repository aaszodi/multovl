/* <LICENSE>
License for the MULTOVL multiple genomic overlap tools

Copyright (c) 2007-2012, Dr Andras Aszodi, 
Campus Science Support Facilities GmbH (CSF),
Dr-Bohr-Gasse 3, A-1030 Vienna, Austria, Europe.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of the Campus Science Support Facilities GmbH
      nor the names of its contributors may be used to endorse
      or promote products derived from this software without specific prior
      written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS
AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
</LICENSE> */
#ifndef MULTOVL_PG_PIPELINE_HEADER
#define MULTOVL_PG_PIPELINE_HEADER

// == Header pgpipeline.hh ==

// -- Own headers --

#include "pipeline.hh"
#include "pgmultovlopts.hh"

namespace multovl {

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
    
    /// \return access to the option-handling object
    PgMultovlOpts* opt_ptr() { return dynamic_cast<PgMultovlOpts*>(opt_pimpl()); }

    private:
    
    bool read_config();
    std::string inputs_to_anctrack() const;
    static
    std::string ancestors_to_pgarray(const MultiRegion& mr);
    static
    std::string login_name();
    
    // data
    std::string _rconnstr, _wconnstr,   // read-only and read-write connection strings
        _download_sql, _trackupload_sql, _regionupload_sql;
};

}   // namespace multovl

#endif  // MULTOVL_PG_PIPELINE_HEADER

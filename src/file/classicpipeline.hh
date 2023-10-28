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
#ifndef MULTOVL_CLASSIC_PIPELINE_HEADER
#define MULTOVL_CLASSIC_PIPELINE_HEADER

// == Header classicpipeline.hh ==

// -- Own headers --

#include "basepipeline.hh"
#include "multioverlap.hh"
#include "classicopts.hh"

// -- Standard headers --

#include <map>
#include <string>

namespace multovl {

// -- Classes --

/// The ClassicPipeline implements the "classic" MULTOVL pipeline.
/// The inputs are files (text or binary),
/// the overlap calculations are serial/single core,
/// the output goes to a GFF2-formatted text file to standard output.
class ClassicPipeline: public BasePipeline
{
public:
    
    /// Inits the pipeline with the command-line arguments.
    /// These will be parsed inside and the program exits with an error message
    /// if parsing goes wrong.
    ClassicPipeline(int argc, char* argv[]);
    
    ~ClassicPipeline();
    
protected:
    
    /// Reads the input tracks from files if no --load option was specified.
    /// The file names are parsed from the command line when the constructor runs.
    /// If the --load <archfile> option was specified, then the complete status
    /// of the program including all input details is read from a binary archive <archfile>.
    /// In this case the input track file name arguments are ignored.
    /// \return the number of tracks successfully read, 0 on error.
    virtual
    unsigned int read_input() override;
    
    /// There is one MultiOverlap object for each chromosome
    typedef std::map<std::string, MultiOverlap> chrom_multovl_map;

    /// \return const access to the chromosome=>multiple overlap map
    const chrom_multovl_map& cmovl() const { return _cmovl; }

    /// \return non-const access to the chromosome=>multiple overlap map
    chrom_multovl_map& cmovl() { return _cmovl; }

    /// Detects the overlaps.
    /// The default implementation provided here uses 1 CPU core.
    /// \return the total number of overlaps found, including solitary regions.
    virtual
    unsigned int detect_overlaps() override;
    
    /// Writes the results to standard output. Format will be decided based on the options.
    /// If the --save <archfile> option was specified, then the complete status of the program
    /// except the results will be serialized to a binary archive <archfile> as well.
    virtual
    bool write_output() override;
    
    /// \return access to the option-handling object
    virtual
    ClassicOpts* opt_ptr() { return dynamic_cast<ClassicOpts*>(opt_pimpl()); }
    
private:
    
    unsigned int read_tracks();
    bool write_gff_output();
    bool write_bed_output();
    void write_comments();
    
    chrom_multovl_map _cmovl;   ///< chromosome ==> MultiOverlap map
};

}   // namespace multovl

#endif  // MULTOVL_CLASSIC_PIPELINE_HEADER

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
#ifndef MULTOVL_PROB_PIPELINE_HEADER
#define MULTOVL_PROB_PIPELINE_HEADER

// == Header probpipeline.hh ==

// -- Own headers --

#include "basepipeline.hh"
#include "stat.hh"
#include "shuffleovl.hh"
#include "probopts.hh"

namespace multovl {
namespace prob {

// -- Classes --

/// The ProbPipeline implements the MULTOVL probability pipeline.
/// The inputs are files (text or binary),
/// the overlap calculations are serial/single core,
/// which are repeated after reshuffling (some) tracks to estimate the probability
/// of overlaps by chance.
class ProbPipeline: public BasePipeline
{
public:
    
    /// Inits the pipeline with the command-line arguments.
    /// These will be parsed inside and the program exits with an error message
    /// if parsing goes wrong.
    ProbPipeline(int argc, char* argv[]);
    
    ~ProbPipeline();
    
protected:
    
    /// Default ctor that does not set the option processing object
    /// used by ParProbPipeline ctor only
    ProbPipeline();
    
    /// Internal class to keep track of overlap lengths.
    /// Basically a multiplicity => total overlap length map
    class OvlenCounter
    {
    public:
        
        /// multiplicity => total overlap length map
        typedef std::map<unsigned int, unsigned int> mtolen_t;
        
        /// Init to empty
        OvlenCounter(): _mtolen() {}
        
        /// Sums the overlap lengths for each multiplicity
        /// \param overlaps a vector of MultiRegion objects
        /// which are the results of a multiple overlap calculation
        void update(const MultiOverlap::multiregvec_t& overlaps);
    
        /// \return const access to the multiplicity => total overlap length map
        const mtolen_t& mtolen() const { return _mtolen; }
        
    private:
    
         mtolen_t _mtolen;
    
    }; // end of class OvlenCounter
    
    /// There is one ShuffleOvl object for each chromosome
    typedef std::map<std::string, ShuffleOvl> chrom_shufovl_map;
    
    /// Reads the input tracks. 
    /// In addition to the "shufflable" tracks specified as positional arguments,
    /// a track containing free regions must also be read. All regions must fall within
    /// these free regions. Optionally, a set of "fixed" tracks may be defined
    /// which won't be shuffled; they are also read here.
    /// \return the total number of tracks successfully read, 0 on error.
    virtual
    unsigned int read_input();
    
    /// Detects overlaps.
    /// First the overlaps without shuffling are calculated, see calc_actual_overlaps().
    /// Then the shufflable tracks
    /// are permuted and the number of overlaps counted again and again which will provide
    /// an estimate of the null distribution (ie. the extent of overlaps by chance).
    /// \return the total number of overlaps found in the unpermuted case.
    virtual
    unsigned int detect_overlaps();
    
    /// Calculates the actual overlaps using the original input tracks without reshuffling.
    /// \return the number of actual overlaps
    unsigned int calc_actual_overlaps();
    
    /// Writes the results to standard output. Only the statistics are printed.
    /// \return true
    virtual
    bool write_output();
    
    /// \return access to the option-handling object
    virtual
    ProbOpts* opt_ptr() { return dynamic_cast<ProbOpts*>(opt_pimpl()); }
    
    /// \return const access to the chrom => ShuffleOvl map
    const chrom_shufovl_map& csovl() const { return _csovl; }
    
    /// \return access to the chrom => ShuffleOvl map
    chrom_shufovl_map& csovl() { return _csovl; }
    
    /// \return const access to the statistics collector object
    const Stat& stat() const { return _stat; }
    
    /// \return access to the statistics collector object
    virtual // because ParProbPipeline needs a thread-safe variant
    Stat& stat() { return _stat; }
    
private:

    chrom_shufovl_map _csovl;   ///< chromosome ==> ShuffleOvl map
    Stat _stat; ///< overlap length statistics collection
    
    unsigned int read_tracks(
        const std::vector<std::string>& inputfiles,
        unsigned int& trackid,
        bool shuffle);
    unsigned int read_free_regions(const std::string& freefile);
    void write_comments() const;
    
};

}   // namespace prob
}   // namespace multovl

#endif  // MULTOVL_PROB_PIPELINE_HEADER

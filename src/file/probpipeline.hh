#ifndef MULTOVL_PROB_PIPELINE_HEADER
#define MULTOVL_PROB_PIPELINE_HEADER

// == Header probpipeline.hh ==

// -- Own headers --

#include "basepipeline.hh"
#include "multovlopts.hh"
#include "empirdistr.hh"
#include "shuffleovl.hh"

namespace multovl {
namespace prob {

class ProbOpts;

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
    
    /// Destructor
    ~ProbPipeline();
    
    protected:
    
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
    /// First the overlaps without shuffling are calculated. Then the shufflable tracks
    /// are permuted and the number of overlaps counted again and again which will provide
    /// an estimate of the null distribution (ie. the extent of overlaps by chance).
    /// \return the total number of overlaps found in the unpermuted case.
    virtual
    unsigned int detect_overlaps();
    
    /// Writes the results to standard output. Only the statistics are printed.
    /// \return true
    virtual
    bool write_output();
    
    /// Base-class sliced access to the option-handling object
    virtual
    MultovlOptbase* opt_ptr() { return _optp; }
    
    // data which should be accessible
    // to derived classes without too much bureaucracy
    chrom_shufovl_map _csovl;   ///< chromosome ==> ShuffleOvl map
    EmpirDistr _nulldistr; ///< null distribution. NOTE: THIS IS FOR TESTING ONLY!!!
    
    private:
    
    unsigned int read_tracks(
        const std::vector<std::string>& inputfiles,
        unsigned int& trackid,
        bool shuffle);
    unsigned int read_free_regions(const std::string& freefile);
    void write_comments() const;
    
    ProbOpts* _optp;
};

}   // namespace prob
}   // namespace multovl

#endif  // MULTOVL_PROB_PIPELINE_HEADER

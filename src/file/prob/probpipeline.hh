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

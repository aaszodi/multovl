#ifndef MULTOVL_CLASSIC_PIPELINE_HEADER
#define MULTOVL_CLASSIC_PIPELINE_HEADER

// == Header classicpipeline.hh ==

// -- Own headers --

#include "pipeline.hh"
#include "classicopts.hh"

namespace multovl {

class MultovlOpts;

// -- Classes --

/// The ClassicPipeline implements the "classic" MULTOVL pipeline.
/// The inputs are files (text or binary),
/// the overlap calculations are serial/single core,
/// the output goes to a GFF2-formatted text file to standard output.
class ClassicPipeline: public Pipeline
{
    public:
    
    /// Inits the pipeline with the command-line arguments.
    /// These will be parsed inside and the program exits with an error message
    /// if parsing goes wrong.
    ClassicPipeline(int argc, char* argv[]);
    
    /// Destructor
    ~ClassicPipeline();
    
    protected:
    
    /// Reads the input tracks from files if no --load option was specified.
    /// The file names are parsed from the command line when the constructor runs.
    /// If the --load <archfile> option was specified, then the complete status
    /// of the program including all input details is read from a binary archive <archfile>.
    /// In this case the input track file name arguments are ignored.
    /// \return the number of tracks successfully read, 0 on error.
    virtual
    unsigned int read_input();
    
    // The default 1-CPU implementation is used.
    // virtual
    // unsigned int detect_overlaps();
    
    /// Writes the results to standard output. Format will be decided based on the options.
    /// If the --save <archfile> option was specified, then the complete status of the program
    /// except the results will be serialized to a binary archive <archfile> as well.
    virtual
    bool write_output();
    
    /// Base-class sliced access to the option-handling object
    /// which is of type ClassicOpts.
    virtual
    MultovlOptbase* opt_ptr() { return _optp; }
    
    private:
    
    unsigned int read_tracks();
    
    bool write_gff_output();
    bool write_bed_output();
    void write_comments();
    
    ClassicOpts* _optp;
};

}   // namespace multovl

#endif  // MULTOVL_CLASSIC_PIPELINE_HEADER

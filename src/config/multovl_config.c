/* == PROGRAM multovl_config.c == */

/**
 * \file Small program to print Multovl configuration information.
 * \author Andras Aszodi
 * \date 2015-01-03
 */

/* -- Own headers -- */

#include "version.h"
#include "build.h"
#include "thirdparty.h"

/* -- Standard headers -- */

#include <stdio.h>
#include <stdlib.h>

/* == MAIN == */

int main(int argc, char* argv[]) {
    printf("MULTOVL configuration, built on %s at %s\n\n", __DATE__, __TIME__);
    
    printf("Version: %s\nGit revision: %s\nRevision date: %s [%s]\n\n",
        config_version(), config_gitrevision(), 
        config_gitrevdate(), config_gitrev()
    );
    
    printf("Build type: %s\nCompiler: %s\nSystem: %s\n\n",
        config_build_type(), config_build_compiler(), config_build_system()
    );

    printf("Third-party libraries:\n%s\n%s\n",
        config_boost_version(), config_bamtools()
    );
    
    exit(EXIT_SUCCESS);
}

/* == END OF PROGRAM multovl_config.c == */

// == PROGRAM config_info.cc ==

/// \file Small program to print Multovl configuration information.
/// \author Andras Aszodi
/// \date 2024-06-10

// -- Own headers --

#include "multovl/config.hh"

// -- Standard headers --

#include <string>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <thread>

// -- Constants --

const char* OPTCHARS = "avbcstGgr";

// -- Functions --

static
void print_help() {
    std::cout << "Usage: config_info -" << OPTCHARS << std::endl
        << "\t-a: Print all information" << std::endl
        << "\t-b: Print build information" << std::endl
        << "\t-c: Print compiler information" << std::endl
        << "\t-s: Print system information" << std::endl
        << "\t-t: Print number of threads available" << std::endl
        << "\t-G: Print full Git SHA1 checksum" << std::endl
        << "\t-g: Print short Git SHA1 checksum" << std::endl
        << "\t-r: Print revision date" << std::endl;
}

// == MAIN ==

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_help();
        std::exit(EXIT_FAILURE);
    }

    // getopt() is still easier than boost::program_option :-(
    signed char choice;
    while((choice = getopt(argc, argv, OPTCHARS)) != -1) {
        if (choice == 'a') {
            std::cout << multovl::config::detailed_versioninfo() << std::endl;
            break;
        }
        switch(choice) {
            case 'v':
                std::cout << multovl::config::versioninfo() << std::endl;
                break;
            case 'b':
                std::cout << multovl::config::build_type() << std::endl;
                break;
            case 'c':
                std::cout << multovl::config::build_compiler() << std::endl;
                break;
            case 's':
                std::cout << multovl::config::build_system() << std::endl;
                break;
            case 't':
                std::cout << multovl::config::num_threads() << std::endl;
                break;
            case 'G':
                std::cout << multovl::config::git_full_checksum() << std::endl;
                break;
            case 'g':
                std::cout << multovl::config::git_short_checksum() << std::endl;
                break;
            case 'r':
                std::cout << multovl::config::git_revision_date() << std::endl;
                break;
            default:
                print_help();
                std::exit(EXIT_FAILURE);
        }
    }
    std::exit(EXIT_SUCCESS);
}

// == END OF PROGRAM config_info.cc ==

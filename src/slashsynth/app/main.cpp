/**
 * \file main.cpp
 * \brief Main application file
 *
 * \author Jaap Versteegh <j.r.versteegh@gmail.com>
 */

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include <slashsynth/version.h>

namespace po = boost::program_options;
namespace fs = std::filesystem;

const po::variables_map& get_program_options();

// Return codes
#define PROGRAM_SUCCESS 0
#define INVALID_COMMAND_LINE 10
#define UNHANDLED_EXCEPTION 11
#define UNKNOWN_EXCEPTION 12

void print_usage(po::options_description& options_description) {
  std::cout << "Usage: slashsynth [<options>] command" << std::endl;
  std::cout << std::endl;
  std::cout << options_description << std::endl;
}

inline void print_version() {
  std::cout << "slashsynth " << STRINGIFY(VERSION) << std::endl;
  std::cout << "   Git revision  : " << STRINGIFY(GITREV) << std::endl;
  std::cout << "   Git date      : " << STRINGIFY(GITDATE) << std::endl;
  std::cout << "   Build date    : " << STRINGIFY(BUILD_DATE) << std::endl;
  std::cout << "   Build type    : " << STRINGIFY(BUILD_TYPE) << std::endl;
  std::cout << "Written by Jaap Versteegh <j.r.versteegh@gmail.com>" << std::endl;
}

po::variables_map vm;

const po::variables_map& get_program_options() {
  return vm;
}

int main(int argc, char* argv[])
{
  try {
    fs::path program_exe{argv[0]};
    program_exe = fs::canonical(program_exe);

    po::options_description desc_args{"Options"};
    desc_args.add_options()
      ("help,h", "display this help and exit")
      ("version,v", "display version info and exit");

    po::options_description command_args{"Command"};
    command_args.add_options()
      ("command", po::value<std::vector<std::string> >(), "command");

    po::positional_options_description pos_args;
    pos_args.add("command", -1);

    po::options_description command_line;
    command_line.add(desc_args).add(command_args);

    po::store(po::command_line_parser(argc, argv).
        options(command_line).positional(pos_args).run(), vm);
    po::notify(vm);

    if (vm.count("version") != 0) {
      print_version();
      return PROGRAM_SUCCESS;
    }

    if (vm.count("help") != 0) {
      print_usage(desc_args);
      return PROGRAM_SUCCESS;
    }

    if (vm.count("command") != 1) {
      std::cerr << "No command given" << std::endl;
      print_usage(desc_args);
      return INVALID_COMMAND_LINE;
    }

    auto command = vm["command"].as<std::vector<std::string>>();
    std::cout << "Running: " << command.at(0) << std::endl;
  }

  catch(std::exception& e) {
    std::cerr << "Exception: " <<  e.what() << std::endl;
    return UNHANDLED_EXCEPTION;
  }
  catch(...) {
    std::cerr << "Unknown exception." << std::endl;
    return UNKNOWN_EXCEPTION;
  }
}


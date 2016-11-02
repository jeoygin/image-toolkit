#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>
#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "config.hpp"
#include "cmd/command_handler.hpp"

using namespace std;

namespace po = boost::program_options;

void print_usage(string app_name, po::options_description desc) {
    cout << "Usage: " << app_name << " [options]" << endl << endl;
    cout << desc << endl;
}

int main(int argc, char** argv) {
    string app_name = boost::filesystem::basename(argv[0]);
    bool verbose = false;
    string cmd, srcpath, dstpath, opsfile, listfile;

    po::options_description desc("Options");
    desc.add_options()
        ("help", "print help messages")
        ("cmd,c", po::value<string>(&cmd), "command")
        ("src,s", po::value<string>(&srcpath), "src path")
        ("dst,d", po::value<string>(&dstpath), "dst path")
        ("list,l", po::value<string>(&listfile), "image list file path")
        ("ops,o", po::value<string>(&opsfile), "operations config file path")
        ("verbose,v", "produce verbose output");

    po::positional_options_description positionalOptions;
    po::variables_map vm;

    try {
        po::store(po::command_line_parser(argc, argv).options(desc)
                  .positional(positionalOptions).run(), vm);

        if (vm.count("help")) {
            print_usage(app_name, desc);
            return 0;
        }

        if (vm.count("verbose")) {
            verbose = true;
        }

        po::notify(vm);
    } catch (boost::program_options::required_option& e) {
        cerr << "ERROR: " << e.what() << endl << endl;
        print_usage(app_name, desc);
        return -1;
    } catch (boost::program_options::error& e) {
        cerr << "ERROR: " << e.what() << endl << endl;
        print_usage(app_name, desc);
        return -1;
    }

    if (srcpath.empty()) {
        print_usage(app_name, desc);
        return -1;
    }

    if (cmd.empty()) {
        cmd = "list";
    }

    ImageDBConfig config(srcpath, dstpath, opsfile, listfile);
    cmd::CommandHandler cmd_handler;
    int ret_code = cmd_handler.process(cmd, config);

    if (ret_code < 0) {
        print_usage(app_name, desc);
        return -1;
    }

    return 0;
}

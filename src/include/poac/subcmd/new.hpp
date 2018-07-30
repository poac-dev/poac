#ifndef POAC_SUBCMD_NEW_HPP
#define POAC_SUBCMD_NEW_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include <boost/filesystem.hpp>

#include "../core/except.hpp"
#include "../io/cli.hpp"
#include "../util/ftemplate.hpp"


namespace poac::subcmd { struct new_ {
    static const std::string summary() { return "Create a new poacpm project."; }
    static const std::string options() { return "<project-name>"; }

    template <typename VS>
    void operator()(VS&& vs) { _main(vs); }
    template <typename VS>
    void _main([[maybe_unused]] VS&& vs) {
        namespace fs     = boost::filesystem;
        namespace except = poac::core::except;

        if (vs.size() != 1)
            throw except::invalid_second_arg("new");
        // Check if the ARGUMENT directory exists.
        else if (const fs::path dir(fs::path(".") / fs::path(vs[0])); fs::is_directory(dir) && fs::exists(dir))
            exists_error(vs[0]);
        else
            exec_new(dir, vs[0]);
    }
    void exists_error(const std::string& arg) {
        std::cerr << poac::io::cli::red
                  << "The "+arg+" directory already exists."
                  << poac::io::cli::reset
                  << std::endl;
        std::exit(EXIT_FAILURE);
    }
    void exec_new(const boost::filesystem::path dir, const std::string& arg) {
        namespace fs = boost::filesystem;
        fs::create_directory(dir);
        std::ofstream ofs;
        std::map<fs::path, std::string> file {
                { ".gitignore", poac::util::ftemplate::_gitignore },
                { "main.cpp",   poac::util::ftemplate::main_cpp },
                { "poac.lock",  poac::util::ftemplate::poac_lock },
                { "poac.yml",   poac::util::ftemplate::poac_yml },
                { "README.md",  poac::util::ftemplate::README_md }
        };
        for (const auto& [name, text] : file) write_to_file(ofs, (dir/name).string(), text);
        echo_notice(arg);
    }
    void write_to_file(std::ofstream& ofs, const std::string& fname, const std::string& text) {
        ofs.open(fname);
        if (ofs.is_open()) ofs << text;
        ofs.close();
        ofs.clear();
    }
    void echo_notice(const std::string& str) {
        std::cout << poac::io::cli::bold
                  << notice(str)
                  << poac::io::cli::reset;
    }
    std::string notice(const std::string& str) {
        return "\n"
               "Your \""+str+"\" project was created successfully.\n"
               "\n"
               "\n"
               "Go into your project by running:\n"
               "    $ cd "+str+"\n"
               "\n"
               "Start your project with:\n"
               "    $ poac install hello_world\n"
               "    $ poac run main.cpp\n"
               "\n";
    }
};} // end namespace
#endif // !POAC_SUBCMD_NEW_HPP

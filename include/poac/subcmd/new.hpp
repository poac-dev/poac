#ifndef POAC_SUBCMD_NEW_HPP
#define POAC_SUBCMD_NEW_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include <boost/filesystem.hpp>

#include "../core/exception.hpp"
#include "../io/file/path.hpp"
#include "../io/cli.hpp"
#include "../util/ftemplate.hpp"


namespace poac::subcmd { struct new_ {
    static const std::string summary() { return "Create a new poacpm project."; }
    // TODO: 自動引数解析
    static const std::string options() { return "<project-name>"; }

    template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    void operator()(VS&& argv) { _main(std::move(argv)); }
    template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    void _main(VS&& argv) {
        namespace fs     = boost::filesystem;

        check_arguments(argv);
        exec_new(argv[0]);
    }

    void check_arguments(const std::vector<std::string>& argv) {
        namespace except = core::exception;
        if (argv.size() != 1)
            throw except::invalid_second_arg("new");
        else if (io::file::path::validate_dir(argv[0]))
            throw except::error("The "+argv[0]+" directory already exists.");
    }

    void exec_new(const boost::filesystem::path& dir) {
        namespace fs = boost::filesystem;
        fs::create_directory(dir);
        std::ofstream ofs;
        std::map<fs::path, std::string> file {
                { ".gitignore", util::ftemplate::_gitignore },
                { "main.cpp",   util::ftemplate::main_cpp },
                { "poac.yml",   util::ftemplate::poac_yml },
                { "README.md",  util::ftemplate::README_md }
        };
        for (const auto& [name, text] : file)
            io::file::path::write_to_file(ofs, (dir/name).string(), text);
        echo_info(dir.string());
    }

    void echo_info(const std::string& str) {
        std::cout << io::cli::bold
                  << info(str)
                  << io::cli::reset;
    }

    std::string info(const std::string& str) {
        return "\n"
               "Your \""+str+"\" project was created successfully.\n"
               "\n"
               "\n"
               "Go into your project by running:\n"
               "    $ cd "+str+"\n"
               "\n"
               "Start your project with:\n"
               "    $ poac install\n"
               "    $ poac run\n"
               "\n";
    }
};} // end namespace
#endif // !POAC_SUBCMD_NEW_HPP

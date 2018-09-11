#ifndef POAC_UTIL_COMPILER_HPP
#define POAC_UTIL_COMPILER_HPP

#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

#include "./command.hpp"


namespace poac::util::compiler {
    template<typename Conf>
    boost::optional<std::vector<std::string>> compile(const Conf& conf) {
        namespace fs = boost::filesystem;

        command cmd(conf.system);
        cmd += conf.version_prefix + std::to_string(conf.cpp_version);
        cmd += "-c";
        for (const auto& s : conf.source_files)
            cmd += s;
        for (const auto& isp : conf.include_search_path)
            cmd += "-I" + isp;
        for (const auto& oa : conf.other_args)
            cmd += oa;
        for (const auto& md : conf.macro_defns)
            cmd += md;
        cmd += "-o";
        std::vector<std::string> obj_files_path;
        for (const auto &s : conf.source_files) {
            const std::string obj_path =
                    (conf.output_path / fs::relative(s)).replace_extension("o").string();
            obj_files_path.push_back(obj_path);
            fs::create_directories(fs::path(obj_path).parent_path());
            cmd += obj_path;
        }

        if (conf.verbose) std::cout << cmd << std::endl;

        if (cmd.exec()) return obj_files_path;
        else            return boost::none;
    }

    template<typename Conf>
    boost::optional<std::string> link(const Conf& conf) {
        const std::string bin_path =
                (conf.output_path / conf.project_name).string();

        command cmd(conf.system);
        for (const auto& o : conf.obj_files_path)
            cmd += o;
        for (const auto& lsp : conf.library_search_path)
            cmd += "-L" + lsp;
        for (const auto& sll : conf.static_link_libs)
            cmd += "-l" + sll;
        for (const auto& oa : conf.other_args)
            cmd += oa;
        cmd += "-o " + bin_path;

        if (conf.verbose) std::cout << cmd << std::endl;

        boost::filesystem::create_directories(conf.output_path);
        if (cmd.exec()) return bin_path;
        else            return boost::none;
    }

    template<typename Conf>
    boost::optional<std::string> gen_static_lib(const Conf& conf) {
        command cmd("ar rcs");
        const std::string stlib_path =
                (conf.output_path / conf.project_name).string() + ".a";
        cmd += stlib_path;
        for (const auto& o : conf.obj_files_path)
            cmd += o;

        if (conf.verbose) std::cout << cmd << std::endl;

        boost::filesystem::create_directories(conf.output_path);
        if (cmd.exec()) return stlib_path;
        else            return boost::none;
    }

    template<typename Conf>
    boost::optional<std::string> gen_dynamic_lib(const Conf& conf) {
        command cmd(conf.system);
        cmd += "-dynamiclib"; // -shared
        for (const auto& o : conf.obj_files_path)
            cmd += o;
        cmd += "-o";
        const std::string dylib_path =
                (conf.output_path / conf.project_name).string() + ".dylib";
        cmd += dylib_path;

        if (conf.verbose) std::cout << cmd << std::endl;

        boost::filesystem::create_directories(conf.output_path);
        if (cmd.exec()) return dylib_path;
        else            return boost::none;
    }
} // end namespace
#endif // !POAC_UTIL_COMPILER_HPP

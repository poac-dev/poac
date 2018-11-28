#ifndef STROITE_CORE_COMPILER_HPP
#define STROITE_CORE_COMPILER_HPP

#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

#include "../../command.hpp"


namespace stroite::core::compiler {
    namespace fs = boost::filesystem;

    template<typename Opts>
    boost::optional<std::vector<std::string>>
    compile(const Opts& opts)
    {
        poac::util::command cmd("cd " + opts.base_dir.string());
        cmd &= opts.system;
        cmd += opts.version_prefix + std::to_string(opts.cpp_version);
        cmd += "-c";
        for (const auto& s : opts.source_files)
            cmd += s;
        for (const auto& isp : opts.include_search_path)
            cmd += "-I" + isp;
        for (const auto& oa : opts.other_args)
            cmd += oa;
        for (const auto& md : opts.macro_defns)
            cmd += md;
        cmd += "-o";
        std::vector<std::string> obj_files_path;
        for (const auto &s : opts.source_files) {
            auto obj_path = opts.output_root / fs::relative(s);
            obj_path.replace_extension("o");
            fs::create_directories(obj_path.parent_path());
            const auto obj_path_str = obj_path.string();
            obj_files_path.push_back(obj_path_str);
            cmd += obj_path_str;
        }

        if (opts.verbose)
            std::cout << cmd << std::endl;

        if (cmd.exec()) return obj_files_path;
        else            return boost::none;
    }

    template<typename Opts>
    boost::optional<std::string>
    link(const Opts& opts)
    {
        const std::string bin_path =
                (opts.output_root / opts.project_name).string();

        poac::util::command cmd(opts.system);
        for (const auto& o : opts.obj_files_path)
            cmd += o;
        for (const auto& lsp : opts.library_search_path)
            cmd += "-L" + lsp;
        for (const auto& sll : opts.static_link_libs)
            cmd += "-l" + sll;
        for (const auto& lp : opts.library_path)
            cmd += lp;
        for (const auto& oa : opts.other_args)
            cmd += oa;
        cmd += "-o " + bin_path;

        if (opts.verbose)
            std::cout << cmd << std::endl;

        fs::create_directories(opts.output_root);
        if (cmd.exec()) return bin_path;
        else            return boost::none;
    }

    template<typename Opts>
    boost::optional<std::string>
    gen_static_lib(const Opts& opts)
    {
        poac::util::command cmd("ar rcs");
        const std::string stlib_path =
                (opts.output_root / opts.project_name).string() + ".a";
        cmd += stlib_path;
        for (const auto& o : opts.obj_files_path)
            cmd += o;

        if (opts.verbose)
            std::cout << cmd << std::endl;

        fs::create_directories(opts.output_root);
        if (cmd.exec()) return stlib_path;
        else            return boost::none;
    }

    template<typename Opts>
    boost::optional<std::string>
    gen_dynamic_lib(const Opts& opts)
    {
        poac::util::command cmd(opts.system);
        cmd += "-dynamiclib"; // -shared
        for (const auto& o : opts.obj_files_path)
            cmd += o;
        cmd += "-o";
        const std::string dylib_path =
                (opts.output_root / opts.project_name).string() + ".dylib";
        cmd += dylib_path;

        if (opts.verbose)
            std::cout << cmd << std::endl;

        fs::create_directories(opts.output_root);
        if (cmd.exec()) return dylib_path;
        else            return boost::none;
    }
} // end namespace
#endif // STROITE_CORE_COMPILER_HPP

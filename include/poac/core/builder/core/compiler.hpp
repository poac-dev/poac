#ifndef POAC_CORE_STROITE_CORE_COMPILER_HPP
#define POAC_CORE_STROITE_CORE_COMPILER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <optional>

#include <boost/filesystem.hpp>

#include "../absorb.hpp"
#include "../../../util/shell.hpp"

namespace poac::core::builder::core::compiler {
    namespace fs = boost::filesystem;

    template <typename Opts>
    std::optional<std::string>
    compile(const Opts& opts, const bool verbose)
    {
        util::shell cmd("cd " + opts.base_dir.string());
        cmd &= opts.system;
        cmd += opts.std_version;
        cmd += "-c";
        cmd += opts.source_file;
        for (const auto& isp : opts.include_search_path)
            cmd += "-I" + isp;
        for (const auto& oa : opts.other_args)
            cmd += oa;
        for (const auto& md : opts.macro_defns)
            cmd += md;

        cmd += "-o";
        auto obj_path = opts.output_root / fs::relative(opts.source_file);
        obj_path.replace_extension("o");
        fs::create_directories(obj_path.parent_path());
        const auto obj_path_str = obj_path.string();
        std::string obj_files_path = obj_path_str;
        cmd += obj_path_str;

        if (verbose) {
            std::cout << cmd << std::endl;
        }

        if (cmd.exec()) {
            return obj_files_path;
        }
        else {
            return std::nullopt;
        }
    }

    template <typename Opts>
    std::optional<std::string>
    link(const Opts& opts, const bool verbose)
    {
        const std::string bin_path =
                (opts.output_root / opts.project_name).string() + absorb::binary_extension;

        util::shell cmd(opts.system);
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

        if (verbose) {
            std::cout << cmd << std::endl;
        }

        fs::create_directories(opts.output_root);
        if (cmd.exec()) {
            return bin_path;
        }
        else {
            return std::nullopt;
        }
    }

    template <typename Opts>
    std::optional<std::string>
    gen_static_lib(const Opts& opts, const bool verbose)
    {
        util::shell cmd("ar rcs");
        const std::string lib_name = "lib" + opts.project_name + ".a";
        const std::string lib_path = (opts.output_root / lib_name).string();
        cmd += lib_path;
        for (const auto& o : opts.obj_files_path)
            cmd += o;

        if (verbose) {
            std::cout << cmd << std::endl;
        }

        fs::create_directories(opts.output_root);
        if (cmd.exec()) {
            return lib_path;
        }
        else {
            return std::nullopt;
        }
    }

    template <typename Opts>
    std::optional<std::string>
    gen_dynamic_lib(const Opts& opts, const bool verbose)
    {
        util::shell cmd(opts.system);
        cmd += absorb::dynamic_lib_option;
        for (const auto& o : opts.obj_files_path)
            cmd += o;
        cmd += "-o";
        const std::string lib_name = "lib" + opts.project_name + absorb::dynamic_lib_extension;
        const std::string lib_path = (opts.output_root / lib_name).string();
        cmd += lib_path;

        if (verbose) {
            std::cout << cmd << std::endl;
        }

        fs::create_directories(opts.output_root);
        if (cmd.exec()) {
            return lib_path;
        }
        else {
            return std::nullopt;
        }
    }
} // end namespace
#endif // POAC_CORE_STROITE_CORE_COMPILER_HPP

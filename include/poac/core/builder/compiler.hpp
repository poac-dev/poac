#ifndef POAC_CORE_STROITE_COMPILER_HPP
#define POAC_CORE_STROITE_COMPILER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <optional>

#include <poac/core/builder/absorb.hpp>
#include <poac/core/builder/options.hpp>
#include <poac/io/path.hpp>
#include <poac/util/shell.hpp>

namespace poac::core::builder::compiler {
    std::optional<std::string>
    compile(const options::compile& opts, const bool verbose) {
        util::shell cmd("cd " + opts.base_dir.string());
        cmd &= opts.system;
        cmd += opts.std_version;
        cmd += "-c";
        cmd += opts.source_file;
        for (const auto& isp : opts.include_search_path) {
            cmd += "-I" + isp;
        }
        for (const auto& oa : opts.other_args) {
            cmd += oa;
        }
        for (const auto& md : opts.definitions) {
            cmd += md;
        }

        cmd += "-o";
        auto obj_path = opts.output_root / io::path::path(opts.source_file).relative_path();
        obj_path.replace_extension("o");
        io::path::create_directories(obj_path.parent_path());
        const std::string obj_files_path = obj_path.string();
        cmd += obj_files_path;

        if (verbose) {
            std::cout << cmd << std::endl;
        }

        if (cmd.exec()) {
            return obj_files_path;
        } else {
            return std::nullopt;
        }
    }

    std::optional<std::string>
    link(const options::link& opts, const bool verbose) {
        const std::string bin_path =
                (opts.output_root / opts.project_name).string() + absorb::binary_extension;

        util::shell cmd(opts.system);
        for (const auto& o : opts.obj_files_path) {
            cmd += o;
        }
        for (const auto& lsp : opts.library_search_path) {
            cmd += "-L" + lsp;
        }
        for (const auto& sll : opts.static_link_libs) {
            cmd += "-l" + sll;
        }
        for (const auto& lp : opts.library_path) {
            cmd += lp;
        }
        for (const auto& oa : opts.other_args) {
            cmd += oa;
        }
        cmd += "-o " + bin_path;

        if (verbose) {
            std::cout << cmd << std::endl;
        }

        io::path::create_directories(opts.output_root);
        if (cmd.exec()) {
            return bin_path;
        } else {
            return std::nullopt;
        }
    }

    std::optional<std::string>
    gen_static_lib(const options::static_lib& opts, const bool verbose) {
        util::shell cmd("ar rcs");
        const std::string lib_name = "lib" + opts.project_name + ".a";
        const std::string lib_path = (opts.output_root / lib_name).string();
        cmd += lib_path;
        for (const auto& o : opts.obj_files_path) {
            cmd += o;
        }

        if (verbose) {
            std::cout << cmd << std::endl;
        }

        io::path::create_directories(opts.output_root);
        if (cmd.exec()) {
            return lib_path;
        } else {
            return std::nullopt;
        }
    }

    std::optional<std::string>
    gen_dynamic_lib(const options::dynamic_lib& opts, const bool verbose) {
        util::shell cmd(opts.system);
        cmd += absorb::dynamic_lib_option;
        for (const auto& o : opts.obj_files_path) {
            cmd += o;
        }
        cmd += "-o";
        const std::string lib_name = "lib" + opts.project_name + absorb::dynamic_lib_extension;
        const std::string lib_path = (opts.output_root / lib_name).string();
        cmd += lib_path;

        if (verbose) {
            std::cout << cmd << std::endl;
        }

        io::path::create_directories(opts.output_root);
        if (cmd.exec()) {
            return lib_path;
        } else {
            return std::nullopt;
        }
    }
} // end namespace
#endif // POAC_CORE_STROITE_COMPILER_HPP

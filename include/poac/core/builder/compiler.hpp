#ifndef POAC_CORE_STROITE_COMPILER_HPP
#define POAC_CORE_STROITE_COMPILER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <numeric>

#include <poac/core/builder/absorb.hpp>
#include <poac/io/filesystem.hpp>
#include <poac/util/shell.hpp>

namespace poac::core::builder {
    namespace options {
        template <typename SinglePassRange, typename T>
        T accumulate(const SinglePassRange& rng, T init) {
            return std::accumulate(std::cbegin(rng), std::cend(rng), init);
        }
        template <typename SinglePassRange, typename T, typename BinaryOp>
        T accumulate(const SinglePassRange& rng, T init, BinaryOp binary_op) {
            return std::accumulate(std::cbegin(rng), std::cend(rng), init, binary_op);
        }

        struct compile {
            std::string system;
            std::string std_version;
            std::string opt_level;
            std::vector<std::string> source_files;
            std::string source_file;
            std::vector<std::string> include_search_path;
            std::vector<std::string> other_args;
            std::vector<std::string> definitions;
            io::filesystem::path base_dir;
            io::filesystem::path output_root;
        };
        std::string to_string(const compile& c) {
            util::shell opts;
            opts += c.std_version;
            opts += "-c";
            opts += accumulate(c.source_files, util::shell());
            opts += accumulate(c.include_search_path, util::shell(),
                               [](util::shell acc, auto s) { return acc + ("-I" + s); });
            opts += accumulate(c.other_args, util::shell());
            opts += accumulate(c.definitions, util::shell());
            opts += "-o";
            for (const auto& s : c.source_files) {
                auto obj_path = c.output_root / io::filesystem::path(s).relative_path();
                obj_path.replace_extension("o");
                io::filesystem::create_directories(obj_path.parent_path());
                opts += obj_path.string();
            }
            return opts.string();
        }

        struct link {
            std::string system;
            std::string project_name;
            io::filesystem::path output_root;
            std::vector<std::string> obj_files_path;
            std::vector<std::string> library_search_path;
            std::vector<std::string> static_link_libs;
            std::vector<std::string> library_path;
            std::vector<std::string> other_args;
        };
        std::string to_string(const link& l) {
            util::shell opts;
            opts += accumulate(l.obj_files_path, util::shell());
            opts += accumulate(l.library_search_path, util::shell(),
                               [](util::shell acc, auto s) { return acc + ("-L" + s); });
            opts += accumulate(l.static_link_libs, util::shell(),
                               [](util::shell acc, auto s) { return acc + ("-l" + s); });
            opts += accumulate(l.library_path, util::shell());
            opts += accumulate(l.other_args, util::shell());
            opts += "-o " + (l.output_root / l.project_name).string();
            return opts.string();
        }

        struct static_lib {
            std::string project_name;
            io::filesystem::path output_root;
            std::vector<std::string> obj_files_path;
        };
        std::string to_string(const static_lib& s) {
            util::shell opts;
            opts += (s.output_root / s.project_name).string() + ".a";
            opts += accumulate(s.obj_files_path, util::shell());
            return opts.string();
        }

        struct dynamic_lib {
            std::string system;
            std::string project_name;
            io::filesystem::path output_root;
            std::vector<std::string> obj_files_path;
        };
        std::string to_string(const dynamic_lib& d) {
            util::shell opts;
            opts += absorb::dynamic_lib_option;
            opts += accumulate(d.obj_files_path, util::shell());
            opts += "-o";
            opts += (d.output_root / d.project_name).string() + absorb::dynamic_lib_extension;
            return opts.string();
        }
    }

    std::optional<std::string>
    compile(const options::compile& opts, const bool verbose) {
        util::shell cmd("cd " + opts.base_dir.string());
        cmd &= opts.system;
        cmd += opts.std_version;
        cmd += "-O" + opts.opt_level;
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
        auto obj_path = opts.output_root / io::filesystem::path(opts.source_file).relative_path();
        obj_path.replace_extension("o");
        io::filesystem::create_directories(obj_path.parent_path());
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

        io::filesystem::create_directories(opts.output_root);
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

        io::filesystem::create_directories(opts.output_root);
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

        io::filesystem::create_directories(opts.output_root);
        if (cmd.exec()) {
            return lib_path;
        } else {
            return std::nullopt;
        }
    }
} // end namespace
#endif // POAC_CORE_STROITE_COMPILER_HPP

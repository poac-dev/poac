#ifndef POAC_CORE_STROITE_COMPILER_HPP
#define POAC_CORE_STROITE_COMPILER_HPP

#include <filesystem>
#include <iostream>
#include <numeric>
#include <optional>
#include <poac/core/builder/absorb.hpp>
#include <poac/util/shell.hpp>
#include <string>
#include <vector>

namespace poac::core::builder {
    namespace options {
        struct compile {
            std::string system;
            std::string std_version;
            std::string opt_level;
            std::filesystem::path source_file;
            std::vector<std::filesystem::path> include_search_path;
            std::vector<std::string> other_args;
            std::vector<std::string> definitions;
            std::filesystem::path base_dir;
            std::filesystem::path output_root;
        };
        struct link {
            std::string system;
            std::string project_name;
            std::filesystem::path output_root;
            std::vector<std::string> obj_files_path;
            std::vector<std::string> library_search_path;
            std::vector<std::string> static_link_libs;
            std::vector<std::string> library_path;
            std::vector<std::string> other_args;
        };
        struct static_lib {
            std::string project_name;
            std::filesystem::path output_root;
            std::vector<std::string> obj_files_path;
        };
        struct dynamic_lib {
            std::string system;
            std::string project_name;
            std::filesystem::path output_root;
            std::vector<std::string> obj_files_path;
        };
    }

    template <typename SinglePassRange, typename T>
    T accumulate(const SinglePassRange& rng, T init) {
        return std::accumulate(std::cbegin(rng), std::cend(rng), init);
    }
    template <typename SinglePassRange, typename T, typename BinaryOp>
    T accumulate(const SinglePassRange& rng, T init, BinaryOp binary_op) {
        return std::accumulate(std::cbegin(rng), std::cend(rng), init, binary_op);
    }

    std::optional<std::string>
    compile(const options::compile& opts, const bool verbose) {
        util::shell cmd(opts.system);
        cmd += opts.std_version;
        cmd += "-O" + opts.opt_level;
        cmd += "-c";
        cmd += std::filesystem::absolute(opts.source_file).string();
        cmd += accumulate(opts.include_search_path, util::shell(),
                [](util::shell& acc, auto& s){
                    return acc + ("-I" + std::filesystem::absolute(s).string());
                });
        cmd += accumulate(opts.other_args, util::shell());
        cmd += accumulate(opts.definitions, util::shell());

        cmd += "-o";
        const std::filesystem::path obj_path = std::filesystem::absolute(
                opts.output_root / std::filesystem::path(opts.source_file).filename().replace_extension("o"));
        std::filesystem::create_directories(obj_path.parent_path());
        cmd += obj_path.string();

        if (verbose) {
            std::cout << cmd << std::endl;
        }
        if (cmd.exec()) {
            return obj_path.string();
        } else {
            return std::nullopt;
        }
    }

    std::optional<std::string>
    link(const options::link& opts, const bool verbose) {
        util::shell cmd(opts.system);
        cmd += accumulate(opts.obj_files_path, util::shell());
        cmd += accumulate(opts.library_search_path, util::shell(),
                [](util::shell& acc, auto& s) { return acc + ("-L" + s); });
        cmd += accumulate(opts.static_link_libs, util::shell(),
                [](util::shell& acc, auto& s) { return acc + ("-l" + s); });
        cmd += accumulate(opts.library_path, util::shell());
        cmd += accumulate(opts.other_args, util::shell());

        const std::string bin_path =
                (opts.output_root / opts.project_name).string() + absorb::binary_extension;
        cmd += "-o " + bin_path;

        if (verbose) {
            std::cout << cmd << std::endl;
        }

        std::filesystem::create_directories(opts.output_root);
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
        cmd += accumulate(opts.obj_files_path, util::shell());

        if (verbose) {
            std::cout << cmd << std::endl;
        }

        std::filesystem::create_directories(opts.output_root);
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
        cmd += accumulate(opts.obj_files_path, util::shell());
        cmd += "-o";
        const std::string lib_name = "lib" + opts.project_name + absorb::dynamic_lib_extension;
        const std::string lib_path = (opts.output_root / lib_name).string();
        cmd += lib_path;

        if (verbose) {
            std::cout << cmd << std::endl;
        }

        std::filesystem::create_directories(opts.output_root);
        if (cmd.exec()) {
            return lib_path;
        } else {
            return std::nullopt;
        }
    }
} // end namespace
#endif // POAC_CORE_STROITE_COMPILER_HPP

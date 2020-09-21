#ifndef POAC_OPTS_POAC_NEW_HPP
#define POAC_OPTS_POAC_NEW_HPP

#include <filesystem>
#include <optional>
#include <poac/core/except.hpp>
#include <string>
#include <string_view>
#include <vector>
#include <poac/core/compiler.hpp>
#include <poac/util.hpp>
#include <fstream>
#include <streambuf>
#include <unordered_map>

namespace poac::opts {

enum class VersionControl {
    Git,
    NoVcs,
};

VersionControl
from_str(std::string_view s) {
    if (s == "git") {
        return VersionControl::Git;
    } else if (s == "none") {
        return VersionControl::NoVcs;
    } else {
        throw core::except::error("unknown vcs specification: `", s, "`");
    }
}

enum class NewProjectKind {
    Bin,
    Lib,
};

struct NewOptions {
    std::optional<VersionControl> version_control;
    NewProjectKind kind;
    /// Absolute path to the directory for the new package
    std::filesystem::path path;
    std::optional<std::string> name;
    std::optional<std::string> edition;
    std::optional<std::string> registry;
};

bool
is_bin(NewProjectKind kind) noexcept {
    return kind == NewProjectKind::Bin;
}

std::ostream&
operator<<(std::ostream& os, NewProjectKind kind) {
    switch (kind) {
        case NewProjectKind::Bin:
            return (os << "binary (application)");
        case NewProjectKind::Lib:
            return (os << "library");
        default:
            throw std::logic_error(
                "To access out of range of the "
                "enumeration values is undefined behavior.");
    }
}

struct SourceFileInformation {
    std::string relative_path;
    std::string target_name;
    bool bin;
};

struct MkOptions {
    std::optional<VersionControl> version_control;
    std::filesystem::path path;
    std::string name;
    std::vector<SourceFileInformation> source_files;
    bool bin;
    std::optional<std::string> edition;
    std::optional<std::string> registry;
};

struct CargoNewConfig {
    std::optional<std::string> name;
    std::optional<std::string> email;
    std::optional<VersionControl> version_control;
};

std::string
get_name(const std::filesystem::path& path, const NewOptions& opts) {
    if (opts.name.has_value()) {
        return opts.name.value();
    }

    const std::string file_name = path.filename();
    return file_name;
}

void
check_name(const std::string& name, const NewOptions& opts) {
    // If --name is already used to override, no point in suggesting it
    // again as a fix.
    const std::string name_help = opts.name.has_value() ? "" : "\nuse --name to override crate name";

    // Ban keywords + test list found at
    // https://en.cppreference.com/w/cpp/keyword
    std::vector<std::string_view> blacklist{
        "alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel", "atomic_commit", "atomic_noexcept",
        "auto", "bitand", "bitor", "bool", "break", "case", "catch", "char", "char8_t", "char16_t", "char32_t",
        "class", "compl", "concept", "const", "consteval", "constexpr", "const_cast", "continue", "co_await",
        "co_return", "co_yield", "decltype", "default", "delete", "do", "double", "dynamic_cast", "else", "enum",
        "explicit", "export", "extern", "false", "float", "for", "friend", "goto", "if", "inline", "int", "long",
        "mutable", "namespace", "new", "noexcept", "not", "not_eq", "nullptr", "operator", "or", "or_eq", "private",
        "protected", "public", "reflexpr", "register", "reinterpret_cast", "requires", "return", "short", "signed",
        "sizeof", "static", "static_assert", "static_cast", "struct", "switch", "synchronized", "template", "this",
        "thread_local", "throw", "true", "try", "typedef", "typeid", "typename", "union", "unsigned", "using",
        "virtual", "void", "volatile", "wchar_t", "while", "xor", "xor_eq",
    };
    if (std::find(blacklist.begin(), blacklist.end(), name) != blacklist.end()
     || (is_bin(opts.kind) && core::compiler::is_bad_artifact_name(name))) {
        throw core::except::error(
            "The name `", name, "` cannot be used as a package name", name_help
        );
    }

    if (std::isdigit(name[0])) {
        throw core::except::error(
            "Package names starting with a digit cannot be used as a package name", name_help
        );
    }

    util::validate_package_name(name, "package name", name_help);
}

void
detect_source_paths_and_types(
    const std::filesystem::path& package_path,
    const std::string& package_name,
    std::vector<SourceFileInformation>& detected_files
) {
    enum class H {
        Bin,
        Lib,
        Detect,
    };

    struct Test {
        std::string proposed_path;
        H handling;
    };

    const std::vector<Test> tests = {
        Test {
            .proposed_path = "src/main.cpp",
            .handling = H::Bin,
        },
        Test {
            .proposed_path = "main.cpp",
            .handling = H::Bin,
        },
        Test {
            .proposed_path = "src/" + package_name + ".cpp",
            .handling = H::Detect,
        },
        Test {
            .proposed_path = package_name + ".cpp",
            .handling = H::Detect,
        },
        Test {
            .proposed_path = "include/lib.hpp",
            .handling = H::Lib,
        },
        Test {
            .proposed_path = "lib.hpp",
            .handling = H::Lib,
        },
    };

    for (const auto& i : tests) {
        const std::string pp = i.proposed_path;

        // path/pp does not exist or is not a file
        if (!std::filesystem::exists(pp) || !std::filesystem::is_regular_file(pp)) {
            continue;
        }

        switch (i.handling) {
            case H::Bin:
                detected_files.push_back(SourceFileInformation {
                    .relative_path = pp,
                    .target_name = package_name,
                    .bin = true,
                });
                break;
            case H::Lib:
                detected_files.push_back(SourceFileInformation {
                    .relative_path = pp,
                    .target_name = package_name,
                    .bin = false,
                });
                break;
            case H::Detect:
                std::ifstream f(package_path / pp);
                const std::string content(
                        (std::istreambuf_iterator<char>(f)),
                        std::istreambuf_iterator<char>());
                const bool isbin = content.find("int main") != std::string::npos;
                detected_files.push_back(SourceFileInformation {
                    .relative_path = pp,
                    .target_name = package_name,
                    .bin = isbin,
                });
                break;
        }
    }

    // Check for duplicate lib attempt

    std::optional<std::string> previous_lib_relpath = std::nullopt;
    std::unordered_map<std::string, SourceFileInformation> duplicates_checker{};

    for (const auto& i : detected_files) {
        if (i.bin) {
            if (const auto x = duplicates_checker.find(i.target_name); x != duplicates_checker.end()) {
                throw core::except::error(
                        "multiple possible binary sources found:\n"
                        "  ", x->second.relative_path,
                        "  ", i.relative_path,
                        "cannot automatically generate poac.toml as the main target would be ambiguous"
                );
            }
            duplicates_checker.emplace(i.target_name, i);
        } else {
            if (previous_lib_relpath) {
                throw core::except::error(
                        "cannot have a package with "
                        "multiple libraries, "
                        "found both `", previous_lib_relpath.value(), "` and `",
                        i.relative_path, "`"
                );
            }
            previous_lib_relpath = i.relative_path;
        }
    }
}

SourceFileInformation
plan_new_source_file(bool bin, const std::string& package_name) {
    if (bin) {
        return SourceFileInformation {
            .relative_path = "src/main.cpp",
            .target_name =  package_name,
            .bin = true,
        };
    } else {
        return SourceFileInformation {
            .relative_path = "include/lib.hpp",
            .target_name = package_name,
            .bin = false,
        };
    }
}

void new_(const NewOptions& opts) {
    const std::filesystem::path path = opts.path;
    if (std::filesystem::exists(path)) {
        throw core::except::error(
            "destination `", path.string(), "` already exists\n\n"
               "Use `poac init` to initialize the directory"
        );
    }

    const std::string name = get_name(path, opts);
    check_name(name, opts);

    const auto mkopts = MkOptions {
        .version_control = opts.version_control,
        .path = path,
        .name = name,
        .source_files = {plan_new_source_file(is_bin(opts.kind), name)},
        .bin = is_bin(opts.kind),
        .edition = opts.edition,
        .registry = opts.registry,
    };
}

/// IgnoreList
struct IgnoreList {
    /// git like formatted entries
    std::vector<std::string> ignore;
    /// mercurial formatted entries
    std::vector<std::string> hg_ignore;

private:
    /// add a new entry to the ignore list. Requires two arguments with the
    /// entry in two different formats. One for "git style" entries and one for
    /// "mercurial like" entries.
    void
    push(const std::string& ignore_, const std::string& hg_ignore_) {
        ignore.push_back(ignore_);
        hg_ignore.push_back(hg_ignore_);
    }

    /// Return the correctly formatted content of the ignore file for the given
    /// version control system as `String`.
    
};

} // namespace poac::opts

#endif // !POAC_OPTS_POAC_NEW_HPP

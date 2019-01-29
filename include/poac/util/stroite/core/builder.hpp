#ifndef STROITE_CORE_BUILDER_HPP
#define STROITE_CORE_BUILDER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <functional>
#include <list>
#include <algorithm>
#include <map>
#include <optional>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "./compiler.hpp"
#include "./depends.hpp"
#include "../utils.hpp"

#include "../../../core/exception.hpp"
#include "../../../core/lock.hpp"
#include "../../../io/file/path.hpp"
#include "../../../core/naming.hpp"
#include "../../../io/cli.hpp"
#include "../../../io/file/yaml.hpp"


namespace stroite {
    struct builder {
        utils::options::compile compile_conf;
        utils::options::link link_conf;
        utils::options::static_lib static_lib_conf;
        utils::options::dynamic_lib dynamic_lib_conf;

        std::string system;
        std::string project_name;
        boost::filesystem::path base_dir;

        std::map<std::string, YAML::Node> node;
        std::map<std::string, std::map<std::string, std::string>> depends_ts;
        std::optional<std::map<std::string, YAML::Node>> deps_node;


        bool is_cpp_file(const boost::filesystem::path& p) {
            namespace fs = boost::filesystem;
            return !fs::is_directory(p)
                && (p.extension().string() == ".cpp"
                || p.extension().string() == ".cxx"
                || p.extension().string() == ".cc"
                || p.extension().string() == ".cp");
        }

        auto make_source_files() {
            namespace fs = boost::filesystem;
            namespace io = poac::io::file;

            std::vector<std::string> source_files;
            if (io::path::validate_dir(base_dir / "src")) {
                for (const fs::path& p : fs::recursive_directory_iterator(base_dir / "src")) {
                    if (is_cpp_file(p)) {
                        source_files.push_back(p.string());
                    }
                }
            }
            return source_files;
        }

        auto make_include_search_path() {
            namespace fs = boost::filesystem;
            namespace naming = poac::core::naming;
            namespace exception = poac::core::exception;
            namespace lock = poac::core::lock;
            namespace yaml = poac::io::file::yaml;
            namespace io = poac::io::file;

            std::vector<std::string> include_search_path;
            if (deps_node) { // subcmd/build.hppで，存在確認が取れている
                if (const auto locked_deps = lock::load_ignore_timestamp()) {
                    for (const auto& [name, dep] : (*locked_deps).backtracked) {
                        const std::string current_package_name = naming::to_current(dep.source, name, dep.version);
                        const fs::path package_path = io::path::current_deps_dir / current_package_name;

                        if (const fs::path include_dir = package_path / "include"; fs::exists(include_dir)) {// io::file::path::validate_dir??
                            include_search_path.push_back(include_dir.string());
                        }
                        else {
                            throw exception::error(
                                    name + " is not installed.\n"
                                           "Please build after running `poac install`");
                        }
                    }
                }
                else {
                    throw exception::error(
                            "Could not load poac.lock.\n"
                            "Please build after running `poac install`");
                }
            }
            return include_search_path;
        }

        auto make_macro_defns() {
            namespace fs = boost::filesystem;
            namespace configure = utils::configure;

            std::vector<std::string> macro_defns;
            // poac automatically define the absolute path of the project's root directory.
            macro_defns.push_back(configure::make_macro_defn("POAC_PROJECT_ROOT", fs::current_path().string()));
            macro_defns.push_back(configure::make_macro_defn("POAC_VERSION", node.at("version").as<std::string>()));
            return macro_defns;
        }

        auto make_compile_other_args() {
            namespace yaml = poac::io::file::yaml;
            if (const auto compile_args = yaml::get<std::vector<std::string>>(node.at("build"), "compile_args")) {
                return *compile_args;
            }
            else {
                return std::vector<std::string>{};
            }
        }


        std::string to_cache_hash_path(const std::string& s) {
            namespace fs = boost::filesystem;
            namespace io = poac::io::file;

            const auto hash_path = io::path::current_build_cache_hash_dir / fs::relative(s);
            return hash_path.string() + ".hash";
        }

        std::optional<std::map<std::string, std::string>>
        load_timestamps(const std::string& src_cpp_hash) {
            namespace io = poac::io::file;

            std::ifstream ifs(src_cpp_hash);
            if(!ifs.is_open()){
                return std::nullopt;
            }

            std::string buff;
            std::map<std::string, std::string> hash;
            while (std::getline(ifs, buff)) {
                const auto list_string = io::path::split(buff, ": \n");
                hash[list_string[0]] = list_string[1];
            }
            return hash;
        }

        void generate_timestamp(
                const std::string& filename,
                std::map<std::string, std::string>& timestamp)
        {
            namespace fs = boost::filesystem;
            namespace io = poac::io::file;

            boost::system::error_code error;
            const std::time_t last_time = fs::last_write_time(filename, error);
            timestamp.emplace(filename, std::to_string(last_time));
        }

        // *.cpp -> hash
        std::optional<std::map<std::string, std::string>>
        generate_timestamps(const std::string& source_file)
        {
            if (const auto deps_headers = core::depends::gen(compile_conf, source_file))
            {
                std::map<std::string, std::string> hash;
                for (const auto& name : *deps_headers) {
                    // Calculate the hash of the source dependent files.
                    generate_timestamp(name, hash);
                }
                // Calculate the hash of the source file itself.
                generate_timestamp(source_file, hash);
                return hash;
            }
            return std::nullopt;
        }

        auto check_src_cpp(const std::vector<std::string>& source_files)
        {
            namespace fs = boost::filesystem;

            std::vector<std::string> new_source_files;
            for (const auto& sf : source_files) {
                if (const auto previous_ts = load_timestamps(to_cache_hash_path(sf))) {
                    if (const auto current_ts = generate_timestamps(sf))
                    {
                        // Since hash of already existing hash file
                        //  does not match hash of current cpp file,
                        //  it does not exclude it from compilation,
                        //  and generates hash for overwriting.
                        if (*previous_ts != *current_ts) {
                            depends_ts[to_cache_hash_path(sf)] = *current_ts;
                            new_source_files.push_back(sf);
                        }
                    }
                }
                else {
                    // Since hash file does not exist, generates hash and compiles source file.
                    if (const auto cur_hash = generate_timestamps(sf))
                    {
                        depends_ts[to_cache_hash_path(sf)] = *cur_hash;
                        new_source_files.push_back(sf);
                    }
                }
            }
            return new_source_files;
        }

        auto hash_source_files(
            std::vector<std::string>&& source_files,
            const bool usemain )
        {
            namespace fs = boost::filesystem;
            namespace exception = poac::core::exception;

            if (usemain) {
                if (!fs::exists("main.cpp")) {
                    throw exception::error("main.cpp does not exists");
                }
                else {
                    source_files.push_back("main.cpp");
                }
            }
            return check_src_cpp(source_files);
        }

        void configure_compile(const bool usemain, const bool verbose)
        {
            compile_conf.system = system;
            compile_conf.version_prefix = utils::configure::default_version_prefix();
            // TODO: 存在することが確約されているときのyaml::get
            compile_conf.cpp_version = node.at("cpp_version").as<unsigned int>();
            compile_conf.include_search_path = make_include_search_path();
            compile_conf.other_args = make_compile_other_args();
            compile_conf.verbose = verbose;
            compile_conf.source_files = hash_source_files(make_source_files(), usemain);
            compile_conf.macro_defns = make_macro_defns();
            compile_conf.base_dir = base_dir;
            compile_conf.output_root = poac::io::file::path::current_build_cache_obj_dir;
        }
        std::optional<std::vector<std::string>>
        _compile() {
            namespace io = poac::io::file;

            if (const auto ret = core::compiler::compile(compile_conf)) {
                namespace fs = boost::filesystem;
                // Since compile succeeded, save hash
                std::ofstream ofs;
                for (const auto& [hash_name, data] : depends_ts) {
                    std::string output_string;
                    for (const auto& [fname, hash] : data) {
                        output_string += fname + ": " + hash + "\n";
                    }
                    fs::create_directories(fs::path(hash_name).parent_path());
                    io::path::write_to_file(ofs, hash_name, output_string);
                }
                // Because it is excluded for the convenience of cache,
                //  ignore the return value of compiler.compile.
                std::vector<std::string> obj_files;
                for (const auto& s : compile_conf.source_files) {
                    obj_files.push_back(
                            (compile_conf.output_root / fs::relative(s))
                                    .replace_extension("o")
                                    .string()
                    );
                }
                return obj_files;
            }
            else {
                return std::nullopt;
            }
        }

        auto make_link_other_args() {
            namespace yaml = poac::io::file::yaml;
            if (const auto link_args = yaml::get<std::vector<std::string>>(node.at("build"), "link_args")) {
                return *link_args;
            }
            else {
                return std::vector<std::string>{};
            }
        }
        // TODO: Divide it finer...
        auto make_link() {
            namespace fs = boost::filesystem;
            namespace yaml = poac::io::file::yaml;
            namespace naming = poac::core::naming;

            std::vector<std::string> library_search_path;
            std::vector<std::string> static_link_libs;
            std::vector<std::string> library_path;

            if (deps_node) {
                for (const auto& [name, next_node] : *deps_node) {
                    const auto[src, name2] = naming::get_source(name);
                    const std::string version = naming::get_version(next_node, src);

                    if (src != "poac") {
                        const std::string pkgname = naming::to_cache(src, name2, version);
                        const fs::path pkgpath = poac::io::file::path::current_deps_dir / pkgname;

                        // TODO: できればlockファイルに書かれたパッケージの./depsディレクトリのpoac.ymlを読むのが好ましい
                        if (const fs::path lib_dir = pkgpath / "lib"; fs::exists(lib_dir)) {
                            library_search_path.push_back(lib_dir.string());

                            if (const auto link = yaml::get<std::vector<std::string>>(next_node, "link", "include")) {
                                for (const auto &l : *link) {
                                    static_link_libs.push_back(l);
                                }
                            }
                            else {
                                static_link_libs.push_back(pkgname);
                            }
                        }
                    }

                    // TODO: 上がpoacがソースでないために，./deps/pkg/lib にlibが存在する
                    // TODO: 下がpoacがソースであるために，./deps/pkg/_build/lib に存在する
                    // TODO: しかし，library_search_path.push_back(lib_dir.string()); 以降の文では，
                    // TODO: poacがソースの場合，ユーザーが選択する必要は無いと判断する．(あとで直す？)

                    else {
                        const std::string pkgname = name2;
                        const fs::path pkgpath = poac::io::file::path::current_build_lib_dir / pkgname;

                        // TODO: dynamic libを指定できるように
                        if (const auto lib_dir = pkgpath.string() + ".a"; fs::exists(lib_dir)) {
                            library_path.push_back(lib_dir);
                        }
                    }
                }
            }
            return std::make_tuple(library_search_path, static_link_libs, library_path);
        }
        void configure_link(
            const std::vector<std::string>& obj_files_path,
            const bool verbose )
        {
            link_conf.system = system;
            link_conf.project_name = project_name;
            link_conf.output_root = poac::io::file::path::current_build_bin_dir;
            link_conf.obj_files_path = obj_files_path;
            const auto links = make_link();
            link_conf.library_search_path = std::get<0>(links);
            link_conf.static_link_libs = std::get<1>(links);
            link_conf.library_path = std::get<2>(links);
            link_conf.other_args = make_link_other_args();
            link_conf.verbose = verbose;
        }
        auto _link()
        {
            return core::compiler::link(link_conf);
        }

        void configure_static_lib(
                const std::vector<std::string>& obj_files_path,
                const bool verbose )
        {
            namespace io = poac::io::file;
            static_lib_conf.project_name = project_name;
            static_lib_conf.output_root = io::path::current_build_lib_dir;
            static_lib_conf.obj_files_path = obj_files_path;
            static_lib_conf.verbose = verbose;
        }
        auto _gen_static_lib()
        {
            return core::compiler::gen_static_lib(static_lib_conf);
        }

        void configure_dynamic_lib(
            const std::vector<std::string>& obj_files_path,
            const bool verbose )
        {
            namespace io = poac::io::file;
            dynamic_lib_conf.system = system;
            dynamic_lib_conf.project_name = project_name;
            // outputを一箇所か分散か選べるように．boost::hoghoeみたいに，enumのオプションを渡すとOK
            // 一箇所ってのは，./ poac build -> ./_buildだけど，depsも./_buildに配置されるやつ
            dynamic_lib_conf.output_root = io::path::current_build_lib_dir;
            dynamic_lib_conf.obj_files_path = obj_files_path;
            dynamic_lib_conf.verbose = verbose;
        }
        auto _gen_dynamic_lib()
        {
            return core::compiler::gen_dynamic_lib(dynamic_lib_conf);
        }

        // TODO: poac.ymlのhashもcheckしてほしい
        // TODO: 自らのinclude，dirも，(存在するなら！) includeパスに渡してほしい．そうすると，poacでinclude<poac/poac.hpp>できる
        explicit builder(const boost::filesystem::path& base_path = boost::filesystem::current_path())
        {
            namespace naming = poac::core::naming;
            namespace yaml = poac::io::file::yaml;

            const auto config_file = yaml::load_config_by_dir(base_path);
            node = yaml::get_by_width(config_file, "name", "version", "cpp_version", "build");
            deps_node = yaml::get<std::map<std::string, YAML::Node>>(config_file, "deps");
            project_name = naming::slash_to_hyphen(node.at("name").as<std::string>());
            system = utils::configure::auto_select_compiler();
            base_dir = base_path;
        }
    };
} // end namespace
#endif // STROITE_CORE_BUILDER_HPP

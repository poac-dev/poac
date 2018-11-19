#ifndef POAC_SUBCMD_UPDATE_HPP
#define POAC_SUBCMD_UPDATE_HPP

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <regex>

#include <boost/filesystem.hpp>

#include "../core/exception.hpp"
#include "../io/cli.hpp"
#include "../io/file.hpp"


// TODO: poac.ymlに書かれたversionが更新されていた時，パッケージをそのversionに更新する(poac.ymlも書き直す(書き換え時に，全部読み込んだら，コメント消える？？))
// TODO: その際，versionを指定せずとも最新版に更新するoptionが欲しい
// TODO: --selfを指定することで，poacのupdateを行う
// TODO: --select | --intractive とすると，インタラクティブに選択してupdateできる．
// TODO: --all で，全てのパッケージを
// TODO: -yおpションを付けないと確認メッセージが出る
// TODO: poac update hogeは，poac install hogeに対応してから．
// TODO: 現状は，poac updateで，poac.ymlからreadする

// TODO: 指定されたパッケージの依存先のみがupdateされていてもupdateする．
namespace poac::subcmd { struct update {
        static const std::string summary() { return "Update package"; }
        static const std::string options() { return "<Nothing>"; }

        template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void operator()(VS&& argv) { _main(std::move(argv)); }
        template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void _main(VS&& argv) {
            namespace fs     = boost::filesystem;
            namespace except = core::exception;

            check_arguments(argv);


        }

        void check_arguments(const std::vector<std::string>& argv) {
            namespace except = core::exception;

            if (argv.size() != 1)
                throw except::invalid_second_arg("login");
            std::regex pattern("\\w{8}-(\\w{4}-){3}\\w{12}");
            if (!std::regex_match(argv[0], pattern))
                throw except::invalid_second_arg("login");
        }
    };} // end namespace
#endif // !POAC_SUBCMD_UPDATE_HPP

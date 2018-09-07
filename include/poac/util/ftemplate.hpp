#ifndef POAC_UTIL_FTEMPLATE_HPP
#define POAC_UTIL_FTEMPLATE_HPP

#include <string>


namespace poac::util::ftemplate {
    // TODO: poac_ymlなどは，functionにして，project nameなどを入力可能にすべき．
    // TODO: なるべくcompile timeに計算すべき


    const std::string _gitignore(
        "# The directory poac downloads your dependencies sources to.\n"
        "deps\n"
        "_build"
    );
    const std::string main_cpp(
        "#include <iostream>\n"
        "\n"
        "\n"
        "int main() {\n"
        "    std::cout << \"Hello, world!\" << std::endl;\n"
        "}\n"
    );
    const std::string poac_yml(
        "name: sample\n"
        "version: 0.0.1\n"
        "owners:\n"
        "  - Your Name\n"
        "deps:\n"
        "  hello_world: 1.0.0\n"
    );
    const std::string README_md(
        "# PROJECT_NAME\n"
    );
} // end namespace
#endif // !POAC_UTIL_FTEMPLATE_HPP

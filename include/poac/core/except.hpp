#ifndef POAC_CORE_EXCEPT_HPP
#define POAC_CORE_EXCEPT_HPP

#include <string>
#include <string_view>
#include <variant>
#include <stdexcept>

namespace poac::core::except {
    template <typename Arg>
    std::string to_string(const Arg& str) {
        return std::to_string(str);
    }
    template <>
    std::string to_string(const std::string& str) {
        return str;
    }
    std::string to_string(std::string_view str) {
        return std::string(str);
    }
    template <typename CharT, std::size_t N>
    std::string to_string(const CharT(&str)[N]) {
        return str;
    }

    struct Error {
        struct General {
            const std::string impl;

            General() = delete;
            General(const General&) = default;
            General& operator=(const General&) = delete;

            General(General&&) = default;
            General& operator=(General&&) = delete;

            explicit General(const std::string& s) : impl(s) {}
            explicit General(const char* s) : impl(s) {}
            template <typename... Args>
            explicit General(const Args&... s)
                : General(
                      (... + except::to_string(s))
                  ) // delegation
            {}

            virtual ~General() noexcept = default;

            virtual std::string what() const {
                return impl;
            }
        };

        struct InvalidFirstArg {
            std::string what() const noexcept {
                return "Invalid argument";
            }
        };

        struct InvalidSecondArg {
            struct Build {
                std::string what() const noexcept {
                    return "build";
                }
            };
            struct Cache {
                std::string what() const noexcept {
                    return "cache";
                }
            };
            struct Cleanup {
                std::string what() const noexcept {
                    return "cleanup";
                }
            };
            struct Help {
                std::string what() const noexcept {
                    return "help";
                }
            };
            struct Init {
                std::string what() const noexcept {
                    return "init";
                }
            };
            struct New {
                std::string what() const noexcept {
                    return "new";
                }
            };
            struct Publish {
                std::string what() const noexcept {
                    return "publish";
                }
            };
            struct Search {
                std::string what() const noexcept {
                    return "search";
                }
            };
            struct Uninstall {
                std::string what() const noexcept {
                    return "uninstall";
                }
            };

            using state_type = std::variant<
                    Build,
                    Cache,
                    Cleanup,
                    Help,
                    Init,
                    New,
                    Publish,
                    Search,
                    Uninstall
            >;
            state_type state;
            template <typename T>
            InvalidSecondArg(T err) : state(err) {}

            std::string what() const {
                return std::visit([](auto&& arg) { return arg.what(); }, state);
            }
        };

        struct DoesNotExist : General {
            // Inheriting constructors
            using General::General;
            virtual ~DoesNotExist() noexcept = default;
            virtual std::string what() const {
                return "`" + General::what() + "` does not exist";
            }
        };
        struct KeyDoesNotExist final : DoesNotExist {
            // Inheriting constructors
            using DoesNotExist::DoesNotExist;
            ~KeyDoesNotExist() noexcept = default;
            std::string what() const {
                return "Required key " + DoesNotExist::what() + " in poac.yml";
            }
        };

        using state_type = std::variant<
                InvalidFirstArg,
                InvalidSecondArg,
                General,
                DoesNotExist,
                KeyDoesNotExist
        >;
        state_type state;
        template <typename T>
        Error(T err) : state(err) {}

        std::string what() const {
            return std::visit([](auto&& arg) { return arg.what(); }, state);
        }
    };

    namespace msg {
        std::string put_period(const std::string& str) {
            return str + ".";
        }

        std::string not_found(const std::string& str) {
            return put_period(str + " not found");
        }

        std::string does_not_exist(const std::string& str) {
            return put_period(str + " does not exist");
        }
        std::string key_does_not_exist(const std::string& str) {
            return put_period("Required key `" + str + "` does not exist in poac.yml");
        }

        std::string already_exist(const std::string& str) {
            return put_period(str + " already exist");
        }

        std::string could_not(const std::string& str) {
            return put_period("Could not " + str);
        }
        std::string could_not_load(const std::string& str) {
            return put_period(could_not("load " + str));
        }
        std::string could_not_read(const std::string& str) {
            return put_period(could_not("read " + str));
        }

        std::string please(const std::string& str) {
            return put_period("Please " + str);
        }
        std::string please_refer_docs(const std::string& str) {
            // str <- /en/getting_started.html
            return please("refer to https://doc.poac.pm" + str);
        }
        std::string please_exec(const std::string& str) {
            return please("Please execute " + str);
        }
    }

    class error : public std::invalid_argument
    {
    public:
        explicit error(const std::string& __s)
            : invalid_argument(__s) {}

        explicit error(const char* __s)
            : invalid_argument(__s) {}

        template <typename... Args>
        explicit error(const Args&... __s)
            : invalid_argument(
                    (... + except::to_string(__s))
              )
        {}

        virtual ~error() = default;
    };
} // end namespace
#endif // !POAC_CORE_EXCEPT_HPP

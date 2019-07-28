#ifndef POAC_CORE_EXCEPT_HPP
#define POAC_CORE_EXCEPT_HPP

#include <string>
#include <string_view>
#include <variant>
#include <stdexcept>

namespace poac::core::except {
    namespace detail {
        template<typename Arg>
        std::string to_string(const Arg& str) {
            return std::to_string(str);
        }

        template<>
        std::string to_string(const std::string& str) {
            return str;
        }

        std::string to_string(std::string_view str) {
            return std::string(str);
        }

        template<typename CharT, std::size_t N>
        std::string to_string(const CharT(&str)[N]) {
            return str;
        }
    }

    class Error {
    public:
        struct General {
        private:
            std::string impl;

        public:
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
                      (... + detail::to_string(s))
                  ) // delegation
            {}

            virtual ~General() noexcept = default;

            virtual std::string what() const {
                return impl;
            }
        };

        enum NoStates {
            InterruptedByUser,
            InvalidFirstArg
        };

        enum class InvalidSecondArg {
            Build,
            Cache,
            Cleanup,
            Help,
            Init,
            New,
            Publish,
            Search,
            Uninstall
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
                return "Required key " + DoesNotExist::what() + " in poac.toml";
            }
        };

    private:
        template <typename T>
        std::string what(const T& s) const {
            return s.what();
        }
        std::string
        what(NoStates err) const noexcept {
            switch (err) {
                case NoStates::InterruptedByUser:
                    return "Interrupted by user";
                case NoStates::InvalidFirstArg:
                    return "Invalid arguments";
            }
        }
        std::string
        what(InvalidSecondArg err) const noexcept {
            switch (err) {
                case InvalidSecondArg::Build:
                    return "build";
                case InvalidSecondArg::Cache:
                    return "cache";
                case InvalidSecondArg::Cleanup:
                    return "cleanup";
                case InvalidSecondArg::Help:
                    return "help";
                case InvalidSecondArg::Init:
                    return "init";
                case InvalidSecondArg::New:
                    return "new";
                case InvalidSecondArg::Publish:
                    return "publish";
                case InvalidSecondArg::Search:
                    return "search";
                case InvalidSecondArg::Uninstall:
                    return "uninstall";
            }
        }

    public:
        using state_type = std::variant<
                NoStates,
                InvalidSecondArg,
                General,
                DoesNotExist,
                KeyDoesNotExist
        >;
        state_type state;

        Error() = delete;
        Error(const Error&) = default;
        Error& operator=(const Error&) = delete;
        Error(Error&&) = default;
        Error& operator=(Error&&) = delete;
        ~Error() = default;

        template <typename T>
        Error(T err) : state(err) {}

        std::string what() const {
            return std::visit([this](auto&& err) { return what(err); }, state);
        }
    };

    namespace msg {
        std::string not_found(const std::string& str) {
            return str + " not found";
        }

        std::string does_not_exist(const std::string& str) {
            return str + " does not exist";
        }
        std::string key_does_not_exist(const std::string& str) {
            return "Required key `" + str + "` does not exist in poac.toml";
        }

        std::string already_exist(const std::string& str) {
            return str + " already exist";
        }

        std::string could_not(const std::string& str) {
            return "Could not " + str;
        }
        std::string could_not_load(const std::string& str) {
            return could_not("load " + str);
        }
        std::string could_not_read(const std::string& str) {
            return could_not("read " + str);
        }

        std::string please(const std::string& str) {
            return "Please " + str;
        }
        std::string please_refer_docs(const std::string& str) {
            // str <- /en/getting_started.html
            return please("refer to https://doc.poac.pm" + str);
        }
        std::string please_exec(const std::string& str) {
            return please("execute " + str);
        }
    }

    class error : public std::invalid_argument
    {
    public:
        explicit error(const std::string& __s) : invalid_argument(__s) {}
        explicit error(const char* __s) : invalid_argument(__s) {}
        template <typename... Args>
        explicit error(const Args&... __s)
            : invalid_argument(
                    (... + detail::to_string(__s))
              )
        {}

        virtual ~error() = default;
    };
} // end namespace
#endif // !POAC_CORE_EXCEPT_HPP

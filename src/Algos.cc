#include "Algos.hpp"

#include "Exception.hpp"
#include "Logger.hpp"
#include "Rustify.hpp"

#include <algorithm>
#include <cctype>
#include <memory>
#include <utility>

String
toUpper(const StringRef str) noexcept {
  String res;
  for (const unsigned char c : str) {
    res += static_cast<char>(std::toupper(c));
  }
  return res;
}

String
toMacroName(StringRef name) noexcept {
  String macroName;
  for (const unsigned char c : name) {
    if (std::isalpha(c)) {
      macroName += static_cast<char>(std::toupper(c));
    } else if (std::isdigit(c)) {
      macroName += static_cast<char>(c);
    } else {
      macroName += '_';
    }
  }
  return macroName;
}

int
execCmd(const StringRef cmd) noexcept {
  Logger::debug("Running `", cmd, '`');
  const int status = std::system(cmd.data());
  const int exitCode = status >> 8;
  return exitCode;
}

String
getCmdOutput(const StringRef cmd) {
  constexpr usize BUFFER_SIZE = 128;
  std::array<char, BUFFER_SIZE> buffer{};
  String result;

  Logger::debug("Running `", cmd, '`');
  FILE* pipe = popen(cmd.data(), "r");
  if (!pipe) {
    throw PoacError("popen() failed!");
  }

  while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
    result += buffer.data();
  }

  const int status = pclose(pipe);
  if (status == -1) {
    throw PoacError("pclose() failed!");
  }
  const int exitCode = status >> 8;
  if (exitCode != EXIT_SUCCESS) {
    std::cerr << result;
    throw PoacError("Command failed with exit code ", exitCode);
  }
  return result;
}

bool
commandExists(const StringRef cmd) noexcept {
  String checkCmd = "command -v ";
  checkCmd += cmd;
  checkCmd += " >/dev/null 2>&1";
  return execCmd(checkCmd) == EXIT_SUCCESS;
}

#ifdef POAC_TEST

#  include <limits>

namespace tests {

void
testLevDistance() {
  // Test bytelength agnosticity
  for (char c = 0; c < std::numeric_limits<char>::max(); ++c) {
    const String str(1, c);
    assertEq(levDistance(str, str), 0UL);
  }

  pass();
}

void
testLevDistance2() {
  constexpr StringRef STR1 = "\nMäry häd ä little lämb\n\nLittle lämb\n";
  constexpr StringRef STR2 = "\nMary häd ä little lämb\n\nLittle lämb\n";
  constexpr StringRef STR3 = "Mary häd ä little lämb\n\nLittle lämb\n";

  static_assert(levDistance(STR1, STR2) == 2UL);
  static_assert(levDistance(STR2, STR1) == 2UL);
  static_assert(levDistance(STR1, STR3) == 3UL);
  static_assert(levDistance(STR3, STR1) == 3UL);
  static_assert(levDistance(STR2, STR3) == 1UL);
  static_assert(levDistance(STR3, STR2) == 1UL);

  static_assert(levDistance("b", "bc") == 1UL);
  static_assert(levDistance("ab", "abc") == 1UL);
  static_assert(levDistance("aab", "aabc") == 1UL);
  static_assert(levDistance("aaab", "aaabc") == 1UL);

  static_assert(levDistance("a", "b") == 1UL);
  static_assert(levDistance("ab", "ac") == 1UL);
  static_assert(levDistance("aab", "aac") == 1UL);
  static_assert(levDistance("aaab", "aaac") == 1UL);

  pass();
}

// ref:
// https://github.com/llvm/llvm-project/commit/a247ba9d15635d96225ef39c8c150c08f492e70a#diff-fd993637669817b267190e7de029b75af5a0328d43d9b70c2e8dd512512091a2

void
testFindSimilarStr() {
  constexpr Arr<StringRef, 8> CANDIDATES{
    "if", "ifdef", "ifndef", "elif", "else", "endif", "elifdef", "elifndef"
  };

  static_assert(findSimilarStr("id", CANDIDATES) == "if"sv);
  static_assert(findSimilarStr("ifd", CANDIDATES) == "if"sv);
  static_assert(findSimilarStr("ifde", CANDIDATES) == "ifdef"sv);
  static_assert(findSimilarStr("elf", CANDIDATES) == "elif"sv);
  static_assert(findSimilarStr("elsif", CANDIDATES) == "elif"sv);
  static_assert(findSimilarStr("elseif", CANDIDATES) == "elif"sv);
  static_assert(findSimilarStr("elfidef", CANDIDATES) == "elifdef"sv);
  static_assert(findSimilarStr("elfindef", CANDIDATES) == "elifdef"sv);
  static_assert(findSimilarStr("elfinndef", CANDIDATES) == "elifndef"sv);
  static_assert(findSimilarStr("els", CANDIDATES) == "else"sv);
  static_assert(findSimilarStr("endi", CANDIDATES) == "endif"sv);

  static_assert(findSimilarStr("i", CANDIDATES) == None);
  static_assert(
      findSimilarStr("special_compiler_directive", CANDIDATES) == None
  );

  pass();
}

void
testFindSimilarStr2() {
  constexpr Arr<StringRef, 2> CANDIDATES{ "aaab", "aaabc" };
  static_assert(findSimilarStr("aaaa", CANDIDATES) == "aaab"sv);
  static_assert(findSimilarStr("1111111111", CANDIDATES) == None);

  constexpr Arr<StringRef, 1> CANDIDATES2{ "AAAA" };
  static_assert(findSimilarStr("aaaa", CANDIDATES2) == "AAAA"sv);

  pass();
}

} // namespace tests

int
main() {
  tests::testLevDistance();
  tests::testLevDistance2();
  tests::testFindSimilarStr();
  tests::testFindSimilarStr2();
}

#endif

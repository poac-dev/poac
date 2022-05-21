// ref: https://github.com/ninja-build/ninja/blob/master/src/status.h

#ifndef POAC_CORE_BUILDER_NINJA_STATUS_PRINTER_HPP
#define POAC_CORE_BUILDER_NINJA_STATUS_PRINTER_HPP

// std
#include <cstdio> // snprintf
#include <queue>  // std::queue

// external
#include <ninja/build.h>        // BuildConfig
#include <ninja/debug_flags.h>  // g_explaining
#include <ninja/graph.h>        // Edge
#include <ninja/line_printer.h> // LinePrinter
#include <ninja/status.h>       // StatusPrinter
#include <poac/poac.hpp>

namespace poac::core::builder::ninja {

struct StatusPrinter : Status {
  StatusPrinter(const BuildConfig& config, StringRef progress_status_format)
      : config_(config), started_edges_(0), finished_edges_(0), total_edges_(0),
        running_edges_(0), time_millis_(0),
        progress_status_format_(progress_status_format.data()),
        current_rate_(config.parallelism) {
    printer_.set_smart_terminal(false);
  }

  void
  PlanHasTotalEdges(int total) {
    total_edges_ = total;
  }

  void
  BuildEdgeStarted(const Edge* edge, i64 start_time_millis) {
    ++started_edges_;
    ++running_edges_;
    time_millis_ = start_time_millis;

    if (edge->use_console() || printer_.is_smart_terminal()) {
      PrintStatus(edge, start_time_millis);
    }
    if (edge->use_console()) {
      printer_.SetConsoleLocked(true);
    }
  }

  void
  BuildEdgeFinished(
      Edge* edge, i64 end_time_millis, bool success, const String& output
  ) {
    time_millis_ = end_time_millis;
    ++finished_edges_;

    if (edge->use_console()) {
      printer_.SetConsoleLocked(false);
    }
    if (config_.verbosity == BuildConfig::QUIET) {
      return;
    }
    if (!edge->use_console()) {
      PrintStatus(edge, end_time_millis);
    }

    --running_edges_;

    // Print the command that is spewing before printing its output.
    if (!success) {
      String outputs;
      for (auto o = edge->outputs_.begin(); o != edge->outputs_.end(); ++o) {
        outputs += (*o)->path() + " ";
      }

      if (printer_.supports_color()) {
        printer_.PrintOnNewLine(
            "\x1B[31m"
            "FAILED: "
            "\x1B[0m" +
            outputs + "\n"
        );
      } else {
        printer_.PrintOnNewLine("FAILED: " + outputs + "\n");
      }
      printer_.PrintOnNewLine(edge->EvaluateCommand() + "\n");
    }

    if (!output.empty()) {
      // ninja sets stdout and stderr of subprocesses to a pipe, to be able to
      // check if the output is empty. Some compilers, e.g. clang, check
      // isatty(stderr) to decide if they should print colored output.
      // To make it possible to use colored output with ninja, subprocesses
      // should be run with a flag that forces them to always print color
      // escape codes. To make sure these escape codes don't show up in a file
      // if ninja's output is piped to a file, ninja strips ansi escape codes
      // again if it's not writing to a |smart_terminal_|. (Launching
      // subprocesses in pseudo ttys doesn't work because there are only a few
      // hundred available on some systems, and ninja can launch thousands of
      // parallel compile commands.)
      String final_output;
      if (!printer_.supports_color()) {
        final_output = StripAnsiEscapeCodes(output);
      } else {
        final_output = output;
      }
#ifdef _WIN32
      // Fix extra CR being added on Windows, writing out CR CR LF (#773)
      _setmode(_fileno(stdout), _O_BINARY); // Begin Windows extra CR fix
#endif

      printer_.PrintOnNewLine(final_output);

#ifdef _WIN32
      _setmode(_fileno(stdout), _O_TEXT); // End Windows extra CR fix
#endif
    }
  }

  void
  BuildLoadDyndeps() {
    // The DependencyScan calls EXPLAIN() to print lines explaining why
    // it considers a portion of the graph to be out of date.  Normally
    // this is done before the build starts, but our caller is about to
    // load a dyndep file during the build.  Doing so may generate more
    // explanation lines (via fprintf directly to stderr), but in an
    // interactive console the cursor is currently at the end of a status
    // line.  Start a new line so that the first explanation does not
    // append to the status line.  After the explanations are done a
    // new build status line will appear.
    if (g_explaining) {
      printer_.PrintOnNewLine("");
    }
  }
  void
  BuildStarted() {
    started_edges_ = 0;
    finished_edges_ = 0;
    running_edges_ = 0;
  }
  void
  BuildFinished() {
    printer_.SetConsoleLocked(false);
    printer_.PrintOnNewLine("");
  }

  String
  FormatProgressStatus(const char* progress_status_format, int64_t time_millis)
      const {
    String out;
    char buf[32];
    for (const char* s = progress_status_format; *s != '\0'; ++s) {
      if (*s == '%') {
        ++s;
        switch (*s) {
          case '%':
            out.push_back('%');
            break;

            // Started edges.
          case 's':
            snprintf(buf, sizeof(buf), "%d", started_edges_);
            out += buf;
            break;

            // Total edges.
          case 't':
            snprintf(buf, sizeof(buf), "%d", total_edges_);
            out += buf;
            break;

            // Running edges.
          case 'r': {
            snprintf(buf, sizeof(buf), "%d", running_edges_);
            out += buf;
            break;
          }

            // Unstarted edges.
          case 'u':
            snprintf(buf, sizeof(buf), "%d", total_edges_ - started_edges_);
            out += buf;
            break;

            // Finished edges.
          case 'f':
            snprintf(buf, sizeof(buf), "%d", finished_edges_);
            out += buf;
            break;

            // Overall finished edges per second.
          case 'o':
            SnprintfRate(finished_edges_ / (time_millis / 1e3), buf, "%.1f");
            out += buf;
            break;

            // Current rate, average over the last '-j' jobs.
          case 'c':
            current_rate_.UpdateRate(finished_edges_, time_millis);
            SnprintfRate(current_rate_.rate(), buf, "%.1f");
            out += buf;
            break;

            // Percentage
          case 'p': {
            int percent = (100 * finished_edges_) / total_edges_;
            snprintf(buf, sizeof(buf), "%3i%%", percent);
            out += buf;
            break;
          }

          case 'e': {
            snprintf(buf, sizeof(buf), "%.3f", time_millis / 1e3);
            out += buf;
            break;
          }

          default:
            Fatal("unknown placeholder '%%%c' in $NINJA_STATUS", *s);
            return "";
        }
      } else {
        out.push_back(*s);
      }
    }

    return out;
  }

  void
  Warning(const char* msg, ...) {
    va_list ap;
    va_start(ap, msg);
    ::Warning(msg, ap);
    va_end(ap);
  }
  void
  Error(const char* msg, ...) {
    va_list ap;
    va_start(ap, msg);
    ::Error(msg, ap);
    va_end(ap);
  }
  void
  Info(const char* msg, ...) {
    va_list ap;
    va_start(ap, msg);
    ::Info(msg, ap);
    va_end(ap);
  }

private:
  void
  PrintStatus(const Edge* edge, i64 time_millis) {
    if (config_.verbosity == BuildConfig::QUIET ||
        config_.verbosity == BuildConfig::NO_STATUS_UPDATE) {
      return;
    }

    const bool force_full_command = config_.verbosity == BuildConfig::VERBOSE;
    String to_print = edge->GetBinding("description");
    if (to_print.empty() || force_full_command) {
      to_print = edge->GetBinding("command");
    }
    to_print =
        FormatProgressStatus(progress_status_format_, time_millis) + to_print;
    printer_.Print(
        to_print, force_full_command ? LinePrinter::FULL : LinePrinter::ELIDE
    );
  }

  const BuildConfig& config_;

  int started_edges_, finished_edges_, total_edges_, running_edges_;
  i64 time_millis_;

  /// Prints progress output.
  LinePrinter printer_;

  /// The custom progress status format to use.
  const char* progress_status_format_;

  template <usize S>
  void
  SnprintfRate(f64 rate, char (&buf)[S], const char* format) const {
    if (rate == -1) {
      std::snprintf(buf, S, "?");
    } else {
      std::snprintf(buf, S, format, rate);
    }
  }

  struct SlidingRateInfo {
    explicit SlidingRateInfo(int n) : rate_(-1), N(n), last_update_(-1) {}

    f64
    rate() {
      return rate_;
    }

    void
    UpdateRate(int update_hint, i64 time_millis_) {
      if (update_hint == last_update_) {
        return;
      }
      last_update_ = update_hint;

      if (times_.size() == N) {
        times_.pop();
      }
      times_.push(time_millis_);
      if (times_.back() != times_.front()) {
        rate_ = times_.size() / ((times_.back() - times_.front()) / 1e3);
      }
    }

  private:
    f64 rate_;
    const usize N;
    std::queue<f64> times_;
    int last_update_;
  };

  mutable SlidingRateInfo current_rate_;
};

} // namespace poac::core::builder::ninja

#endif // !POAC_CORE_BUILDER_NINJA_STATUS_PRINTER_HPP

// ref: https://github.com/ninja-build/ninja/blob/master/src/status.h

#ifndef POAC_CORE_BUILDER_NINJA_STATUS_PRINTER_HPP_
#define POAC_CORE_BUILDER_NINJA_STATUS_PRINTER_HPP_

// std
#include <cstdio> // snprintf
#include <queue>  // std::queue

// external
#include <ninja/build.h>        // BuildConfig // NOLINT(build/include_order)
#include <ninja/debug_flags.h>  // g_explaining // NOLINT(build/include_order)
#include <ninja/graph.h>        // Edge // NOLINT(build/include_order)
#include <ninja/line_printer.h> // LinePrinter // NOLINT(build/include_order)
#include <ninja/status.h>       // StatusPrinter // NOLINT(build/include_order)

// internal
#include "poac/poac.hpp"

namespace poac::core::builder::ninja {

struct StatusPrinter : Status {
  StatusPrinter(const BuildConfig& config, StringRef progress_status_format);

  void
  PlanHasTotalEdges(int total);

  void
  BuildEdgeStarted(const Edge* edge, i64 start_time_millis);

  void
  BuildEdgeFinished(
      Edge* edge, i64 end_time_millis, bool success, const String& output
  );

  void
  BuildLoadDyndeps();
  void
  BuildStarted();
  void
  BuildFinished();

  String
  FormatProgressStatus(const char* progress_status_format, int64_t time_millis)
      const;

  void
  Warning(const char* msg, ...);
  void
  Error(const char* msg, ...);
  void
  Info(const char* msg, ...);

private:
  void
  PrintStatus(const Edge* edge, i64 time_millis);

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

    inline f64
    rate() {
      return rate_;
    }

    void
    UpdateRate(int update_hint, i64 time_millis_);

  private:
    f64 rate_;
    const usize N;
    std::queue<f64> times_;
    int last_update_;
  };

  mutable SlidingRateInfo current_rate_;
};

} // namespace poac::core::builder::ninja

#endif // POAC_CORE_BUILDER_NINJA_STATUS_PRINTER_HPP_

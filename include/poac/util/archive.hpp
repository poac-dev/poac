#pragma once

// std
#include <memory>
#include <string>

// external
#include <archive.h> // NOLINT(build/include_order)
#include <archive_entry.h> // NOLINT(build/include_order)

// internal
#include "poac/util/format.hpp"
#include "poac/util/log.hpp"
#include "poac/util/result.hpp"
#include "poac/util/rustify.hpp"

namespace poac::util::archive {

using Archive = struct archive;

struct ArchiveWriteDelete {
  inline void operator()(Archive* w) {
    archive_write_close(w);
    archive_write_free(w);
  }
};
using Writer = std::unique_ptr<Archive, ArchiveWriteDelete>;

[[nodiscard]] Fn archive_write_data_block(
    const Writer& writer, const void* buffer, usize size, i64 offset
) noexcept -> Result<void, String>;

[[nodiscard]] Fn copy_data(Archive* reader, const Writer& writer) noexcept
    -> Result<void, String>;

[[nodiscard]] Fn archive_write_finish_entry(const Writer& writer) noexcept
    -> Result<void, String>;

[[nodiscard]] Fn archive_write_header(
    Archive* reader, const Writer& writer, archive_entry* entry
) noexcept -> Result<void, String>;

Fn set_extract_path(archive_entry* entry, const Path& extract_path)->String;

[[nodiscard]] Fn archive_read_next_header(
    Archive* reader, archive_entry** entry
) noexcept(!(ARCHIVE_EOF))
    ->Result<bool, String>;

[[nodiscard]] Fn
extract_impl(Archive* reader, const Writer& writer, const Path& extract_path)
    ->Result<String, String>;

[[nodiscard]] Fn archive_read_open_filename(
    Archive* reader, const Path& file_path, usize block_size
) noexcept -> Result<void, String>;

inline Fn make_flags() noexcept -> i32 {
  // Select which attributes we want to restore.
  // int
  return ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_UNLINK
         | ARCHIVE_EXTRACT_SECURE_NODOTDOT;
}

inline void read_as_targz(Archive* reader) noexcept {
  archive_read_support_format_tar(reader);
  archive_read_support_filter_gzip(reader);
}

[[nodiscard]] Fn extract(const Path& target_file_path, const Path& extract_path)
    ->Result<String, String>;

} // namespace poac::util::archive

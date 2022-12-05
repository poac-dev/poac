#ifndef POAC_UTIL_ARCHIVE_HPP_
#define POAC_UTIL_ARCHIVE_HPP_

// std
#include <memory>
#include <string>

// external
#include <archive.h> // NOLINT(build/include_order)
#include <archive_entry.h> // NOLINT(build/include_order)

// internal
#include "poac/poac.hpp"

namespace poac::util::archive {

using Archive = struct archive;

struct ArchiveWriteDelete {
  inline void operator()(Archive* w) {
    archive_write_close(w);
    archive_write_free(w);
  }
};
using Writer = std::unique_ptr<Archive, ArchiveWriteDelete>;

[[nodiscard]] Result<void, String> archive_write_data_block(
    const Writer& writer, const void* buffer, usize size, i64 offset
) noexcept;

[[nodiscard]] Result<void, String>
copy_data(Archive* reader, const Writer& writer) noexcept;

[[nodiscard]] Result<void, String>
archive_write_finish_entry(const Writer& writer) noexcept;

[[nodiscard]] Result<void, String> archive_write_header(
    Archive* reader, const Writer& writer, archive_entry* entry
) noexcept;

String set_extract_path(archive_entry* entry, const Path& extract_path);

[[nodiscard]] Result<bool, String> archive_read_next_header_(
    Archive* reader, archive_entry** entry
) noexcept(!(true == ARCHIVE_EOF));

[[nodiscard]] Result<String, String>
extract_impl(Archive* reader, const Writer& writer, const Path& extract_path);

[[nodiscard]] Result<void, String> archive_read_open_filename(
    Archive* reader, const Path& file_path, usize block_size
) noexcept;

inline i32 make_flags() noexcept {
  // Select which attributes we want to restore.
  // int
  return ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_UNLINK
         | ARCHIVE_EXTRACT_SECURE_NODOTDOT;
}

inline void read_as_targz(Archive* reader) noexcept {
  archive_read_support_format_tar(reader);
  archive_read_support_filter_gzip(reader);
}

[[nodiscard]] Result<String, String>
extract(const Path& target_file_path, const Path& extract_path);

} // namespace poac::util::archive

#endif // POAC_UTIL_ARCHIVE_HPP_

module;

// std
#include <memory>
#include <string>

// external
#include <archive.h> // NOLINT(build/include_order)
#include <archive_entry.h> // NOLINT(build/include_order)
#include <boost/scope_exit.hpp>
#include <string>

// internal
#include "result-macros.hpp"

export module poac.util.archive;

import poac.util.format;
import poac.util.log;
import poac.util.result;
import poac.util.rustify;

namespace poac::util::archive {

using Archive = struct archive;

struct ArchiveWriteDelete {
  inline void operator()(Archive* w) {
    archive_write_close(w);
    archive_write_free(w);
  }
};
using Writer = std::unique_ptr<Archive, ArchiveWriteDelete>;

[[nodiscard]] auto
archive_write_data_block(
    const Writer& writer, const void* buffer, usize size, i64 offset
) noexcept -> Result<void, std::string> {
  const la_ssize_t res =
      archive_write_data_block(writer.get(), buffer, size, offset);
  if (res < ARCHIVE_OK) {
    return Err(archive_error_string(writer.get()));
  }
  return Ok();
}

[[nodiscard]] auto
copy_data(Archive* reader, const Writer& writer) noexcept
    -> Result<void, std::string> {
  usize size{};
  const void* buff = nullptr;
  i64 offset{};

  while (true) {
    const i32 res = archive_read_data_block(reader, &buff, &size, &offset);
    if (res == ARCHIVE_EOF) {
      return Ok();
    } else if (res < ARCHIVE_OK) {
      return Err(archive_error_string(reader));
    }
    Try(archive_write_data_block(writer, buff, size, offset));
  }
}

[[nodiscard]] auto
archive_write_finish_entry(const Writer& writer
) noexcept -> Result<void, std::string> {
  const i32 res = archive_write_finish_entry(writer.get());
  if (res < ARCHIVE_OK) {
    return Err(archive_error_string(writer.get()));
  } else if (res < ARCHIVE_WARN) {
    return Err("Encountered error while finishing entry.");
  }
  return Ok();
}

[[nodiscard]] auto
archive_write_header(
    Archive* reader, const Writer& writer, archive_entry* entry
) noexcept -> Result<void, std::string> {
  if (archive_write_header(writer.get(), entry) < ARCHIVE_OK) {
    return Err(archive_error_string(writer.get()));
  } else if (archive_entry_size(entry) > 0) {
    Try(copy_data(reader, writer));
  }
  return Ok();
}

auto
set_extract_path(archive_entry* entry, const fs::path& extract_path)
    -> std::string {
  std::string current_file = archive_entry_pathname(entry);
  const fs::path full_output_path = extract_path / current_file;
  log::debug("extracting to `{}`", full_output_path.string());
  archive_entry_set_pathname(entry, full_output_path.c_str());
  return current_file;
}

[[nodiscard]] auto
archive_read_next_header(Archive* reader, archive_entry** entry) noexcept(
    !static_cast<bool>(ARCHIVE_EOF) // NOLINT(modernize-use-bool-literals)
) -> Result<bool, std::string> {
  const i32 res = ::archive_read_next_header(reader, entry);
  if (res == ARCHIVE_EOF) {
    return Ok(ARCHIVE_EOF);
  } else if (res < ARCHIVE_OK) {
    return Err(archive_error_string(reader));
  } else if (res < ARCHIVE_WARN) {
    return Err("Encountered error while reading header.");
  }
  return Ok(false);
}

[[nodiscard]] auto
extract_impl(
    Archive* reader, const Writer& writer, const fs::path& extract_path
) -> Result<std::string, std::string> {
  archive_entry* entry = nullptr;
  std::string extracted_directory_name;
  while (Try(archive::archive_read_next_header(reader, &entry)) != ARCHIVE_EOF
  ) {
    if (extracted_directory_name.empty()) {
      extracted_directory_name = set_extract_path(entry, extract_path);
    } else {
      set_extract_path(entry, extract_path);
    }
    Try(archive_write_header(reader, writer, entry));
    Try(archive_write_finish_entry(writer));
  }
  return Ok(extracted_directory_name);
}

[[nodiscard]] auto
archive_read_open_filename(
    Archive* reader, const fs::path& file_path, usize block_size
) noexcept -> Result<void, std::string> {
  if (archive_read_open_filename(reader, file_path.c_str(), block_size)) {
    return Err("Cannot archive_read_open_filename");
  }
  return Ok();
}

inline auto
make_flags() noexcept -> i32 {
  // Select which attributes we want to restore.
  // int
  return ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_UNLINK
         | ARCHIVE_EXTRACT_SECURE_NODOTDOT;
}

inline void
read_as_targz(Archive* reader) noexcept {
  archive_read_support_format_tar(reader);
  archive_read_support_filter_gzip(reader);
}

export [[nodiscard]] auto
extract(const fs::path& target_file_path, const fs::path& extract_path)
    -> Result<std::string, std::string> {
  Archive* reader = archive_read_new();
  if (!reader) {
    return Err("Cannot archive_read_new");
  }
  BOOST_SCOPE_EXIT_ALL(&reader) {
    archive_read_free(reader);
  };
  read_as_targz(reader);

  const Writer writer(archive_write_disk_new());
  if (!writer) {
    return Err("Cannot archive_write_disk_new");
  }

  archive_write_disk_set_options(writer.get(), make_flags());
  archive_write_disk_set_standard_lookup(writer.get());

  Try(archive_read_open_filename(reader, target_file_path, 10'240));
  BOOST_SCOPE_EXIT_ALL(&reader) {
    archive_read_close(reader);
  };

  return extract_impl(reader, writer, extract_path);
}

} // namespace poac::util::archive

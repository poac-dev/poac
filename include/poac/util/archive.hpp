#ifndef POAC_UTIL_ARCHIVE_HPP
#define POAC_UTIL_ARCHIVE_HPP

// std
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <memory>
#include <vector>

// external
#include <archive.h>
#include <archive_entry.h>
#include <boost/scope_exit.hpp>
#include <fmt/core.h>
#include <mitama/result/result.hpp>
#include <plog/Log.h>

namespace poac::util::archive {
    using archive_t = struct archive;

    struct archive_write_delete {
        void operator()(archive_t* w) {
            archive_write_close(w);
            archive_write_free(w);
        }
    };
    using writer_t = std::unique_ptr<archive_t, archive_write_delete>;

    [[nodiscard]] mitama::result<void, std::string>
    archive_write_data_block(
        const writer_t& writer, const void* buffer,
        std::size_t size, std::int64_t offset
    ) noexcept {
        int res = archive_write_data_block(writer.get(), buffer, size, offset);
        if (res < ARCHIVE_OK) {
            return mitama::failure(archive_error_string(writer.get()));
        }
        return mitama::success();
    }

    [[nodiscard]] mitama::result<void, std::string>
    copy_data(archive_t* reader, const writer_t& writer) noexcept {
        std::size_t size{};
        const void* buff = nullptr;
        std::int64_t offset{};

        while (true) {
            int res = archive_read_data_block(reader, &buff, &size, &offset);
            if (res == ARCHIVE_EOF) {
                return mitama::success();
            } else if (res < ARCHIVE_OK) {
                return mitama::failure(archive_error_string(reader));
            }
            MITAMA_TRY(archive_write_data_block(writer, buff, size, offset));
        }
    }

    [[nodiscard]] mitama::result<void, std::string>
    archive_write_finish_entry(const writer_t& writer) noexcept {
        int res = archive_write_finish_entry(writer.get());
        if (res < ARCHIVE_OK) {
            return mitama::failure(archive_error_string(writer.get()));
        } else if (res < ARCHIVE_WARN) {
            return mitama::failure("Encountered error while finishing entry.");
        }
        return mitama::success();
    }

    [[nodiscard]] mitama::result<void, std::string>
    archive_write_header(archive_t* reader, const writer_t& writer, archive_entry* entry) noexcept {
        if (archive_write_header(writer.get(), entry) < ARCHIVE_OK) {
            return mitama::failure(archive_error_string(writer.get()));
        } else if (archive_entry_size(entry) > 0) {
            MITAMA_TRY(copy_data(reader, writer));
        }
        return mitama::success();
    }

    std::string
    set_extract_path(archive_entry* entry, const std::filesystem::path& extract_path) noexcept {
        const std::string current_file = archive_entry_pathname(entry);
        const std::filesystem::path full_output_path = extract_path / current_file;
        PLOG_DEBUG << fmt::format("extracting to `{}`", full_output_path);
        archive_entry_set_pathname(entry, full_output_path.c_str());
        return current_file;
    }

    [[nodiscard]] mitama::result<bool, std::string>
    archive_read_next_header_(archive_t* reader, archive_entry** entry)
        noexcept(!(true == ARCHIVE_EOF))
    {
        int res = archive_read_next_header(reader, entry);
        if (res == ARCHIVE_EOF) {
            return mitama::success(ARCHIVE_EOF);
        } else if (res < ARCHIVE_OK) {
            return mitama::failure(archive_error_string(reader));
        } else if (res < ARCHIVE_WARN) {
            return mitama::failure("Encountered error while reading header.");
        }
        return mitama::success(false);
    }

    [[nodiscard]] mitama::result<std::string, std::string>
    extract_impl(archive_t* reader, const writer_t& writer, const std::filesystem::path& extract_path) noexcept {
        archive_entry* entry = nullptr;
        std::string extracted_directory_name{""};
        while (MITAMA_TRY(archive_read_next_header_(reader, &entry)) != ARCHIVE_EOF) {
            if (extracted_directory_name.empty()) {
                extracted_directory_name = set_extract_path(entry, extract_path);
            } else {
                set_extract_path(entry, extract_path);
            }
            MITAMA_TRY(archive_write_header(reader, writer, entry));
            MITAMA_TRY(archive_write_finish_entry(writer));
        }
        return mitama::success(extracted_directory_name);
    }

    [[nodiscard]] mitama::result<void, std::string>
    archive_read_open_filename(
        archive_t* reader,
        const std::filesystem::path& file_path,
        std::size_t block_size) noexcept
    {
        if (archive_read_open_filename(reader, file_path.c_str(), block_size)) {
            return mitama::failure("Cannot archive_read_open_filename");
        }
        return mitama::success();
    }

    int make_flags() noexcept
    {
        // Select which attributes we want to restore.
        // int
        return ARCHIVE_EXTRACT_TIME
             | ARCHIVE_EXTRACT_UNLINK
             | ARCHIVE_EXTRACT_SECURE_NODOTDOT;
    }

    void read_as_targz(archive_t* reader) noexcept {
        archive_read_support_format_tar(reader);
        archive_read_support_filter_gzip(reader);
    }

    [[nodiscard]] mitama::result<std::string, std::string>
    extract(
        const std::filesystem::path& target_file_path,
        const std::filesystem::path& extract_path
    ) noexcept
    {
        archive_t* reader = archive_read_new();
        if (!reader) {
            return mitama::failure("Cannot archive_read_new");
        }
        BOOST_SCOPE_EXIT_ALL(&reader) {
            archive_read_free(reader);
        };
        read_as_targz(reader);

        writer_t writer(archive_write_disk_new());
        if (!writer) {
            return mitama::failure("Cannot archive_write_disk_new");
        }

        archive_write_disk_set_options(writer.get(), make_flags());
        archive_write_disk_set_standard_lookup(writer.get());

        MITAMA_TRY(archive_read_open_filename(reader, target_file_path, 10'240));
        BOOST_SCOPE_EXIT_ALL(&reader) {
            archive_read_close(reader);
        };

        return extract_impl(reader, writer, extract_path);
    }
} // end namespace

#endif // !POAC_UTIL_ARCHIVE_HPP

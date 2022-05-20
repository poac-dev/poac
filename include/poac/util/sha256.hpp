#ifndef POAC_UTIL_SHA256_HPP
#define POAC_UTIL_SHA256_HPP

// std
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

// external
#include <fmt/core.h>
#include <mitama/result/result.hpp>
#include <mitama/anyhow/anyhow.hpp>
#include <mitama/thiserror/thiserror.hpp>
#include <openssl/evp.h>
#include <openssl/sha.h> // SHA256_DIGEST_LENGTH

namespace poac::util::sha256 {
    namespace anyhow = mitama::anyhow;
    namespace thiserror = mitama::thiserror;

    class Error {
        template <thiserror::fixed_string S, class ...T>
        using error = thiserror::error<S, T...>;

    public:
        using FailedToReadFile =
            error<"Failed to read file: {0}", std::filesystem::path>;

        using FailedToCreateSha256Digest =
            error<"Failed to create a sha256 digest">;
    };

    inline constexpr int buf_size = 32768;
    inline constexpr int hash_size = 65;

    // ref: https://stackoverflow.com/a/2458382
    std::string
    hash_string(const std::vector<unsigned char>& hash) {
        std::string output;
        for (const unsigned char h : hash) {
            // ref: https://stackoverflow.com/a/64311447
            output += fmt::format("{:0>2x}", h); // zero paddings in 2 length
        }
        return output;
    }

    // ref: https://stackoverflow.com/a/34289358
    [[nodiscard]] anyhow::result<std::string>
    sum(const std::filesystem::path& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            return anyhow::failure<Error::FailedToReadFile>(path);
        }

        std::vector<char> buffer(buf_size);
        std::vector<unsigned char> hash(SHA256_DIGEST_LENGTH);
        EVP_MD_CTX* ctx = EVP_MD_CTX_create();
        if (ctx == NULL) {
            return anyhow::failure<Error::FailedToCreateSha256Digest>();
        }
        if (1 != EVP_DigestInit_ex(ctx, EVP_sha256(), NULL)) {
            return anyhow::failure<Error::FailedToCreateSha256Digest>();
        }

        int bytesRead;
        while ((bytesRead = file.read(buffer.data(), buf_size).gcount())) {
            if (1 != EVP_DigestUpdate(ctx, buffer.data(), bytesRead)) {
                return anyhow::failure<Error::FailedToCreateSha256Digest>();
            }
        }

        // ref: https://github.com/openssl/openssl/blob/22dbb176deef7d9a80f5c94f57a4b518ea935f50/crypto/evp/digest.c#L691
        // Let me know if this is wrong.
        unsigned int temp = 0;
        if (1 != EVP_DigestFinal_ex(ctx, hash.data(), &temp)) {
            return anyhow::failure<Error::FailedToCreateSha256Digest>();
        }
        EVP_MD_CTX_destroy(ctx);

        return mitama::success(hash_string(hash));
    }
} // end namespace

#endif // !POAC_UTIL_SHA256_HPP

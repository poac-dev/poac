#ifndef POAC_UTIL_SHA256_HPP
#define POAC_UTIL_SHA256_HPP

// std
#include <fstream>

// external
#include <openssl/evp.h>
#include <openssl/sha.h> // SHA256_DIGEST_LENGTH

// internal
#include <poac/poac.hpp>

namespace poac::util::sha256 {
    class Error {
        template <thiserror::fixed_string S, class ...T>
        using error = thiserror::error<S, T...>;

    public:
        using FailedToReadFile =
            error<"Failed to read file: `{}`", String>;

        using FailedToCreateSha256Digest =
            error<"Failed to create a sha256 digest">;
    };

    inline constexpr int buf_size = 32768;
    inline constexpr int hash_size = 65;

    // ref: https://stackoverflow.com/a/2458382
    String
    hash_string(const Vec<unsigned char>& hash) {
        String output;
        for (const unsigned char h : hash) {
            // ref: https://stackoverflow.com/a/64311447
            output += format("{:0>2x}", h); // zero paddings in 2 length
        }
        return output;
    }

    // ref: https://stackoverflow.com/a/34289358
    [[nodiscard]] Result<String>
    sum(const fs::path& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            return Err<Error::FailedToReadFile>(path.string());
        }

        Vec<char> buffer(buf_size);
        Vec<unsigned char> hash(SHA256_DIGEST_LENGTH);
        EVP_MD_CTX* ctx = EVP_MD_CTX_create();
        if (ctx == NULL) {
            return Err<Error::FailedToCreateSha256Digest>();
        }
        if (1 != EVP_DigestInit_ex(ctx, EVP_sha256(), NULL)) {
            return Err<Error::FailedToCreateSha256Digest>();
        }

        int bytesRead;
        while ((bytesRead = file.read(buffer.data(), buf_size).gcount())) {
            if (1 != EVP_DigestUpdate(ctx, buffer.data(), bytesRead)) {
                return Err<Error::FailedToCreateSha256Digest>();
            }
        }

        // ref: https://github.com/openssl/openssl/blob/22dbb176deef7d9a80f5c94f57a4b518ea935f50/crypto/evp/digest.c#L691
        // Let me know if this is wrong.
        u32 temp = 0;
        if (1 != EVP_DigestFinal_ex(ctx, hash.data(), &temp)) {
            return Err<Error::FailedToCreateSha256Digest>();
        }
        EVP_MD_CTX_destroy(ctx);

        return Ok(hash_string(hash));
    }
} // end namespace

#endif // !POAC_UTIL_SHA256_HPP

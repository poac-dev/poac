module;

// std
#include <string>

// external
#include <openssl/evp.h> // NOLINT(build/include_order)
#include <openssl/sha.h> // SHA256_DIGEST_LENGTH // NOLINT(build/include_order)

export module poac.util.sha256;

// internal
import poac.util.format;
import poac.util.log;
import poac.util.result;
import poac.util.rustify;

namespace poac::util::sha256 {

using FailedToReadFile = Error<"Failed to read file: `{}`", std::string>;
using FailedToCreateSha256Digest = Error<"Failed to create a sha256 digest">;

inline constexpr i32 BUF_SIZE = 32768;
inline constexpr i32 HASH_SIZE = 65;

// ref: https://stackoverflow.com/a/2458382
auto
hash_string(const std::vector<unsigned char>& hash) -> std::string {
  std::string output;
  for (const unsigned char h : hash) {
    // ref: https://stackoverflow.com/a/64311447
    output += format("{:0>2x}", h); // zero paddings in 2 length
  }
  return output;
}

// ref: https://stackoverflow.com/a/34289358
export [[nodiscard]] auto
sum(const fs::path& path) -> Result<std::string> {
  std::ifstream file(path, std::ios::binary);
  if (!file) {
    return Err<FailedToReadFile>(path.string());
  }

  std::vector<char> buffer(BUF_SIZE);
  std::vector<unsigned char> hash(SHA256_DIGEST_LENGTH);
  EVP_MD_CTX* ctx = EVP_MD_CTX_create();
  if (ctx == nullptr) {
    return Err<FailedToCreateSha256Digest>();
  }
  if (1 != EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr)) {
    return Err<FailedToCreateSha256Digest>();
  }

  int bytes_read;
  while ((bytes_read = file.read(buffer.data(), BUF_SIZE).gcount())) {
    if (1 != EVP_DigestUpdate(ctx, buffer.data(), bytes_read)) {
      return Err<FailedToCreateSha256Digest>();
    }
  }

  // ref:
  // https://github.com/openssl/openssl/blob/22dbb176deef7d9a80f5c94f57a4b518ea935f50/crypto/evp/digest.c#L691
  // Let me know if this is wrong.
  u32 temp = 0;
  if (1 != EVP_DigestFinal_ex(ctx, hash.data(), &temp)) {
    return Err<FailedToCreateSha256Digest>();
  }
  EVP_MD_CTX_destroy(ctx);

  return Ok(hash_string(hash));
}

} // namespace poac::util::sha256

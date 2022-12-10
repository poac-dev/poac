// std
#include <fstream>

// internal
#include "poac/util/sha256.hpp"

namespace poac::util::sha256 {

// ref: https://stackoverflow.com/a/2458382
Fn hash_string(const Vec<unsigned char>& hash)->String {
  String output;
  for (const unsigned char h : hash) {
    // ref: https://stackoverflow.com/a/64311447
    output += format("{:0>2x}", h); // zero paddings in 2 length
  }
  return output;
}

// ref: https://stackoverflow.com/a/34289358
[[nodiscard]] Fn sum(const Path& path)->Result<String> {
  std::ifstream file(path, std::ios::binary);
  if (!file) {
    return Err<FailedToReadFile>(path.string());
  }

  Vec<char> buffer(buf_size);
  Vec<unsigned char> hash(SHA256_DIGEST_LENGTH);
  EVP_MD_CTX* ctx = EVP_MD_CTX_create();
  if (ctx == nullptr) {
    return Err<FailedToCreateSha256Digest>();
  }
  if (1 != EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr)) {
    return Err<FailedToCreateSha256Digest>();
  }

  int bytesRead;
  while ((bytesRead = file.read(buffer.data(), buf_size).gcount())) {
    if (1 != EVP_DigestUpdate(ctx, buffer.data(), bytesRead)) {
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

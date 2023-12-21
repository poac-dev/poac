#include "Manifest.hpp"

#include <memory>
#include <toml.hpp>

class Manifest {
public:
  static Manifest& instance() noexcept {
    static Manifest instance;
    instance.load();
    return instance;
  }

  void load() {
    if (data) {
      return;
    }
    data = std::make_unique<toml::value>(toml::parse("poac.toml"));
  }

  std::unique_ptr<toml::value> data = nullptr;

private:
  Manifest() noexcept = default;

  // Delete copy constructor and assignment operator to prevent copying
  Manifest(const Manifest&) = delete;
  Manifest& operator=(const Manifest&) = delete;
};

String getPackageName() {
  Manifest& manifest = Manifest::instance();
  return toml::find<String>(*manifest.data, "package", "name");
}

String getCppEdition() {
  Manifest& manifest = Manifest::instance();
  String edition = toml::find<String>(*manifest.data, "package", "edition");
  if (edition.size() != 2) {
    throw std::runtime_error("invalid edition: " + edition);
  }
  return edition;
}

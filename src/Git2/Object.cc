#include "Object.hpp"

#include "Oid.hpp"

#include <git2/object.h>

namespace git2 {

Object::~Object() {
  git_object_free(mRaw);
}

Object::Object(git_object* obj) : mRaw(obj) {}

Oid
Object::id() const {
  return Oid(git_object_id(mRaw));
}

} // namespace git2

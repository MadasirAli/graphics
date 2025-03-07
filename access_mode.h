#pragma once

namespace base {
  namespace graphics {
    enum class access_mode {
      undef,
      immutable,
      none,
      write_discard,
      read,
      write,
      read_write
    };
  }
}
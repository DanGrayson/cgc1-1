#pragma once
#include "gc_allocator.hpp"
namespace cgc1
{
  namespace details
  {
    CGC1_OPT_INLINE bool is_marked(const object_state_t *os)
    {
      return os->user_flags() & 1;
    }
    CGC1_OPT_INLINE bool is_atomic(const object_state_t *os)
    {
      return 0 != (os->user_flags() & 2);
    }
    CGC1_OPT_INLINE bool is_complex(const object_state_t *os)
    {
      return 0 != (os->user_flags() & 4);
    }
    CGC1_ALWAYS_INLINE void clear_mark(object_state_t *os)
    {
      os->set_user_flags(os->user_flags() & 6);
    }
    CGC1_OPT_INLINE void set_mark(object_state_t *os, bool status)
    {
      os->set_user_flags((os->user_flags() & 6) | static_cast<size_t>(status));
    }
    CGC1_OPT_INLINE void set_atomic(object_state_t *os, bool status)
    {
      os->set_user_flags((os->user_flags() & 5) | (static_cast<size_t>(status) << 1));
    }
    CGC1_OPT_INLINE void set_complex(object_state_t *os, bool status)
    {
      os->set_user_flags((os->user_flags() & 3) | (static_cast<size_t>(status) << 2));
    }
  }
}

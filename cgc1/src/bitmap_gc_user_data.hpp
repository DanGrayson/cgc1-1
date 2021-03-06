#pragma once
#include "gc_user_data.hpp"
#include "global_kernel_state.hpp"
#include <cgc1/cgc1.hpp>
namespace cgc1
{
  namespace details
  {
    /**
     * \brief Type code for raw allocations.
     **/
    static const constexpr size_t cs_bitmap_allocation_type_raw{0};
    /**
     * \brief Type code for atomic allocations.
     **/
    static const constexpr size_t cs_bitmap_allocation_type_atomic{1};
    /**
     * \brief Type code for allocations with user data.
     **/
    static const constexpr size_t cs_bitmap_allocation_type_user_data{2};
    /**
     * \brief User bit index for enabling finalization.
     **/
    static const constexpr size_t cs_bitmap_allocation_user_bit_finalizeable{0};
    /**
     * \brief User bit index for enabling finalization on arbitrary threads.
     **/
    static const constexpr size_t cs_bitmap_allocation_user_bit_finalizeable_arbitrary_thread{1};
    /**
     * \brief User data that can be associated with an allocation.
     **/
    class alignas(::mcppalloc::details::user_data_alignment_t) bitmap_gc_user_data_t
    {
    public:
      auto gc_user_data_ref() noexcept -> gc_user_data_t &
      {
        return m_gc_user_data;
      }
      auto gc_user_data_ref() const noexcept -> const gc_user_data_t &
      {
        return m_gc_user_data;
      }

      auto atomic() const noexcept -> bool
      {
        return m_atomic;
      }
      void set_atomic(bool atomic) noexcept
      {
        m_atomic = atomic;
      }
      auto abort_on_collect() const noexcept -> bool
      {
        return m_abort_on_collect;
      }
      void set_abort_on_collect(bool abort_on_collect) noexcept
      {
        m_abort_on_collect = abort_on_collect;
      }

    private:
      gc_user_data_t m_gc_user_data;
      bool m_atomic{false};
      bool m_abort_on_collect{false};
    };

    inline details::bitmap_gc_user_data_t *bitmap_allocator_user_data(void *addr)
    {
      if (!is_bitmap_allocator(addr)) {
        return nullptr;
      }
      const auto state = ::mcppalloc::bitmap_allocator::details::get_state(addr);
      if (mcpputil_unlikely(!state->has_valid_magic_numbers())) {
        assert(false);
        return nullptr;
      }
      if (state->type_id() != 2) {
        return nullptr;
      }
      const auto bin_sz = state->real_entry_size();
      uint8_t *const start_of_gc_user_data =
          ::mcpputil::unsafe_cast<uint8_t>(addr) + bin_sz - sizeof(details::bitmap_gc_user_data_t);
      return ::mcpputil::unsafe_cast<details::bitmap_gc_user_data_t>(start_of_gc_user_data);
    }
  }
}

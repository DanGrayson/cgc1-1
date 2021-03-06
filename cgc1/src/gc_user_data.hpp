#pragma once
#include <functional>
#include <mcppalloc/user_data_base.hpp>
namespace cgc1
{
  namespace details
  {
    /**
     * \brief User data that can be associated with an allocation.
     **/
    class alignas(::mcppalloc::details::user_data_alignment_t) gc_user_data_t : public ::mcppalloc::details::user_data_base_t
    {
    public:
      /**
       * \brief Type of finalizer.
       **/
      using finalizer_type = ::std::function<void(void *)>;
      /**
       * \brief Constructor
       *
       **/
      gc_user_data_t() noexcept;
      gc_user_data_t(const gc_user_data_t &);
      gc_user_data_t(gc_user_data_t &&);
      gc_user_data_t &operator=(const gc_user_data_t &);
      gc_user_data_t &operator=(gc_user_data_t &&);

      auto is_uncollectable() const noexcept -> bool
      {
        return m_uncollectable;
      }
      void set_uncollectable(bool val) noexcept
      {
        m_uncollectable = val;
      }
      /**
       * \brief Return if finalizer is allowed to run in arbitrary thread.
       **/
      auto allow_arbitrary_finalizer_thread() const noexcept -> bool
      {
        return m_allow_arbitrary_finalizer_thread;
      }
      /**
       * \brief Set if finalizer is allowed to run in arbitrary thread.
       **/
      void set_allow_arbitrary_finalizer_thread(bool allow) noexcept
      {
        m_allow_arbitrary_finalizer_thread = allow;
      }
      /**
       * \brief Return reference to finalizer.
       **/
      auto finalizer_ref() noexcept -> finalizer_type &;
      /**
       * \brief Return reference to finalizer.
       **/
      auto finalizer_ref() const noexcept -> const finalizer_type &;
      /**
       * \brief Optional finalizer function to run.
       **/
      finalizer_type m_finalizer{nullptr};

    private:
      /**
       * \brief True if uncollectable, false otherwise.
       *
       **/
      bool m_uncollectable{false};
      /**
       * \brief Allow arbitrary finalization thread.
       *
       * True if an arbittrary finalization thread can be used, false otherwise.
       **/
      bool m_allow_arbitrary_finalizer_thread{false};
    };
    inline gc_user_data_t::gc_user_data_t() noexcept = default;
    inline gc_user_data_t::gc_user_data_t(const gc_user_data_t &) = default;
    inline gc_user_data_t::gc_user_data_t(gc_user_data_t &&) = default;
    inline gc_user_data_t &gc_user_data_t::operator=(const gc_user_data_t &) = default;
    inline gc_user_data_t &gc_user_data_t::operator=(gc_user_data_t &&) = default;
  }
}
#include "gc_user_data_impl.hpp"

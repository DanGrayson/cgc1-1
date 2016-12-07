#pragma once
#include <gsl/gsl>
namespace cgc1
{
  /**
   * \brief Default policy for root collections.
   **/
  template <typename Lock, typename Allocator_Type>
  class default_root_collection_policy_t
  {
  public:
    using lock_type = Lock;
    using allocator_type = Allocator_Type;
  };
  /**
   * \brief Class that stores root pointers for a garbage collector.
   **/
  template <typename Policy>
  class root_collection_t
  {
  public:
    using policy_type = Policy;
    using lock_type = typename policy_type::lock_type;
    using allocator_type = typename policy_type::allocator_type;
    root_collection_t(lock_type &lock, allocator_type allocator);
    root_collection_t(const root_collection_t &) = default;
    root_collection_t(root_collection_t &&) noexcept = default;
    root_collection_t &operator=(const root_collection_t &) = default;
    root_collection_t &operator=(root_collection_t &&) noexcept = default;
    /**
     * \brief Add root to root collection.
     *
     * Does not permit duplicates.
     **/
    void add_root(void **r) NO_THREAD_SAFETY_ANALYSIS;
    /**
     * \brief Remove single root from root collection if present.
     **/
    void remove_root(void **r) NO_THREAD_SAFETY_ANALYSIS;
    /**
     * \brief Return if single root is in root collection.
     **/
    bool has_root(void **r) NO_THREAD_SAFETY_ANALYSIS;
    void clear() NO_THREAD_SAFETY_ANALYSIS;
    /**
     * \brief Return single roots.
     **/
    ::gsl::span<void **> roots();
    /**
     * \brief Return single roots.
     **/
    ::gsl::span<const void **> roots() const;

    void add_memory_range(mcpputil::system_memory_range_t range);
    void remove_memory_range(mcpputil::system_memory_range_t range);
    void remove_memory_range(ptrdiff_t location);
    bool has_memory_range(mcpputil::system_memory_range_t range);

  private:
    lock_type &m_lock;
    /**
     * \brief Vector of pointers to roots.
     **/
    ::mcpputil::rebind_vector_t<void **, allocator_type> m_roots;
    /**
     * \brief Allocator to be used.
     **/
    allocator_type m_allocator;
  };
  template <typename Policy>
  root_collection_t<Policy>::root_collection_t(lock_type &lock, allocator_type allocator)
      : m_lock(lock), m_roots(allocator), m_allocator(::std::move(allocator))
  {
  }
  template <typename Policy>
  void root_collection_t<Policy>::add_root(void **r)
  {
    MCPPALLOC_CONCURRENCY_LOCK_GUARD(m_lock);
    // make sure not already a root.
    const auto it = find(m_roots.begin(), m_roots.end(), r);
    if (it == m_roots.end())
      m_roots.push_back(r);
  }
  template <typename Policy>
  void root_collection_t<Policy>::remove_root(void **r)
  {
    MCPPALLOC_CONCURRENCY_LOCK_GUARD(m_lock);
    // find root and erase.
    auto it = find(m_roots.begin(), m_roots.end(), r);
    if (it != m_roots.end())
      m_roots.erase(it);
  }
  template <typename Policy>
  bool root_collection_t<Policy>::has_root(void **r)
  {
    MCPPALLOC_CONCURRENCY_LOCK_GUARD(m_lock);
    // find root and erase.
    auto it = find(m_roots.begin(), m_roots.end(), r);
    if (it != m_roots.end())
      return true;
    return false;
  }
  template <typename Policy>
  void root_collection_t<Policy>::clear()
  {
    MCPPALLOC_CONCURRENCY_LOCK_GUARD(m_lock);
    m_roots.clear();
  }
  template <typename Policy>
  ::gsl::span<void **> root_collection_t<Policy>::roots()
  {
    return m_roots;
  }
  template <typename Policy>
  ::gsl::span<const void **> root_collection_t<Policy>::roots() const
  {
    return m_roots;
  }
}

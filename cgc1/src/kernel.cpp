#include "internal_declarations.hpp"
#include "global_kernel_state.hpp"
#include "thread_local_kernel_state.hpp"
#include <cgc1/cgc1.hpp>
namespace cgc1
{
  namespace details
  {
    global_kernel_state_t g_gks;
    void thread_gc_handler(int)
    {
      g_gks._collect_current_thread();
    }
#ifdef __APPLE__
    pthread_key_t g_pkey;
#else
#ifndef _WIN32
    thread_local thread_local_kernel_state_t *t_tlks;
#else
    __declspec(thread) thread_local_kernel_state_t *t_tlks;
#endif
#endif
  }
  namespace debug
  {
    size_t num_gc_collections()
    {
      return details::g_gks.num_collections();
    }
  }
  bool in_signal_handler()
  {
    auto tlks = details::get_tlks();
    if (tlks)
      return tlks->in_signal_handler();
    return false;
  }

  void *cgc_malloc(size_t sz)
  {
    auto &ta = details::g_gks.gc_allocator().initialize_thread();
    return ta.allocate(sz);
  }
  void *cgc_realloc(void * v, size_t sz)
  {
    void* ret = cgc_malloc(sz);
    memcpy(ret, v, sz);
    return ret;
  }
  void cgc_free(void *v)
  {
    auto &ta = details::g_gks.gc_allocator().initialize_thread();
    ta.destroy(v);
  }
  bool cgc_is_cgc(void * v)
  {
    return cgc_size(v) > 0;
  }
  void* cgc_start(void* addr)
  {
    if (!addr)
      return nullptr;
    details::object_state_t *os = details::object_state_t::from_object_start(addr);
    if (!details::g_gks.is_valid_object_state(os))
    {
      os = details::g_gks.find_valid_object_state(addr);
      if (!os)
        return nullptr;
    }
    return os->object_start();
  }
  size_t cgc_size(void * addr)
  {
    details::object_state_t* os = details::object_state_t::from_object_start(cgc_start(addr));
    if (os)
      return os->object_size();
    else
      return 0;
  }
  void cgc_add_root(void **v)
  {
    details::g_gks.add_root(v);
  }
  void cgc_remove_root(void **v)
  {
    details::g_gks.remove_root(v);
  }
  size_t cgc_heap_size()
  {
    return details::g_gks.gc_allocator().end() - details::g_gks.gc_allocator().begin();
  }
  size_t cgc_heap_free()
  {
    return details::g_gks.gc_allocator().end() - details::g_gks.gc_allocator().current_end();
  }
  void cgc_enable()
  {
    details::g_gks.enable();
  }
  void cgc_disable()
  {
    details::g_gks.disable();
  }
  bool cgc_is_enabled()
  {
    return details::g_gks.enabled();
  }
  void cgc_register_thread(void *top_of_stack)
  {
    details::g_gks.initialize_current_thread(top_of_stack);
  }
  void cgc_collect()
  {
    details::g_gks.collect();
  }
  void cgc_force_collect()
  {
    details::g_gks.force_collect();
  }
  void cgc_unregister_thread()
  {
    details::g_gks.destroy_current_thread();
  }
  void cgc_shutdown()
  {
    details::g_gks.shutdown();
  }
  void cgc_register_finalizer(void* addr, ::std::function<void(void*)> finalizer)
  {
    if (!addr)
      return;
    details::object_state_t* os = details::object_state_t::from_object_start(cgc_start(addr));
    if (!os)
      return;
    details::gc_user_data_t* ud = static_cast<details::gc_user_data_t*>(os->user_data());
    if (ud->m_is_default)
    {
      ud = make_unique_allocator<details::gc_user_data_t, cgc_internal_allocator_t<void>>(*ud).release();
      ud->m_is_default = false;
      os->set_user_data(ud);
    }
    ud->m_finalizer = finalizer;
  }
  void cgc_set_uncollectable(void* addr, bool is_uncollectable)
  {
    if (!addr)
      return;
    void* start = cgc_start(addr);
    if (!start)
      return;
    details::object_state_t* os = details::object_state_t::from_object_start(start);
    if (!os)
      return;
    details::gc_user_data_t* ud = static_cast<details::gc_user_data_t*>(os->user_data());
    if (ud->m_is_default)
    {
      ud = make_unique_allocator<details::gc_user_data_t, cgc_internal_allocator_t<void>>(*ud).release();
      ud->m_is_default = false;
      os->set_user_data(ud);
    }
    ud->m_uncollectable = is_uncollectable;
    set_complex(os, true);
  }
}

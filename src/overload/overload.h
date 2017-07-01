#include <type_traits>
#include <iostream>
#include <utility>
#include <string>
#include <functional>

namespace sftb
{
  namespace detail
  {
    template <typename... Args> class overload_class;

    template <typename Arg>
    class overload_class<Arg> : Arg
    {
      using self = overload_class<Arg>;
    public:
      using Arg::operator ();
      template <class ActualArg, std::enable_if_t<!std::is_same<std::decay_t<ActualArg>, self>::value, int> = 0>
      overload_class(const ActualArg &arg) : Arg(arg) {}
    };

    template <typename Arg0, typename... Args>
    class overload_class<Arg0, Args...> : Arg0, overload_class<Args...>
    {
      using self = overload_class<Arg0, Args...>;
      using parent_t = overload_class<Args...>;

    public:
      using Arg0::operator();
      using parent_t::operator ();

      template <class ActualArg0, class... ActualArgs, std::enable_if_t<!std::is_same<std::decay_t<ActualArg0>, self>::value, int> = 0>
      overload_class(const ActualArg0 &arg0, const ActualArgs &... args) : Arg0(arg0), parent_t(args...) {}
    };
  }  // namespace detail

    template <typename T>
    struct wrapper_for
    {
      using type = T;
    };

    template <typename T> struct func_ptr_wrapper;
    template <typename Ret, typename... Args>
    struct func_ptr_wrapper<Ret(Args...)>
    {
      using func_ptr_t = Ret(*)(Args...);
      func_ptr_t m_ptr;
      func_ptr_wrapper(func_ptr_t ptr) : m_ptr(ptr) {}
      template <typename... ActualArgs>
      auto operator()(ActualArgs&&... args) -> decltype (this->m_ptr(std::forward<ActualArgs>(args)...)) { return this->m_ptr(std::forward<ActualArgs>(args)...); }
    };

    template <typename Ret, typename ...Args>
    struct wrapper_for<Ret(*) (Args...)>
    {
      using type = func_ptr_wrapper<Ret(Args...)>;
    };

    template <typename T>
    using wrapper_for_t = typename wrapper_for<T>::type;
  }

  template <typename... Args>
  auto overload(Args&&... args) -> detail::overload_class<detail::wrapper_for_t<std::decay_t<Args>>...> { return { std::forward<Args>(args)... }; }
} // namespace misc

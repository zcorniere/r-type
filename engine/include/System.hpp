#ifndef BABEL_SYSTEM_H_
#define BABEL_SYSTEM_H_

#include "Game.hpp"
#include "function_traits.hpp"
#include <concepts>
#include <functional>

template <typename Fn>
concept SystemFunction =
    std::is_same<typename function_traits<Fn>::result_type, void>::value
        &&function_traits<Fn>::arity > 0;
// concept SystemFunction = boost::function_traits<Fn>::result_type;

class System
{
  public:
    System(SystemFunction auto system);

    void call(Game &) const;

  private:
    std::function<void(Game &)> m_call_wrapper;
};

#include "System.tpp"

#endif // BABEL_SYSTEM_H_

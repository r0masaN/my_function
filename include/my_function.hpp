#ifndef MY_FUNCTION_HPP
#define MY_FUNCTION_HPP

template<typename>
class my_function;

template<typename R, typename... Args>
class my_function<R(Args...)> {
    void *obj{nullptr};
    void *(*copier)(void *){nullptr};
    void (*deleter)(void *){nullptr};
    R (*invoker)(void *, Args&&...){nullptr};

    template<typename D>
    static void *copy(void *obj);

    template<typename D>
    static void destruct(void *obj) noexcept(std::is_nothrow_destructible_v<D>);

    template<typename D>
    static R invoke(void *obj, Args&&... args);

public:
    my_function() = default;

    template<typename L, typename D = std::decay_t<L>, typename = std::enable_if_t<std::is_invocable_r_v<R, D&, Args...>>>
    my_function(L&& l) noexcept(noexcept(D(std::forward<L>(l))));

    my_function(const my_function& other) noexcept(noexcept((*other.copier)(obj)));

    my_function(my_function&& other) noexcept;

    ~my_function() noexcept(noexcept(deleter(obj)));

    my_function& operator=(const my_function& other) noexcept(noexcept((*deleter)(obj)) && noexcept(noexcept((*copier)(obj))));

    my_function& operator=(my_function&& other) noexcept;

    R operator()(Args&&... args) const;
};

#include "my_function.tcc"

#endif

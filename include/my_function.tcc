#ifndef MY_FUNCTION_TCC
#define MY_FUNCTION_TCC

template<typename R, typename... Args>
template<typename D>
void *my_function<R(Args...)>::copy(void *const obj) noexcept(noexcept(std::is_nothrow_copy_constructible_v<D>)) {
    if (obj) return new D(const_cast<const D&>(*static_cast<D *>(obj)));
    return nullptr;
}

template<typename R, typename... Args>
template<typename D>
void my_function<R(Args...)>::destruct(void *const obj) noexcept(noexcept(std::is_nothrow_destructible_v<D>)) {
    if (obj) delete static_cast<D *>(obj);
}

template<typename R, typename... Args>
template<typename D>
R my_function<R(Args...)>::invoke(void *const obj, Args&&... args) {
    if (obj) return (*static_cast<D *>(obj))(std::forward<Args>(args)...);
    throw std::invalid_argument("can't invoke an empty my_function");
}

template<typename R, typename... Args>
template<typename L, typename D, typename>
my_function<R(Args...)>::my_function(L&& l) noexcept(noexcept(D(std::forward<L>(l)))) : obj(new D(std::forward<L>(l))),
    copier(copy<D>),
    deleter(destruct<D>),
    invoker(invoke<D>) {
}

template<typename R, typename... Args>
my_function<R(Args...)>::my_function(const my_function& other) noexcept(noexcept((*other.copier)(obj))) :
    obj(other.copier(other.obj)),
    copier(other.copier),
    deleter(other.deleter),
    invoker(other.invoker) {
}

template<typename R, typename... Args>
my_function<R(Args...)>::my_function(my_function&& other) noexcept :
    obj(other.obj),
    copier(other.copier),
    deleter(other.deleter),
    invoker(other.invoker) {
    other.obj = nullptr;
}

template<typename R, typename... Args>
my_function<R(Args...)>::~my_function() noexcept(noexcept(deleter(obj))) {
    if (obj && deleter) (*deleter)(obj);
}

template<typename R, typename... Args>
my_function<R(Args...)>& my_function<R(Args...)>::operator=(const my_function& other)
    noexcept(noexcept((*deleter)(obj)) && noexcept(noexcept((*copier)(obj)))) {
    if (this != &other) {
        my_function temp{other};
        std::swap(*this, temp);
    }

    return *this;
}

template<typename R, typename... Args>
my_function<R(Args...)>& my_function<R(Args...)>::operator=(my_function&& other) noexcept {
    if (this != &other) {
        my_function temp{std::move(other)};
        std::swap(*this, temp);
    }

    return *this;
}

template<typename R, typename... Args>
R my_function<R(Args...)>::operator()(Args&&... args) const {
    if (invoker && obj) return (*invoker)(obj, std::forward<Args>(args)...);
    throw std::invalid_argument("can't invoke an empty my_function");
}

#endif

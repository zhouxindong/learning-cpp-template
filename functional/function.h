#pragma once

#include <iostream>
#include <type_traits>
#include <utility>

namespace xdbd {

template <typename R, typename... Args> class FunctionBase {
public:
  virtual ~FunctionBase() {}

public:
  virtual FunctionBase *clone() const = 0;
  virtual R call(Args... args) const = 0;
};

template <typename Callable, typename R, typename... Args>
class FunctionStorage : public FunctionBase<R, Args...> {
  Callable callable_;

public:
  template <typename U>
  FunctionStorage(U &&f) : callable_(std::forward<U>(f)) {}

  FunctionBase<R, Args...> *clone() const override {
    return new FunctionStorage(callable_);
  }

  R call(Args... args) const override { return callable_(args...); }
};

template <typename Signature> class Function;

template <typename R, typename... Args> class Function<R(Args...)> {
  FunctionBase<R, Args...> *holder_;

  friend void swap(Function &a, Function &b) {
    std::swap(a.holder_, b.holder_);
  }

public:
  Function() noexcept : holder_(nullptr) {}

  Function(Function const &rhs)
      : holder_(rhs.holder_ == nullptr ? nullptr : rhs.holder_->clone()) {}

  Function(Function &rhs) noexcept
      : Function(static_cast<Function const &>(rhs)) {}

  Function(Function &&rhs) noexcept : holder_(rhs.holder_) {
    rhs.holder_ = nullptr;
  }

  template <typename Callable> Function(Callable &&c) : holder_(nullptr) {
    using Functor = std::decay_t<Callable>;
    using Holder = FunctionStorage<Functor, R, Args...>;
    holder_ = new Holder(std::forward<Callable>(c));
  }

  ~Function() { delete holder_; }

public:
  Function &operator=(Function const &rhs) {
    Function tmp(rhs);
    swap(*this, tmp);

    return *this;
  }

  Function &operator=(Function &rhs) {
    this->operator=(static_cast<Function const &>(rhs));
    return *this;
  }

  Function &operator=(Function &&rhs) noexcept {
    delete holder_;
    holder_ = rhs.holder_;
    rhs.holder_ = nullptr;
    return *this;
  }

  template <typename Callable> Function &operator=(Callable &&c) {
    Function tmp(std::forward<Callable>(c));
    swap(*this, tmp);
    return *this;
  }

public:
  explicit operator bool() const { return holder_ != nullptr; }

  R operator()(Args... args) const { return holder_->call(args...); }
};

} // namespace xdbd

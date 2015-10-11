#pragma once

#include <utility>
#include <assert.h>

namespace optional_namespace {
template <bool val>
struct clear_t;

template <>
struct clear_t<false> {
  template <typename T>
  void clear(T &){};
};

template <>
struct clear_t<true> {
  template <typename T>
  void clear(T &val) {
    val = T();
  };
};
}

template <typename T>
class aligned_storage_wrapper {
  static_assert(!std::is_reference<T>::value, "");

 public:
  aligned_storage_wrapper() {
    optional_namespace::clear_t<std::is_pod<T>::value>().clear(get());
  }

  T const &get() const { return reinterpret_cast<T const &>(m_value); }
  T &get() { return reinterpret_cast<T &>(m_value); }

 private:
  typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type
      m_value;
};

// Helper structure, needed for overload selection.
struct in_place_t {};
// Helper structure, represents the lack of value.
struct nullopt_t {};

const in_place_t in_place; // TODO: add constexpr here
const nullopt_t nullopt;   // TODO: add constexpr here

/// The class template optional manages an optional contained value, i.e. a
/// value that semantically may not be present.
///
/// A common use case for optional is the return value of a function that may
/// fail. As opposed to other approaches, such as std::pair<T,bool>, optional
/// handles expensive to construct objects well and is more readable, as the
/// intent is expressed explicitly.
///
/// The value is guaranteed to be allocated within the optional object itself,
/// i.e. no dynamic memory allocation ever takes place. Thus, an optional object
/// models an object, not a pointer, even though the operator * () and
/// operator -> () are defined.

/// The value inside an optional object may be in either an initialized or
/// uninitialized state. An optional object with a value in initialized state is
/// called engaged, whereas if the value is in uninitialized state, the object
/// is called disengaged.

/// The optional object is engaged on the following conditions:
/// * The object is initialized with a value of type T
/// * The object is assigned an engaged optional.

/// The object is disengaged on the following conditions:
/// * The object is default-initialized.
/// * The object is initialized with a value of nullopt_t or a disengaged
///   optional object.
/// * The object is assigned a value of nullopt_t or a disengaged optional.
///
template <typename T>
class optional {
  static_assert(
      !std::is_same<typename std::remove_cv<T>::type, nullopt_t>::value &&
          !std::is_same<typename std::remove_cv<T>::type, in_place_t>::value &&
          !std::is_reference<T>::value,
      "Invalid instantiation of optional<T>");

 public:
  /// Constructs the object without initializing the contained value.
  /// The object is in disengaged state after the call.
  optional() {}

  /// Constructs the object without initializing the contained value.
  /// The object is in disengaged state after the call.
  optional(nullopt_t) {}

  /// Initializes the contained value by copying the contained value of other,
  ///  but only if other is engaged. other is still in engaged state if it was
  /// in engaged state before the call.
  optional(const optional<T> &other) {
    static_assert(std::is_copy_constructible<T>::value,
                  "This method should be deleted in this instantiation.");

    if (other.m_isset) construct_value(other.m_storage.get());
  }

  /// Initializes the contained value by moving the contained value of other,
  ///  but only if other is engaged. other is still in engaged state if it was
  /// in engaged state before the call.
  optional(optional<T> &&other) {
    static_assert(std::is_move_constructible<T>::value,
                  "This method should be deleted in this instantiation.");

    if (other.m_isset) construct_value(std::move(other.m_storage.get()));
  }

  /// Initializes the contained value by copying value.
  /// *this is in engaged state after the call.
  optional(const T &value) {
    static_assert(std::is_copy_constructible<T>::value,
                  "This method should be deleted in this instantiation.");

    construct_value(value);
  }

  /// Initializes the contained value by moving value.
  /// *this is in engaged state after the call.
  optional(T &&value) {
    static_assert(std::is_move_constructible<T>::value,
                  "This method should be deleted in this instantiation.");

    construct_value(std::move(value));
  }

  /// Initializes the contained value by constructing it in-place and passing
  /// args... to the constructor.
  /// *this is in engaged state after the call.
  template <typename... Args>
  explicit optional(in_place_t, Args &&... args) {
    construct_value(std::forward<Args>(args)...);
  }

  /// Initializes the contained value by constructing it in-place and passing
  /// the initializer list ilist and arguments args... to the constructor.
  /// *this is in engaged state after the call.
  /// The function does not participate in the overload resolution if
  /// std::is_constructible<T, std::initializer_list<U>&, Args&&...>::value !=
  /// true
  template <typename U, typename... Args,
   typename std::enable_if<std::is_constructible<T, std::initializer_list<U> &, Args &&...>::value, int>::type = 0>
  explicit optional(in_place_t, std::initializer_list<U> ilist,
                    Args &&... args) {
    construct_value(ilist, std::forward<Args>(args)...);
  }

  /// If the object is in engaged state, destroys the contained value by calling
  /// its destructor.
  ~optional() {
    static_assert(std::is_destructible<T>::value,
                  "This method should be deleted in this instantiation.");

    destroy_value();
  }

  /// If *this is in engaged state before the call, the contained value is
  /// destroyed by calling its destructor.
  /// *this is in disengaged state after the call.
  optional &operator=(nullopt_t) {
    destroy_value();
    return *this;
  }

  /// Assigns the state of other.
  /// * If both *this and other are in disengaged states, the function has no
  ///   effect.
  /// * If *this is in engaged state, but other is not, then the contained value
  ///   is destroyed by calling its destructor. *this is in disengaged state
  ///   after the call.
  /// * If other is in engaged state, then depending on whether *this is in
  ///   engaged state, the contained value is either direct-constructed or
  ///   assigned the contained value of other. The contained value is copied.
  optional &operator=(const optional<T> &other) {
    static_assert(std::is_copy_assignable<T>::value &&
                      std::is_copy_constructible<T>::value,
                  "This method should be deleted in this instantiation.");

    if (other.m_isset)
      assign_value(other.m_storage.get());
    else
      destroy_value();

    return *this;
  }

  /// Assigns the state of other.
  /// * If both *this and other are in disengaged states, the function has no
  ///   effect.
  /// * If *this is in engaged state, but other is not, then the contained value
  ///   is destroyed by calling its destructor. *this is in disengaged state
  ///   after the call.
  /// * If other is in engaged state, then depending on whether *this is in
  ///   engaged state, the contained value is either direct-constructed or
  ///   assigned the contained value of other. The contained value is moved.
  optional &operator=(optional<T> &&other) {
    static_assert(std::is_move_assignable<T>::value &&
                      std::is_move_constructible<T>::value,
                  "This method should be deleted in this instantiation.");

    if (other.m_isset)
      assign_value(std::move(other.m_storage.get()));
    else
      destroy_value();
    return *this;
  }

  /// Depending on whether *this is in engaged state before the call, the
  /// contained value is either direct-constructed from value or assigned value.
  /// T must be constructible and assignable from U.
    template <typename U, typename std::enable_if<std::is_same<T, typename std::decay<U>::type>::value, int>::type = 0>
  optional &operator=(U &&value) {
    static_assert(
        std::is_constructible<T, U>::value && std::is_assignable<T &, U>::value,
        "Cannot assign to value type from argument");

    assign_value(std::forward<U>(value));
    return *this;
  }

  /// Initializes the contained value by calling its constructor with args.. as
  /// parameters.
  /// If *this is in engaged state before the call, the contained value is
  /// destroyed by calling its destructor.
  template <typename... Args>
  void emplace(Args &&... args) {
    static_assert(std::is_constructible<T, Args &&...>::value,
                  "Cannot emplace value type from arguments");

    destroy_value();
    construct_value(std::forward<Args>(args)...);
  }

  /// Returns a pointer to the contained value.
  /// The behavior is undefined if *this is in disengaged state.
  T const *operator->() const {
    assert(m_isset);
    return &m_storage.get();
  }

  /// Returns a pointer to the contained value.
  /// The behavior is undefined if *this is in disengaged state.
  T *operator->() {
    assert(m_isset);
    return &m_storage.get();
  }

  /// Returns a reference to the contained value.
  /// The behavior is undefined if *this is in disengaged state.
  const T &operator*() const {
    assert(m_isset);
    return m_storage.get();
  }

  /// Returns a reference to the contained value.
  /// The behavior is undefined if *this is in disengaged state.
  T &operator*() {
    assert(m_isset);
    return m_storage.get();
  }

  /// Checks whether *this is in engaged state, i.e. whether the contained value
  /// is initialized.
  explicit operator bool() const { return m_isset; }

  /// Returns the contained value.
  T const &value() const {
    assert(m_isset);
    return m_storage.get();
  }

  /// Returns the contained value.
  T &value() {
    assert (m_isset);
    return m_storage.get();
  }

  /// Returns the contained value if engaged, another value otherwise.
  template <typename U>
  T value_or(U &&value) const {
    static_assert(std::is_convertible<U &&, T>::value,
                  "value should be implicitly convertible to T");
    return m_isset ? m_storage.get() : static_cast<T>(std::forward<U>(value));
  }

  /// Swaps the contents with those of other.
  void swap(optional<T> &other) {
    if (!m_isset && !other.m_isset) return;

    if (m_isset && !other.m_isset) {
      other = std::move(*this);
      destroy_value();
      return;
    }

    if (!m_isset && other.m_isset) {
      *this = std::move(other);
      other.destroy_value();
      return;
    }

    if (m_isset && other.m_isset) {
      std::swap(**this, *other);
      return;
    }
  }

 private:
  template <typename... Args>
  void construct_value(Args &&... args) {
    new (&m_storage.get()) T(std::forward<Args>(args)...);
    m_isset = true;
  }

  void destroy_value() {
    if (!std::is_pod<T>::value) {
      if (m_isset) m_storage.get().T::~T();
    }
    m_isset = false;
  }

  void assign_value(const T &value) {
    if (!m_isset)
      construct_value(value);
    else
      m_storage.get() = value;
  }

  void assign_value(T &&value) {
    if (!m_isset)
      construct_value(std::move(value));
    else
      m_storage.get() = std::move(value);
  }

 private:
  bool m_isset = false;
  aligned_storage_wrapper<T> m_storage;
};

template <typename T>
optional<typename std::decay<T>::type> make_optional(T &&value) {
  return optional<typename std::decay<T>::type>(std::forward<T>(value));
}

namespace std {
template <typename T>
void swap(optional<T> &lhs, optional<T> &rhs) {
  lhs.swap(rhs);
}
}

// [X.Y.8] Comparisons between optional values.
template <typename T>
bool operator==(const optional<T> &lhs, const optional<T> &rhs) {
  return static_cast<bool>(lhs) == static_cast<bool>(rhs) &&
         (!lhs || *lhs == *rhs);
}

template <typename T>
bool operator!=(const optional<T> &lhs, const optional<T> &rhs) {
  return !(lhs == rhs);
}

template <typename T>
bool operator<(const optional<T> &lhs, const optional<T> &rhs) {
  return static_cast<bool>(rhs) && (!lhs || *lhs < *rhs);
}

template <typename T>
bool operator>(const optional<T> &lhs, const optional<T> &rhs) {
  return rhs < lhs;
}

template <typename T>
bool operator<=(const optional<T> &lhs, const optional<T> &rhs) {
  return !(rhs < lhs);
}

template <typename T>
bool operator>=(const optional<T> &lhs, const optional<T> &rhs) {
  return !(lhs < rhs);
}

// [X.Y.9] Comparisons with nullopt.
template <typename T>
bool operator==(const optional<T> &lhs, nullopt_t) {
  return !lhs;
}

template <typename T>
bool operator==(nullopt_t, const optional<T> &rhs) {
  return !rhs;
}

template <typename T>
bool operator!=(const optional<T> &lhs, nullopt_t) {
  return static_cast<bool>(lhs);
}

template <typename T>
bool operator!=(nullopt_t, const optional<T> &rhs) {
  return static_cast<bool>(rhs);
}

template <typename T>
bool operator<(const optional<T> & /* lhs */, nullopt_t) {
  return false;
}

template <typename T>
bool operator<(nullopt_t, const optional<T> &rhs) {
  return static_cast<bool>(rhs);
}

template <typename T>
bool operator>(const optional<T> &lhs, nullopt_t) {
  return static_cast<bool>(lhs);
}

template <typename T>
bool operator>(nullopt_t, const optional<T> & /* rhs */) {
  return false;
}

template <typename T>
bool operator<=(const optional<T> &lhs, nullopt_t) {
  return !lhs;
}

template <typename T>
bool operator<=(nullopt_t, const optional<T> & /* rhs */) {
  return true;
}

template <typename T>
bool operator>=(const optional<T> & /* lhs */, nullopt_t) {
  return true;
}

template <typename T>
bool operator>=(nullopt_t, const optional<T> &rhs) {
  return !rhs;
}

// [X.Y.10] Comparisons with value type.
template <typename T>
bool operator==(const optional<T> &lhs, const T &rhs) {
  return lhs && *lhs == rhs;
}

template <typename T>
bool operator==(const T &lhs, const optional<T> &rhs) {
  return rhs && lhs == *rhs;
}

template <typename T>
bool operator!=(const optional<T> &lhs, T const &rhs) {
  return !lhs || !(*lhs == rhs);
}

template <typename T>
bool operator!=(const T &lhs, const optional<T> &rhs) {
  return !rhs || !(lhs == *rhs);
}

template <typename T>
bool operator<(const optional<T> &lhs, const T &rhs) {
  return !lhs || *lhs < rhs;
}

template <typename T>
bool operator<(const T &lhs, const optional<T> &rhs) {
  return rhs && lhs < *rhs;
}

template <typename T>
bool operator>(const optional<T> &lhs, const T &rhs) {
  return lhs && rhs < *lhs;
}

template <typename T>
bool operator>(const T &lhs, const optional<T> &rhs) {
  return !rhs || *rhs < lhs;
}

template <typename T>
bool operator<=(const optional<T> &lhs, const T &rhs) {
  return !lhs || !(rhs < *lhs);
}

template <typename T>
bool operator<=(const T &lhs, const optional<T> &rhs) {
  return rhs && !(*rhs < lhs);
}

template <typename T>
bool operator>=(const optional<T> &lhs, const T &rhs) {
  return lhs && !(*lhs < rhs);
}

template <typename T>
bool operator>=(const T &lhs, const optional<T> &rhs) {
  return !rhs || !(lhs < *rhs);
}

// Core/SmartPointers.hpp
#pragma once

/**
 * @file SmartPointers.hpp
 * @brief Utility smart-pointer types, pools and helpers for QObject-derived
 * types.
 *
 * This header provides:
 *  - a custom deleter that respects Qt parent ownership,
 *  - unique/shared aliases for Qt objects,
 *  - factory functions to create those smart pointers,
 *  - a small RAII wrapper for raw Qt pointers with parent assignment,
 *  - an object pool optimized for frequently created Qt objects,
 *  - a weak-reference wrapper that tracks QObject destruction,
 *  - and several memory helper utilities.
 *
 * All APIs are intended to make ownership semantics explicit and reduce
 * accidental double-deletes or leaks when working with QObject hierarchy.
 */

#include <QObject>
#include <QWidget>
#include <memory>
#include <type_traits>
#include <utility>
#include "Concepts.hpp"

namespace DeclarativeUI::Core {

/**
 * @brief Custom deleter for Qt objects that respects Qt's parent-child
 * ownership.
 *
 * If the pointed-to object is derived from QObject and has no parent,
 * deleteLater() is called to safely schedule deletion on the Qt event loop. For
 * non-QObject types standard delete is used.
 *
 * @tparam T pointed-to type.
 */
template <typename T>
struct QtDeleter {
    void operator()(T* ptr) const noexcept {
        if constexpr (std::derived_from<T, QObject>) {
            if (ptr && !ptr->parent()) {
                ptr->deleteLater();
            }
        } else {
            delete ptr;
        }
    }
};

/**
 * @brief Unique pointer type alias for QObject-derived types using QtDeleter.
 *
 * This alias uses QtDeleter to ensure QObject deletion respects parent
 * ownership.
 *
 * @tparam T QObject-derived type.
 */
template <typename T>
using qt_unique_ptr = std::unique_ptr<T, QtDeleter<T>>;

/**
 * @brief Shared pointer alias for QObject types.
 *
 * Note: shared pointers to QObjects must be used carefully; this alias exists
 * for convenience but shared ownership semantics can interfere with Qt parent
 * ownership.
 *
 * @tparam T QObject-derived type.
 */
template <typename T>
using qt_shared_ptr = std::shared_ptr<T>;

/**
 * @brief Factory to create a qt_unique_ptr for a QObject-derived type.
 *
 * Constructs T with forwarded arguments and returns a qt_unique_ptr that will
 * call QtDeleter on destruction.
 *
 * Usage:
 * @code
 * auto w = make_qt_unique<QWidget>(parentWidget);
 * @endcode
 *
 * @tparam T QObject-derived type.
 * @tparam Args constructor argument types.
 * @param args forwarded constructor arguments.
 * @return qt_unique_ptr<T> owning the newly created object.
 */
template <typename T, typename... Args>
    requires std::derived_from<T, QObject>
[[nodiscard]] constexpr auto make_qt_unique(Args&&... args)
    -> qt_unique_ptr<T> {
    return qt_unique_ptr<T>(new T(std::forward<Args>(args)...));
}

/**
 * @brief Factory to create a qt_shared_ptr for a QObject-derived type.
 *
 * Constructs T with forwarded arguments and returns a std::shared_ptr that
 * uses QtDeleter to schedule deletion via deleteLater if appropriate.
 *
 * @note Prefer unique pointers where possible; shared ownership of QObjects
 *       may be error-prone with Qt parent-child lifetimes.
 *
 * @tparam T QObject-derived type.
 * @tparam Args constructor argument types.
 * @param args forwarded constructor arguments.
 * @return qt_shared_ptr<T> owning the newly created object.
 */
template <typename T, typename... Args>
    requires std::derived_from<T, QObject>
[[nodiscard]] constexpr auto make_qt_shared(Args&&... args)
    -> qt_shared_ptr<T> {
    return std::shared_ptr<T>(new T(std::forward<Args>(args)...),
                              QtDeleter<T>{});
}

/**
 * @brief RAII wrapper for raw Qt pointers that optionally assigns a parent.
 *
 * QtObjectWrapper holds a raw pointer to a QObject-derived type and optionally
 * sets its parent at construction or reset. The wrapper is move-only to reflect
 * unique ownership semantics but does not delete the object itself; deletion
 * should follow Qt ownership rules (parenting) or external smart pointers.
 *
 * @tparam T QObject-derived pointer type (raw pointer).
 */
template <Concepts::QtObject T>
class QtObjectWrapper {
public:
    /**
     * @brief Construct the wrapper with an optional object and parent.
     * @param object raw pointer to manage (not deleted by wrapper).
     * @param parent optional parent to assign to the object.
     */
    explicit QtObjectWrapper(T* object = nullptr, QObject* parent = nullptr)
        : object_(object) {
        if (object_ && parent) {
            object_->setParent(parent);
        }
    }

    /** @brief Defaulted destructor. Does not delete the wrapped object. */
    ~QtObjectWrapper() = default;

    /** @name Move semantics
     * Move operations transfer the wrapped pointer and null the source.
     */
    QtObjectWrapper(QtObjectWrapper&& other) noexcept
        : object_(std::exchange(other.object_, nullptr)) {}

    QtObjectWrapper& operator=(QtObjectWrapper&& other) noexcept {
        if (this != &other) {
            object_ = std::exchange(other.object_, nullptr);
        }
        return *this;
    }

    /** Disable copy semantics to enforce unique-like ownership. */
    QtObjectWrapper(const QtObjectWrapper&) = delete;
    QtObjectWrapper& operator=(const QtObjectWrapper&) = delete;

    /** @name Accessors */
    [[nodiscard]] T* get() const noexcept {
        return object_;
    }  ///< Return raw pointer.
    [[nodiscard]] T* operator->() const noexcept {
        return object_;
    }  ///< Member access.
    [[nodiscard]] T& operator*() const noexcept {
        return *object_;
    }  ///< Dereference.
    [[nodiscard]] explicit operator bool() const noexcept {
        return object_ != nullptr;
    }  ///< Validity.

    /**
     * @brief Release ownership of the wrapped pointer.
     * @return the raw pointer previously held; wrapper becomes empty.
     */
    [[nodiscard]] T* release() noexcept {
        return std::exchange(object_, nullptr);
    }

    /**
     * @brief Reset the wrapper to manage a new object and optionally set its
     * parent. The previous pointer is not deleted by this wrapper.
     *
     * @param new_object new raw pointer to manage.
     * @param parent optional parent to assign to the new object.
     */
    void reset(T* new_object = nullptr, QObject* parent = nullptr) {
        object_ = new_object;
        if (object_ && parent) {
            object_->setParent(parent);
        }
    }

private:
    T* object_;  ///< The raw pointer being wrapped. Not owned for deletion by
                 ///< this wrapper.
};

/**
 * @brief Simple object pool for frequently allocated QObject-derived types.
 *
 * The pool stores qt_unique_ptr<T> instances for reuse to reduce allocation
 * churn. When an object is released back to the pool, an optional reset()
 * method is invoked (if present) to reinitialize state. If the pool is full
 * the unique_ptr is destroyed, allowing normal deletion semantics.
 *
 * @tparam T QObject-derived type to pool.
 */
template <typename T>
    requires std::derived_from<T, QObject>
class QtObjectPool {
public:
    static constexpr std::size_t DEFAULT_POOL_SIZE =
        32;  ///< Default maximum number of pooled objects.

    /**
     * @brief Construct a pool with an optional maximum size.
     * @param pool_size capacity of the pool.
     */
    explicit QtObjectPool(std::size_t pool_size = DEFAULT_POOL_SIZE)
        : pool_size_(pool_size) {
        pool_.reserve(pool_size_);
    }

    /** @brief Destructor clears the pool, allowing Qt deletion semantics to
     * run. */
    ~QtObjectPool() { clear(); }

    /**
     * @brief Acquire an object from the pool or create a new one.
     *
     * If the pool is non-empty, the last stored object is returned. Otherwise a
     * new object is constructed via make_qt_unique with forwarded arguments.
     *
     * @tparam Args constructor argument types.
     * @param args forwarded constructor arguments.
     * @return qt_unique_ptr<T> either recycled or newly created.
     */
    template <typename... Args>
    [[nodiscard]] qt_unique_ptr<T> acquire(Args&&... args) {
        if (!pool_.empty()) {
            auto obj = std::move(pool_.back());
            pool_.pop_back();
            return obj;
        }
        return make_qt_unique<T>(std::forward<Args>(args)...);
    }

    /**
     * @brief Return an object to the pool for later reuse.
     *
     * If the pooled object type defines a reset() method it will be invoked to
     * clear state before storing. If the pool is already at capacity the
     * object is simply destroyed.
     *
     * @param obj unique pointer to return to the pool.
     */
    void release(qt_unique_ptr<T> obj) {
        if (obj && pool_.size() < pool_size_) {
            // Reset object state if needed
            if constexpr (requires { obj->reset(); }) {
                obj->reset();
            }
            pool_.push_back(std::move(obj));
        }
        // If pool is full, object will be automatically destroyed
    }

    /** @brief Clear all pooled objects immediately. */
    void clear() { pool_.clear(); }

    /** @brief Get the current number of objects stored in the pool. */
    [[nodiscard]] std::size_t size() const noexcept { return pool_.size(); }

    /** @brief Check whether the pool is empty. */
    [[nodiscard]] bool empty() const noexcept { return pool_.empty(); }

private:
    std::size_t pool_size_;  ///< Maximum number of items to retain.
    std::vector<qt_unique_ptr<T>> pool_;  ///< Container holding pooled objects.
};

/**
 * @brief Lightweight weak-reference wrapper for QObject-derived objects.
 *
 * QtWeakRef stores a raw pointer and connects to the QObject::destroyed signal
 * to automatically nullify its pointer when the object is deleted. This
 * provides a low-overhead way to detect lifetime without using shared_ptr.
 *
 * The class offers:
 *  - isValid() to test liveness,
 *  - get() to retrieve the raw pointer (may be nullptr),
 *  - withObject(F) convenience to invoke a callable with the object if alive,
 *  - reset() to repoint the reference.
 *
 * @tparam T QObject-derived type.
 */
template <Concepts::QtObject T>
class QtWeakRef {
public:
    /**
     * @brief Construct a weak reference optionally pointing at an object.
     * If object is non-null a connection to destroyed is created to clear the
     * ref.
     *
     * @param object optional pointer to track.
     */
    explicit QtWeakRef(T* object = nullptr) : object_(object) {
        if (object_) {
            // Connect to destroyed signal to automatically nullify reference
            QObject::connect(object_, &QObject::destroyed,
                             [this]() { object_ = nullptr; });
        }
    }

    /**
     * @brief Returns true if the referenced object is still alive.
     * @return true if pointer is not null (object not yet destroyed).
     */
    [[nodiscard]] bool isValid() const noexcept { return object_ != nullptr; }

    /**
     * @brief Get the raw pointer being referenced.
     * @return raw pointer or nullptr if the object was destroyed.
     */
    [[nodiscard]] T* get() const noexcept { return object_; }

    /**
     * @brief Invoke a callable with the object pointer if still alive.
     *
     * If the object is null the callable is not invoked. If the callable
     * returns a non-void value and the object is null, a default-constructed
     * return value is produced (via value-initialization).
     *
     * @tparam F callable type invocable with T*.
     * @param func callable to invoke when object is alive.
     * @return result of invoking func or default-constructed value if object is
     * null.
     */
    template <typename F>
        requires std::invocable<F, T*>
    auto withObject(F&& func) const -> std::invoke_result_t<F, T*> {
        if (object_) {
            return std::invoke(std::forward<F>(func), object_);
        }
        if constexpr (!std::is_void_v<std::invoke_result_t<F, T*>>) {
            return std::invoke_result_t<F, T*>{};
        }
    }

    /**
     * @brief Reset the weak reference to a new object pointer.
     * If new_object is non-null a destroyed connection is created to clear the
     * ref.
     *
     * @param new_object new pointer to track (or nullptr).
     */
    void reset(T* new_object = nullptr) {
        object_ = new_object;
        if (object_) {
            QObject::connect(object_, &QObject::destroyed,
                             [this]() { object_ = nullptr; });
        }
    }

private:
    T* object_;  ///< Raw pointer being observed; nullified when the QObject is
                 ///< destroyed.
};

/**
 * @brief Memory helper utilities for Qt object management.
 *
 * The Memory namespace contains convenience helpers such as safe_cast
 * (null-safe qobject_cast), createWithParent to construct an object and assign
 * a parent, batchDelete to schedule deletion for multiple objects, and isAlive
 * checks.
 */
namespace Memory {

/**
 * @brief Null-safe cast wrapper around qobject_cast.
 *
 * Returns a pointer of type To* if the cast succeeds, otherwise nullptr.
 *
 * @tparam To target QObject-derived type.
 * @tparam From source QObject-derived type.
 * @param from pointer to cast.
 * @return To* result of qobject_cast or nullptr.
 */
template <typename To, typename From>
    requires std::derived_from<From, QObject>
[[nodiscard]] constexpr auto safe_cast(From* from) noexcept -> To* {
    return from ? qobject_cast<To*>(from) : nullptr;
}

/**
 * @brief Safe cast overload for qt_unique_ptr.
 *
 * Convenience overload to cast the raw pointer held by a qt_unique_ptr.
 *
 * @tparam To target QObject-derived type.
 * @tparam From source QObject-derived type.
 * @param from unique pointer to cast.
 * @return To* result of qobject_cast or nullptr.
 */
template <typename To, typename From>
    requires std::derived_from<From, QObject>
[[nodiscard]] constexpr auto safe_cast(const qt_unique_ptr<From>& from) noexcept
    -> To* {
    return safe_cast<To>(from.get());
}

/**
 * @brief Check if a raw object pointer appears alive (non-null).
 *
 * This function is a trivial wrapper returning pointer != nullptr and exists
 * for consistent naming with other helpers.
 *
 * @tparam T QObject-derived type.
 * @param object pointer to check.
 * @return true if pointer is non-null.
 */
template <Concepts::QtObject T>
[[nodiscard]] constexpr bool isAlive(T* object) noexcept {
    return object != nullptr;
}

/**
 * @brief Construct an object and assign a parent immediately.
 *
 * Equivalent to new T(args...), followed by setParent(parent) if parent is not
 * null. Useful to ensure Qt parent-child ownership is configured at creation
 * time.
 *
 * @tparam T QObject-derived type to create.
 * @tparam Parent QObject-derived type used as parent.
 * @tparam Args constructor argument types for T.
 * @param parent parent pointer to assign.
 * @param args forwarded constructor arguments.
 * @return pointer to the newly created object.
 */
template <typename T, typename Parent, typename... Args>
    requires std::derived_from<T, QObject> && std::derived_from<Parent, QObject>
[[nodiscard]] constexpr auto createWithParent(Parent* parent, Args&&... args)
    -> T* {
    auto* object = new T(std::forward<Args>(args)...);
    if (parent) {
        object->setParent(parent);
    }
    return object;
}

/**
 * @brief Batch delete or schedule deletion for a container of Qt objects.
 *
 * Supports containers of raw pointers or smart pointers that expose get().
 * For each element, if the object has no parent deleteLater() is called to
 * schedule safe deletion on the Qt event loop.
 *
 * @tparam Container container type (iterable) whose elements are pointers or
 * smart pointers.
 * @param objects container of objects to delete.
 */
template <typename Container>
    requires Concepts::Container<Container>
void batchDelete(Container&& objects) {
    for (auto&& obj : objects) {
        if constexpr (std::is_pointer_v<std::decay_t<decltype(obj)>>) {
            if (obj && !obj->parent()) {
                obj->deleteLater();
            }
        } else if constexpr (requires { obj.get(); }) {
            if (auto* ptr = obj.get(); ptr && !ptr->parent()) {
                ptr->deleteLater();
            }
        }
    }
}

}  // namespace Memory

}  // namespace DeclarativeUI::Core

// Core/SmartPointers.hpp
#pragma once

#include <memory>
#include <type_traits>
#include <utility>
#include <QObject>
#include <QWidget>
#include "Concepts.hpp"

namespace DeclarativeUI::Core {

// **Custom deleter for Qt objects that respects Qt's parent-child ownership**
template<typename T>
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

// **Smart pointer type aliases for Qt objects**
template<typename T>
using qt_unique_ptr = std::unique_ptr<T, QtDeleter<T>>;

template<typename T>
using qt_shared_ptr = std::shared_ptr<T>;

// **Factory functions for creating Qt smart pointers**
template<typename T, typename... Args>
requires std::derived_from<T, QObject>
[[nodiscard]] constexpr auto make_qt_unique(Args&&... args) -> qt_unique_ptr<T> {
    return qt_unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<typename T, typename... Args>
requires std::derived_from<T, QObject>
[[nodiscard]] constexpr auto make_qt_shared(Args&&... args) -> qt_shared_ptr<T> {
    return std::shared_ptr<T>(new T(std::forward<Args>(args)...), QtDeleter<T>{});
}

// **RAII wrapper for Qt objects with automatic parent management**
template<Concepts::QtObject T>
class QtObjectWrapper {
public:
    explicit QtObjectWrapper(T* object = nullptr, QObject* parent = nullptr) 
        : object_(object) {
        if (object_ && parent) {
            object_->setParent(parent);
        }
    }

    ~QtObjectWrapper() = default;

    // **Move semantics**
    QtObjectWrapper(QtObjectWrapper&& other) noexcept 
        : object_(std::exchange(other.object_, nullptr)) {}

    QtObjectWrapper& operator=(QtObjectWrapper&& other) noexcept {
        if (this != &other) {
            object_ = std::exchange(other.object_, nullptr);
        }
        return *this;
    }

    // **Disable copy semantics**
    QtObjectWrapper(const QtObjectWrapper&) = delete;
    QtObjectWrapper& operator=(const QtObjectWrapper&) = delete;

    // **Access operators**
    [[nodiscard]] T* get() const noexcept { return object_; }
    [[nodiscard]] T* operator->() const noexcept { return object_; }
    [[nodiscard]] T& operator*() const noexcept { return *object_; }
    [[nodiscard]] explicit operator bool() const noexcept { return object_ != nullptr; }

    // **Release ownership**
    [[nodiscard]] T* release() noexcept {
        return std::exchange(object_, nullptr);
    }

    // **Reset with new object**
    void reset(T* new_object = nullptr, QObject* parent = nullptr) {
        object_ = new_object;
        if (object_ && parent) {
            object_->setParent(parent);
        }
    }

private:
    T* object_;
};

// **Memory pool for frequently allocated Qt objects**
template<typename T>
requires std::derived_from<T, QObject>
class QtObjectPool {
public:
    static constexpr std::size_t DEFAULT_POOL_SIZE = 32;

    explicit QtObjectPool(std::size_t pool_size = DEFAULT_POOL_SIZE) 
        : pool_size_(pool_size) {
        pool_.reserve(pool_size_);
    }

    ~QtObjectPool() {
        clear();
    }

    // **Acquire object from pool or create new one**
    template<typename... Args>
    [[nodiscard]] qt_unique_ptr<T> acquire(Args&&... args) {
        if (!pool_.empty()) {
            auto obj = std::move(pool_.back());
            pool_.pop_back();
            return obj;
        }
        return make_qt_unique<T>(std::forward<Args>(args)...);
    }

    // **Return object to pool**
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

    // **Clear all pooled objects**
    void clear() {
        pool_.clear();
    }

    // **Get current pool size**
    [[nodiscard]] std::size_t size() const noexcept {
        return pool_.size();
    }

    // **Check if pool is empty**
    [[nodiscard]] bool empty() const noexcept {
        return pool_.empty();
    }

private:
    std::size_t pool_size_;
    std::vector<qt_unique_ptr<T>> pool_;
};

// **Weak reference wrapper for Qt objects**
template<Concepts::QtObject T>
class QtWeakRef {
public:
    explicit QtWeakRef(T* object = nullptr) : object_(object) {
        if (object_) {
            // Connect to destroyed signal to automatically nullify reference
            QObject::connect(object_, &QObject::destroyed, 
                           [this]() { object_ = nullptr; });
        }
    }

    // **Check if reference is still valid**
    [[nodiscard]] bool isValid() const noexcept {
        return object_ != nullptr;
    }

    // **Get the object (may return nullptr if destroyed)**
    [[nodiscard]] T* get() const noexcept {
        return object_;
    }

    // **Safe access with validity check**
    template<typename F>
    requires std::invocable<F, T*>
    auto withObject(F&& func) const -> std::invoke_result_t<F, T*> {
        if (object_) {
            return std::invoke(std::forward<F>(func), object_);
        }
        if constexpr (!std::is_void_v<std::invoke_result_t<F, T*>>) {
            return std::invoke_result_t<F, T*>{};
        }
    }

    // **Reset reference**
    void reset(T* new_object = nullptr) {
        object_ = new_object;
        if (object_) {
            QObject::connect(object_, &QObject::destroyed, 
                           [this]() { object_ = nullptr; });
        }
    }

private:
    T* object_;
};

// **Utility functions for memory management**
namespace Memory {

// **Safe cast with null check**
template<typename To, typename From>
requires std::derived_from<From, QObject>
[[nodiscard]] constexpr auto safe_cast(From* from) noexcept -> To* {
    return from ? qobject_cast<To*>(from) : nullptr;
}

// **Safe cast for smart pointers**
template<typename To, typename From>
requires std::derived_from<From, QObject>
[[nodiscard]] constexpr auto safe_cast(const qt_unique_ptr<From>& from) noexcept -> To* {
    return safe_cast<To>(from.get());
}

// **Check if object is still alive**
template<Concepts::QtObject T>
[[nodiscard]] constexpr bool isAlive(T* object) noexcept {
    return object != nullptr;
}

// **Create object with automatic parent assignment**
template<typename T, typename Parent, typename... Args>
requires std::derived_from<T, QObject> && std::derived_from<Parent, QObject>
[[nodiscard]] constexpr auto createWithParent(Parent* parent, Args&&... args) -> T* {
    auto* object = new T(std::forward<Args>(args)...);
    if (parent) {
        object->setParent(parent);
    }
    return object;
}

// **Batch delete Qt objects**
template<typename Container>
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

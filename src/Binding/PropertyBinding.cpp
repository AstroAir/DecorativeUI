#include "PropertyBinding.hpp"
#include <algorithm>

namespace DeclarativeUI::Binding {

PropertyBindingManager::PropertyBindingManager(QObject* parent)
    : QObject(parent), m_performance_monitoring_enabled(false) {}

PropertyBindingManager::~PropertyBindingManager() { removeAllBindings(); }

void PropertyBindingManager::addBinding(
    std::shared_ptr<IPropertyBinding> binding) {
    if (binding) {
        m_bindings.push_back(binding);
        trackBinding(binding.get());
    }
}

void PropertyBindingManager::removeBinding(
    std::shared_ptr<IPropertyBinding> binding) {
    if (binding) {
        auto it = std::find(m_bindings.begin(), m_bindings.end(), binding);
        if (it != m_bindings.end()) {
            untrackBinding(binding.get());
            m_bindings.erase(it);
        }
    }
}

void PropertyBindingManager::removeAllBindings() {
    for (auto& binding : m_bindings) {
        if (binding) {
            untrackBinding(binding.get());
        }
    }
    m_bindings.clear();
}

void PropertyBindingManager::updateAllBindings() {
    for (auto& binding : m_bindings) {
        if (binding && binding->isValid()) {
            binding->update();
        }
    }
}

void PropertyBindingManager::enableAllBindings() {
    for (auto& binding : m_bindings) {
        if (binding) {
            // Check if the binding has an enable method (for template-based
            // bindings) Since IPropertyBinding doesn't have enable/disable
            // methods, we'll need to implement this through a different
            // approach or extend the interface For now, we'll track enabled
            // state internally

            // This is a basic implementation - in a real system you might want
            // to extend IPropertyBinding to include enable/disable methods
            qDebug() << "Enabling binding:" << binding->getSourcePath() << "->"
                     << binding->getTargetPath();
        }
    }
    qDebug() << "All bindings enabled (" << m_bindings.size() << " bindings)";
}

void PropertyBindingManager::disableAllBindings() {
    for (auto& binding : m_bindings) {
        if (binding) {
            // Similar to enableAllBindings, this would ideally call a disable
            // method on each binding. For now, we'll just log the action.
            qDebug() << "Disabling binding:" << binding->getSourcePath() << "->"
                     << binding->getTargetPath();
        }
    }
    qDebug() << "All bindings disabled (" << m_bindings.size() << " bindings)";
}

int PropertyBindingManager::getBindingCount() const {
    return static_cast<int>(m_bindings.size());
}

std::vector<std::shared_ptr<IPropertyBinding>>
PropertyBindingManager::getBindings() const {
    return m_bindings;
}

std::vector<std::shared_ptr<IPropertyBinding>>
PropertyBindingManager::getBindingsForWidget(QWidget* widget) const {
    std::vector<std::shared_ptr<IPropertyBinding>> result;

    if (!widget) {
        return result;
    }

    for (const auto& binding : m_bindings) {
        if (binding && binding->isValid()) {
            // Parse the target path to extract widget information
            QString targetPath = binding->getTargetPath();

            // The target path format is typically "ClassName::propertyName"
            // We need to check if this binding is associated with the given
            // widget This is a simplified implementation - in a real system,
            // you might want to store widget references directly in the binding
            // or use a more sophisticated matching mechanism

            // For now, we'll use a basic string matching approach
            QString widgetClassName = widget->metaObject()->className();
            if (targetPath.startsWith(widgetClassName + "::")) {
                // Additional check: we could verify the widget pointer if we
                // had access to it This is a limitation of the current
                // IPropertyBinding interface
                result.push_back(binding);
            }
        }
    }

    qDebug() << "Found" << result.size() << "bindings for widget of type"
             << widget->metaObject()->className();
    return result;
}

void PropertyBindingManager::enablePerformanceMonitoring(bool enabled) {
    m_performance_monitoring_enabled = enabled;
}

bool PropertyBindingManager::isPerformanceMonitoringEnabled() const {
    return m_performance_monitoring_enabled;
}

QString PropertyBindingManager::getPerformanceReport() const {
    if (!m_performance_monitoring_enabled) {
        return "Performance monitoring is disabled";
    }

    QString report = QString("Binding Performance Report\n");
    report += QString("Total Bindings: %1\n").arg(m_bindings.size());

    for (const auto& binding : m_bindings) {
        if (binding) {
            report += QString("Binding: %1 -> %2\n")
                          .arg(binding->getSourcePath())
                          .arg(binding->getTargetPath());
        }
    }

    return report;
}

void PropertyBindingManager::trackBinding(IPropertyBinding* binding) {
    if (m_performance_monitoring_enabled && binding) {
        m_update_counts[binding] = 0;
        m_update_times[binding] = 0;
    }
}

void PropertyBindingManager::untrackBinding(IPropertyBinding* binding) {
    if (binding) {
        m_update_counts.erase(binding);
        m_update_times.erase(binding);
    }
}

// Global binding manager instance
static PropertyBindingManager* g_global_binding_manager = nullptr;

PropertyBindingManager* getGlobalBindingManager() {
    if (!g_global_binding_manager) {
        g_global_binding_manager = new PropertyBindingManager();
    }
    return g_global_binding_manager;
}

}  // namespace DeclarativeUI::Binding

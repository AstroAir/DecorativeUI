#include "DeclarativeBuilder.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMetaMethod>
#include <QMetaObject>
#include <QProgressBar>
#include <QRadioButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QSplitter>
#include <QStackedLayout>
#include <QTextEdit>
#include <QVBoxLayout>

namespace DeclarativeUI::Core {

// **Explicit template instantiations for common widget types**
template class DeclarativeBuilder<QWidget>;
template class DeclarativeBuilder<QPushButton>;
template class DeclarativeBuilder<QLabel>;
template class DeclarativeBuilder<QLineEdit>;
template class DeclarativeBuilder<QTextEdit>;
template class DeclarativeBuilder<QCheckBox>;
template class DeclarativeBuilder<QRadioButton>;
template class DeclarativeBuilder<QComboBox>;
template class DeclarativeBuilder<QSpinBox>;
template class DeclarativeBuilder<QDoubleSpinBox>;
template class DeclarativeBuilder<QProgressBar>;
template class DeclarativeBuilder<QGroupBox>;
template class DeclarativeBuilder<QFrame>;
template class DeclarativeBuilder<QScrollArea>;

}  // namespace DeclarativeUI::Core

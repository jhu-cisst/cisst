/*
  (C) Copyright 2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnQt.h>

#include <QApplication>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLayout>
#include <QScreen>
#include <QTimer>
#include <QVariant>
#include <QWheelEvent>

#include <algorithm>
#include <cmath>

namespace {

class FontZoom: public QObject
{
public:
    explicit FontZoom(QApplication * application): QObject(application) {}

protected:
    bool eventFilter(QObject * watched, QEvent * event) override
    {
        if ((event->type() != QEvent::ShortcutOverride)
            && (event->type() != QEvent::KeyPress)
            && (event->type() != QEvent::Wheel)) {
            return false;
        }
        auto * widget = qobject_cast<QWidget *>(watched);
        if (!widget) {
            return false;
        }
        QWidget * window = widget->window();
        // Menus and tooltips should retain their ordinary input behavior.
        if ((window->windowType() == Qt::Popup) || (window->windowType() == Qt::ToolTip)) {
            return false;
        }
        int steps = window->property("_cmn_qt_zoom_steps").toInt();
        if (event->type() == QEvent::Wheel) {
            auto * wheel = static_cast<QWheelEvent *>(event);
            if (!(wheel->modifiers() & Qt::ControlModifier)) {
                window->setProperty("_cmn_qt_zoom_wheel", 0);
                window->setProperty("_cmn_qt_zoom_pixels", 0);
                return false;
            }
            if (wheel->modifiers() & (Qt::AltModifier | Qt::MetaModifier)) {
                return false;
            }
            // Accumulate high-resolution wheel events rather than treating
            // every small trackpad movement as a whole zoom step.
            const bool angle = wheel->angleDelta().y() != 0;
            const int delta = angle ? wheel->angleDelta().y() : wheel->pixelDelta().y();
            if (delta == 0) {
                return false;
            }
            const char * property = angle ? "_cmn_qt_zoom_wheel" : "_cmn_qt_zoom_pixels";
            const int threshold = angle ? 120 : 40;
            const int accumulated = window->property(property).toInt() + delta;
            window->setProperty(property, accumulated % threshold);
            SetZoom(window, steps + accumulated / threshold);
        } else {
            auto * key = static_cast<QKeyEvent *>(event);
            if (!(key->modifiers() & Qt::ControlModifier)
                || (key->modifiers() & (Qt::AltModifier | Qt::MetaModifier))) {
                return false;
            }
            switch (key->key()) {
            case Qt::Key_Plus:
            case Qt::Key_Equal:
                ++steps;
                break;
            case Qt::Key_Minus:
                --steps;
                break;
            case Qt::Key_0:
                steps = 0;
                break;
            default:
                return false;
            }
            if (event->type() == QEvent::KeyPress) {
                window->setProperty("_cmn_qt_zoom_wheel", 0);
                window->setProperty("_cmn_qt_zoom_pixels", 0);
                SetZoom(window, steps);
            }
        }
        // Reserve the shortcut and consume Ctrl+wheel before an editor can
        // change its contents or a spin box can change its numeric value.
        event->accept();
        return true;
    }

private:
    static void SetZoom(QWidget * window, int steps)
    {
        steps = std::max(-4, std::min(6, steps));
        const int previousSteps = window->property("_cmn_qt_zoom_steps").toInt();
        if (steps == previousSteps) {
            return;
        }
        const double previousScale = std::pow(1.1, previousSteps);
        const double scale = std::pow(1.1, steps);
        if (previousSteps == 0) {
            window->setProperty("_cmn_qt_zoom_size", window->size());
        }
        const QSize targetSize = window->property("_cmn_qt_zoom_size").toSize() * scale;
        auto widgets = window->findChildren<QWidget *>();
        widgets.prepend(window);
        // Snapshot before changing fonts, since children can inherit them.
        for (auto * widget : widgets) {
            if ((widget->window() != window) || widget->property("_cmn_qt_zoom_font").isValid()) {
                continue;
            }
            QFont baseFont = widget->font();
            if (baseFont.pointSizeF() > 0.0) {
                baseFont.setPointSizeF(baseFont.pointSizeF() / previousScale);
            } else {
                baseFont.setPixelSize(std::max(1, qRound(baseFont.pixelSize() / previousScale)));
            }
            widget->setProperty("_cmn_qt_zoom_font", baseFont);
            // Shared vector displays and aligned labels fix their dimensions
            // using font metrics at construction time.
            if (widget->minimumWidth() == widget->maximumWidth()) {
                widget->setProperty("_cmn_qt_zoom_width", widget->width() / previousScale);
            }
            if (widget->minimumHeight() == widget->maximumHeight()) {
                widget->setProperty("_cmn_qt_zoom_height", widget->height() / previousScale);
            }
            if (auto * header = qobject_cast<QHeaderView *>(widget)) {
                header->setProperty("_cmn_qt_zoom_section", header->defaultSectionSize() / previousScale);
                header->setProperty("_cmn_qt_zoom_min_section", header->minimumSectionSize() / previousScale);
            }
        }
        for (auto * widget : widgets) {
            if (widget->window() != window) {
                continue;
            }
            const QFont baseFont = qvariant_cast<QFont>(widget->property("_cmn_qt_zoom_font"));
            // Preserve live styling, including strikeout and bold state.
            QFont font = widget->font();
            if (baseFont.pointSizeF() > 0.0) {
                font.setPointSizeF(baseFont.pointSizeF() * scale);
            } else {
                font.setPixelSize(std::max(1, qRound(baseFont.pixelSize() * scale)));
            }
            widget->setFont(font);
            if (widget->property("_cmn_qt_zoom_width").isValid()) {
                widget->setFixedWidth(qRound(widget->property("_cmn_qt_zoom_width").toDouble() * scale));
            }
            if (widget->property("_cmn_qt_zoom_height").isValid()) {
                widget->setFixedHeight(qRound(widget->property("_cmn_qt_zoom_height").toDouble() * scale));
            }
            if (auto * header = qobject_cast<QHeaderView *>(widget)) {
                header->setMinimumSectionSize(qRound(header->property("_cmn_qt_zoom_min_section").toDouble() * scale));
                header->setDefaultSectionSize(qRound(header->property("_cmn_qt_zoom_section").toDouble() * scale));
            }
            if (widget->layout()) {
                widget->layout()->invalidate();
            }
            widget->updateGeometry();
        }
        window->setProperty("_cmn_qt_zoom_steps", steps);
        // Fit after pending font/layout requests. The context prevents this
        // callback from running if the window is destroyed in the meantime.
        QTimer::singleShot(0, window, [window, targetSize] {
            if (window->isMaximized() || window->isFullScreen()) {
                return;
            }
            if (window->layout()) {
                window->layout()->activate();
            }
            QScreen * screen = QGuiApplication::screenAt(window->frameGeometry().center());
            if (!screen) {
                screen = QGuiApplication::primaryScreen();
            }
            if (screen) {
                const QRect available = screen->availableGeometry();
                const QSize decoration = window->frameGeometry().size() - window->size();
                window->resize(targetSize.expandedTo(window->minimumSizeHint()).boundedTo(available.size() - decoration));
                const QRect frame = window->frameGeometry();
                const int left = std::max(available.left(), std::min(frame.left(), available.right() - frame.width() + 1));
                const int top = std::max(available.top(), std::min(frame.top(), available.bottom() - frame.height() + 1));
                window->move(window->pos() + QPoint(left - frame.left(), top - frame.top()));
            } else {
                window->resize(targetSize.expandedTo(window->minimumSizeHint()));
            }
        });
    }
};

}

void cmnQt::EnableFontZoom(bool enable)
{
    auto * application = qobject_cast<QApplication *>(QCoreApplication::instance());
    if (!application) {
        return;
    }
    const QString name = QStringLiteral("_cmn_qt_font_zoom");
    QObject * filter = application->findChild<QObject *>(name, Qt::FindDirectChildrenOnly);
    if (!filter && enable) {
        filter = new FontZoom(application);
        filter->setObjectName(name);
    }
    if (filter) {
        if (enable) {
            application->installEventFilter(filter);
        } else {
            application->removeEventFilter(filter);
        }
    }
}

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2010-03-20

  (C) Copyright 2010-2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnLoggerQtWidget.h>
#include <cisstCommon/cmnClassRegister.h>
#include <cisstCommon/cmnLogger.h>

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QTabWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QHeaderView>
#include <QAbstractTableModel>
#include <QItemDelegate>
#include <QTableView>
#include <QSortFilterProxyModel>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QTimer>
#include <QFont>
#include <QTime>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QPalette>

#include <streambuf>
#include <ostream>
#include <vector>
#include <string>

CMN_IMPLEMENT_SERVICES(cmnLoggerQtWidget);

// -----------------------------------------------------------------------
// Per-nibble mask preset tables
// Init: low nibble (bits 0-3).  Run: high nibble (bits 4-7).
// Labels follow the CMN_LOG_ALLOW_* naming convention.
// -----------------------------------------------------------------------
struct cmnLoggerQtMaskEntry {
    const char * Name;
    cmnLogMask   Mask;
};

static const cmnLoggerQtMaskEntry cmnLoggerQtInitMaskEntries[] = {
    {"ALLOW_NONE",                CMN_LOG_ALLOW_NONE},
    {"ALLOW_ERRORS",              CMN_LOG_LEVEL_INIT_ERROR},
    {"ALLOW_ERRORS_AND_WARNINGS", (cmnLogMask)(CMN_LOG_LEVEL_INIT_ERROR | CMN_LOG_LEVEL_INIT_WARNING)},
    {"ALLOW_VERBOSE",             (cmnLogMask)(CMN_LOG_LEVEL_INIT_ERROR | CMN_LOG_LEVEL_INIT_WARNING | CMN_LOG_LEVEL_INIT_VERBOSE)},
    {"ALLOW_ALL",                 (cmnLogMask)(CMN_LOG_LEVEL_INIT_ERROR | CMN_LOG_LEVEL_INIT_WARNING | CMN_LOG_LEVEL_INIT_VERBOSE | CMN_LOG_LEVEL_INIT_DEBUG)},
    {0, 0}
};

static const cmnLoggerQtMaskEntry cmnLoggerQtRunMaskEntries[] = {
    {"ALLOW_NONE",                CMN_LOG_ALLOW_NONE},
    {"ALLOW_ERRORS",              CMN_LOG_LEVEL_RUN_ERROR},
    {"ALLOW_ERRORS_AND_WARNINGS", (cmnLogMask)(CMN_LOG_LEVEL_RUN_ERROR | CMN_LOG_LEVEL_RUN_WARNING)},
    {"ALLOW_VERBOSE",             (cmnLogMask)(CMN_LOG_LEVEL_RUN_ERROR | CMN_LOG_LEVEL_RUN_WARNING | CMN_LOG_LEVEL_RUN_VERBOSE)},
    {"ALLOW_ALL",                 (cmnLogMask)(CMN_LOG_LEVEL_RUN_ERROR | CMN_LOG_LEVEL_RUN_WARNING | CMN_LOG_LEVEL_RUN_VERBOSE | CMN_LOG_LEVEL_RUN_DEBUG)},
    {0, 0}
};

// Extract init & run nibbles from a combined mask
static inline cmnLogMask initNibble(cmnLogMask m) { return (cmnLogMask)(m & 0x0F); }
static inline cmnLogMask runNibble (cmnLogMask m) { return (cmnLogMask)(m & 0xF0); }

// Populate a QComboBox from a null-terminated mask entry table
static void populateCombo(QComboBox * combo, const cmnLoggerQtMaskEntry * table) {
    for (const cmnLoggerQtMaskEntry * e = table; e->Name; ++e)
        combo->addItem(e->Name, (unsigned int)e->Mask);
}

// Select the combo item whose data matches mask; fall back to index 0
static void selectComboByMask(QComboBox * combo, cmnLogMask mask) {
    int i = combo->findData((unsigned int)mask);
    combo->setCurrentIndex(i != -1 ? i : 0);
}

// Look up the ALLOW_* label for a nibble value; return hex string if unknown
static QString maskNibbleName(cmnLogMask nibbleMask, const cmnLoggerQtMaskEntry * table) {
    for (const cmnLoggerQtMaskEntry * e = table; e->Name; ++e)
        if (e->Mask == nibbleMask) return QString(e->Name);
    return QString("0x%1").arg((unsigned int)nibbleMask, 2, 16, QChar('0')).toUpper();
}

// -----------------------------------------------------------------------
// cmnLoggerQtStreambufEmitter
// QObject shim: bridges the non-QObject streambuf world to Qt signals.
// Lives on the GUI thread; called from any thread via QueuedConnection.
// -----------------------------------------------------------------------
class cmnLoggerQtStreambufEmitter : public QObject
{
    Q_OBJECT
public:
    explicit cmnLoggerQtStreambufEmitter(QObject * parent = nullptr): QObject(parent) {}
    // Called from the streambuf (potentially a non-GUI thread)
    void emitLine(const std::string & text) { emit LineReady(QString::fromStdString(text)); }
signals:
    void LineReady(const QString & text);
};

// -----------------------------------------------------------------------
// cmnLoggerQtStreambuf
// Accumulates characters and ships complete lines to the emitter.
// Buffer is thread_local so no locking is needed — zero overhead
// for the calling thread even if multiple threads log simultaneously.
// -----------------------------------------------------------------------
class cmnLoggerQtStreambuf : public std::streambuf
{
public:
    explicit cmnLoggerQtStreambuf(cmnLoggerQtStreambufEmitter * emitter): Emitter(emitter) {}

protected:
    // Single-character path
    int_type overflow(int_type c) override {
        if (c != EOF) {
            char ch = static_cast<char>(c);
            tlBuffer() += ch;
            if (ch == '\n') flushBuffer();
        }
        return c;
    }

    // Multi-character path: flush each newline-terminated segment immediately
    std::streamsize xsputn(const char * s, std::streamsize n) override {
        tlBuffer().append(s, static_cast<size_t>(n));
        size_t pos;
        while ((pos = tlBuffer().find('\n')) != std::string::npos) {
            Emitter->emitLine(tlBuffer().substr(0, pos + 1));
            tlBuffer().erase(0, pos + 1);
        }
        return n;
    }

    // Flush any remaining buffered text without a trailing newline
    int sync() override { flushBuffer(); return 0; }

private:
    // Per-thread buffer: each calling thread has its own string, no mutex needed
    static std::string & tlBuffer() {
        static thread_local std::string buffer;
        return buffer;
    }
    void flushBuffer() {
        std::string & buf = tlBuffer();
        if (!buf.empty()) { Emitter->emitLine(buf); buf.clear(); }
    }

    cmnLoggerQtStreambufEmitter * Emitter;
};

// -----------------------------------------------------------------------
// cmnLoggerQtWidgetClassServicesModel
// QAbstractTableModel with three columns: Class | Init mask | Run mask
// -----------------------------------------------------------------------
class cmnLoggerQtWidgetClassServicesModel : public QAbstractTableModel
{
    struct Entry { std::string Name; cmnClassServicesBase * Services; };
    std::vector<Entry> Entries;

public:
    cmnLoggerQtWidgetClassServicesModel(QObject * parent = nullptr)
        : QAbstractTableModel(parent) { Refresh(); }

    // Re-populate if the number of registered classes has changed
    void Refresh() {
        if (Entries.size() != cmnClassRegister::size()) {
            beginResetModel();
            Entries.clear();
            for (auto it = cmnClassRegister::begin(); it != cmnClassRegister::end(); ++it) {
                Entry e; e.Name = it->first; e.Services = it->second;
                Entries.push_back(e);
            }
            endResetModel();
        }
    }

    int rowCount   (const QModelIndex & CMN_UNUSED(parent) = QModelIndex()) const override { return (int)Entries.size(); }
    int columnCount(const QModelIndex & CMN_UNUSED(parent) = QModelIndex()) const override { return 3; }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
            switch (section) {
                case 0: return QString("Class");
                case 1: return QString("Init mask");
                case 2: return QString("Run mask");
            }
        }
        return QAbstractTableModel::headerData(section, orientation, role);
    }

    QVariant data(const QModelIndex & index, int role) const override {
        if (!index.isValid() || index.row() >= (int)Entries.size()) return QVariant();
        const Entry & e = Entries[index.row()];
        const cmnLogMask full = e.Services->GetLogMask();

        if (role == Qt::DisplayRole) {
            switch (index.column()) {
                case 0: return QString(e.Name.c_str());
                case 1: return maskNibbleName(initNibble(full), cmnLoggerQtInitMaskEntries);
                case 2: return maskNibbleName(runNibble(full),  cmnLoggerQtRunMaskEntries);
            }
        }
        if (role == Qt::EditRole) {
            switch (index.column()) {
                case 0: return QString(e.Name.c_str());
                case 1: return QVariant((unsigned int)initNibble(full));
                case 2: return QVariant((unsigned int)runNibble(full));
            }
        }
        if (role == Qt::TextAlignmentRole) return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        if (role == Qt::ToolTipRole) {
            switch (index.column()) {
                case 0: return QString("Registered class name");
                case 1: return QString("Init log mask — double-click to change");
                case 2: return QString("Run log mask — double-click to change");
            }
        }
        return QVariant();
    }

    Qt::ItemFlags flags(const QModelIndex & index) const override {
        if (!index.isValid()) return Qt::NoItemFlags;
        Qt::ItemFlags f = QAbstractTableModel::flags(index);
        if (index.column() == 1 || index.column() == 2) f |= Qt::ItemIsEditable;
        return f;
    }

    bool setData(const QModelIndex & index, const QVariant & value, int role) override {
        if (!index.isValid() || (role != Qt::EditRole && role != Qt::DisplayRole)) return false;
        const Entry & e = Entries[index.row()];
        const cmnLogMask full = e.Services->GetLogMask();
        cmnLogMask newFull = full;
        if (index.column() == 1)       // update init nibble only
            newFull = (cmnLogMask)((full & 0xF0) | (value.toUInt() & 0x0F));
        else if (index.column() == 2)  // update run nibble only
            newFull = (cmnLogMask)((full & 0x0F) | (value.toUInt() & 0xF0));
        else
            return false;
        cmnClassRegister::SetLogMaskClass(e.Name, newFull);
        emit dataChanged(index, index);
        return true;
    }

    // Notify the view that all mask columns may have changed (e.g. after "Apply to all")
    void NotifyAllDataChanged() {
        if (!Entries.empty())
            emit dataChanged(this->index(0, 1), this->index((int)Entries.size() - 1, 2));
    }
};

// -----------------------------------------------------------------------
// cmnLoggerQtWidgetLoDDelegate
// Provides an inline QComboBox editor for the Init and Run mask columns.
// -----------------------------------------------------------------------
class cmnLoggerQtWidgetLoDDelegate : public QItemDelegate
{
public:
    cmnLoggerQtWidgetLoDDelegate(QObject * parent = nullptr): QItemDelegate(parent) {}

    QWidget * createEditor(QWidget * parent,
                           const QStyleOptionViewItem & CMN_UNUSED(option),
                           const QModelIndex & index) const override {
        QComboBox * cb = new QComboBox(parent);
        // column 1 = init presets, column 2 = run presets
        populateCombo(cb, (index.column() == 1) ? cmnLoggerQtInitMaskEntries
                                                : cmnLoggerQtRunMaskEntries);
        return cb;
    }
    void setEditorData(QWidget * editor, const QModelIndex & index) const override {
        QComboBox * cb = static_cast<QComboBox *>(editor);
        selectComboByMask(cb, (cmnLogMask)index.model()->data(index, Qt::EditRole).toUInt());
    }
    void setModelData(QWidget * editor, QAbstractItemModel * model,
                      const QModelIndex & index) const override {
        QComboBox * cb = static_cast<QComboBox *>(editor);
        model->setData(index, cb->itemData(cb->currentIndex()).toUInt(), Qt::EditRole);
    }
    void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option,
                              const QModelIndex & CMN_UNUSED(index)) const override {
        editor->setGeometry(option.rect);
    }
};

// -----------------------------------------------------------------------
// Color coding for log lines
// -----------------------------------------------------------------------
static QColor colorForLogLine(const QString & line) {
    if (line.startsWith("E-") || line.startsWith("Error"))   return QColor(0xFF, 0x77, 0x77); // red
    if (line.startsWith("W-") || line.startsWith("Warning")) return QColor(0xFF, 0xCC, 0x44); // amber
    if (line.startsWith("D-") || line.startsWith("Debug"))   return QColor(0x88, 0xFF, 0x88); // green
    return QColor(); // invalid = inherit default text colour
}

// -----------------------------------------------------------------------
// cmnLoggerQtWidget — constructor
// -----------------------------------------------------------------------
cmnLoggerQtWidget::cmnLoggerQtWidget(QWidget * parent)
    : Emitter(nullptr)
    , Streambuf(nullptr)
    , LogStream(nullptr)
{
    // ---- Top-level widget/layout ----
    this->Widget = new QWidget(parent);

    this->Emitter = new cmnLoggerQtStreambufEmitter(this->Widget);
    this->Streambuf = new cmnLoggerQtStreambuf(this->Emitter);
    this->LogStream = new std::ostream(this->Streambuf);
    cmnLogger::AddChannel(*(this->LogStream));

    this->Layout = new QVBoxLayout(this->Widget);
    this->Layout->setContentsMargins(0, 0, 0, 0);
    this->Layout->setSpacing(0);

    QTabWidget * tabs = new QTabWidget(this->Widget);
    this->Layout->addWidget(tabs);

    // ================================================================
    // TAB 1 — "Logs"
    // ================================================================
    {
        QWidget * logsTab = new QWidget();
        QVBoxLayout * logsLayout = new QVBoxLayout(logsTab);
        logsLayout->setContentsMargins(5, 5, 5, 5);
        logsLayout->setSpacing(4);

        // Toolbar
        QHBoxLayout * toolbarLayout = new QHBoxLayout();
        logsLayout->addLayout(toolbarLayout);

        toolbarLayout->addWidget(new QLabel("Max lines:"));
        this->MaxLinesDisplay = new QComboBox(logsTab);
        this->MaxLinesDisplay->addItem("100",  100);
        this->MaxLinesDisplay->addItem("500",  500);
        this->MaxLinesDisplay->addItem("1000", 1000);
        this->MaxLinesDisplay->addItem("5000", 5000);
        this->MaxLinesDisplay->setCurrentIndex(1); // 500 default
        toolbarLayout->addWidget(this->MaxLinesDisplay);

        QCheckBox * autoScrollCheck = new QCheckBox("Auto-scroll", logsTab);
        autoScrollCheck->setChecked(true);
        toolbarLayout->addWidget(autoScrollCheck);
        toolbarLayout->addStretch();

        QPushButton * clearBtn = new QPushButton("Clear", logsTab);
        toolbarLayout->addWidget(clearBtn);

        // Log display
        this->LogDisplay = new QPlainTextEdit(logsTab);
        this->LogDisplay->setReadOnly(true);
        this->LogDisplay->setMaximumBlockCount(500);
        this->LogDisplay->setLineWrapMode(QPlainTextEdit::NoWrap);
        QFont mono("Monospace");
        mono.setStyleHint(QFont::Monospace);
        mono.setPointSize(9);
        this->LogDisplay->setFont(mono);
        logsLayout->addWidget(this->LogDisplay);

        tabs->addTab(logsTab, "Logs");

        // Connections (Logs tab)
        QObject::connect(clearBtn, &QPushButton::clicked, this->LogDisplay, &QPlainTextEdit::clear);

        QObject::connect(this->MaxLinesDisplay, QOverload<int>::of(&QComboBox::currentIndexChanged),
                         [=](int idx) {
                             if (idx != -1)
                                 this->LogDisplay->setMaximumBlockCount(
                                     this->MaxLinesDisplay->itemData(idx).toInt());
                         });

        // Opt 1: LineReady just queues the line on the GUI thread.
        // Opt 3: A 50 ms flush timer does one batch insert per tick.
        QObject::connect(this->Emitter, &cmnLoggerQtStreambufEmitter::LineReady,
                         this->Widget,
                         [=](const QString & text) {
                             this->PendingLines.append(text);
                         },
                         Qt::QueuedConnection);

        // 50 ms flush timer: drain PendingLines in one beginEditBlock transaction
        QTimer * flushTimer = new QTimer(this->Widget);
        flushTimer->setInterval(50);
        QObject::connect(flushTimer, &QTimer::timeout, [=]() {
            if (this->PendingLines.isEmpty()) return;
            QStringList batch;
            batch.swap(this->PendingLines); // grab all pending, clear atomically on GUI thread
            QTextCursor cursor = this->LogDisplay->textCursor();
            cursor.movePosition(QTextCursor::End);
            cursor.beginEditBlock();  // Opt 3: one layout/repaint for the whole batch
            const QString timestamp = QTime::currentTime().toString("HH:mm:ss.zzz ");
            QTextCharFormat fmtGrey;
            fmtGrey.setForeground(QColor(0x80, 0x80, 0x80));
            for (const QString & text : batch) {
                cursor.setCharFormat(fmtGrey);
                cursor.insertText(timestamp);
                QTextCharFormat fmtMsg;
                QColor col = colorForLogLine(text);
                fmtMsg.setForeground(col.isValid() ? col
                                                   : this->LogDisplay->palette().color(QPalette::Text));
                cursor.setCharFormat(fmtMsg);
                cursor.insertText(text);
            }
            cursor.endEditBlock();
            if (autoScrollCheck->isChecked())
                this->LogDisplay->verticalScrollBar()->setValue(
                    this->LogDisplay->verticalScrollBar()->maximum());
        });
        flushTimer->start();
    }

    // ================================================================
    // TAB 2 — "Settings"
    // ================================================================
    {
        QWidget * settingsTab = new QWidget();
        QVBoxLayout * settingsLayout = new QVBoxLayout(settingsTab);
        settingsLayout->setContentsMargins(5, 5, 5, 5);
        settingsLayout->setSpacing(6);

        // ---- Global masks ----
        this->MainFilterWidget = new QWidget(settingsTab);
        settingsLayout->addWidget(this->MainFilterWidget);
        this->MainFilterLayout = new QGridLayout(this->MainFilterWidget);
        this->MainFilterLayout->setContentsMargins(0, 0, 0, 0);

        // Row 0: Overall mask
        this->OverallFilterLabel = new QLabel("Overall mask:", this->MainFilterWidget);
        this->MainFilterLayout->addWidget(this->OverallFilterLabel, 0, 0);
        this->MainFilterLayout->addWidget(new QLabel("Init:", this->MainFilterWidget), 0, 1);
        this->OverallFilterData = new QComboBox(this->MainFilterWidget);
        populateCombo(this->OverallFilterData, cmnLoggerQtInitMaskEntries);
        this->MainFilterLayout->addWidget(this->OverallFilterData, 0, 2);
        this->MainFilterLayout->addWidget(new QLabel("Run:", this->MainFilterWidget), 0, 3);
        this->OverallRunData = new QComboBox(this->MainFilterWidget);
        populateCombo(this->OverallRunData, cmnLoggerQtRunMaskEntries);
        this->MainFilterLayout->addWidget(this->OverallRunData, 0, 4);
        this->MainFilterLayout->setColumnStretch(5, 1);

        // Row 1: Function mask
        this->FunctionFilterLabel = new QLabel("Function mask:", this->MainFilterWidget);
        this->MainFilterLayout->addWidget(this->FunctionFilterLabel, 1, 0);
        this->MainFilterLayout->addWidget(new QLabel("Init:", this->MainFilterWidget), 1, 1);
        this->FunctionFilterData = new QComboBox(this->MainFilterWidget);
        populateCombo(this->FunctionFilterData, cmnLoggerQtInitMaskEntries);
        this->MainFilterLayout->addWidget(this->FunctionFilterData, 1, 2);
        this->MainFilterLayout->addWidget(new QLabel("Run:", this->MainFilterWidget), 1, 3);
        this->FunctionRunData = new QComboBox(this->MainFilterWidget);
        populateCombo(this->FunctionRunData, cmnLoggerQtRunMaskEntries);
        this->MainFilterLayout->addWidget(this->FunctionRunData, 1, 4);

        // ---- Set all classes to ----
        QHBoxLayout * setAllLayout = new QHBoxLayout();
        settingsLayout->addLayout(setAllLayout);
        setAllLayout->addWidget(new QLabel("Set all classes to:"));
        setAllLayout->addWidget(new QLabel("Init:"));
        QComboBox * setAllInitCombo = new QComboBox(settingsTab);
        populateCombo(setAllInitCombo, cmnLoggerQtInitMaskEntries);
        setAllLayout->addWidget(setAllInitCombo);
        setAllLayout->addWidget(new QLabel("Run:"));
        QComboBox * setAllRunCombo = new QComboBox(settingsTab);
        populateCombo(setAllRunCombo, cmnLoggerQtRunMaskEntries);
        setAllLayout->addWidget(setAllRunCombo);
        QPushButton * setAllBtn = new QPushButton("Apply to all", settingsTab);
        setAllLayout->addWidget(setAllBtn);
        setAllLayout->addStretch();

        // ---- Class name search ----
        QHBoxLayout * searchLayout = new QHBoxLayout();
        settingsLayout->addLayout(searchLayout);
        searchLayout->addWidget(new QLabel("Search class:"));
        this->ClassNameFilterLineEdit = new QLineEdit(settingsTab);
        this->ClassNameFilterLineEdit->setPlaceholderText("Filter classes by name...");
        this->ClassNameFilterLineEdit->setClearButtonEnabled(true);
        searchLayout->addWidget(this->ClassNameFilterLineEdit);

        // ---- Class table ----
        this->View = new QTableView(settingsTab);
        settingsLayout->addWidget(this->View);
        this->Model = new cmnLoggerQtWidgetClassServicesModel(this->View);
        this->ProxyModel = new QSortFilterProxyModel(this->View);
        this->ProxyModel->setSourceModel(this->Model);
        this->ProxyModel->setFilterKeyColumn(0);
        this->ProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
        this->Delegate = new cmnLoggerQtWidgetLoDDelegate(this->View);
        this->View->verticalHeader()->hide();
        this->View->setModel(this->ProxyModel);
        this->View->setItemDelegate(this->Delegate);
        this->View->horizontalHeader()->setStretchLastSection(true);
        this->View->setSelectionBehavior(QAbstractItemView::SelectRows);
        this->View->setSelectionMode(QAbstractItemView::SingleSelection);
        this->View->setSortingEnabled(true);
        this->View->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);

        tabs->addTab(settingsTab, "Settings");

        // ---- Initialise combos from current logger state ----
        const cmnLogMask currentOverall  = cmnLogger::GetMask();
        const cmnLogMask currentFunction = cmnLogger::GetMaskFunction();
        selectComboByMask(this->OverallFilterData,  initNibble(currentOverall));
        selectComboByMask(this->OverallRunData,      runNibble(currentOverall));
        selectComboByMask(this->FunctionFilterData,  initNibble(currentFunction));
        selectComboByMask(this->FunctionRunData,     runNibble(currentFunction));

        // ---- Connections (Settings tab) ----
        QObject::connect(this->ClassNameFilterLineEdit, &QLineEdit::textChanged,
                         this->ProxyModel, &QSortFilterProxyModel::setFilterFixedString);

        auto updateOverall = [=]() {
            cmnLogger::SetMask((cmnLogMask)(this->OverallFilterData->currentData().toUInt()
                                            | this->OverallRunData->currentData().toUInt()));
        };
        QObject::connect(this->OverallFilterData, QOverload<int>::of(&QComboBox::currentIndexChanged),
                         [=](int){ updateOverall(); });
        QObject::connect(this->OverallRunData, QOverload<int>::of(&QComboBox::currentIndexChanged),
                         [=](int){ updateOverall(); });

        auto updateFunction = [=]() {
            cmnLogger::SetMaskFunction((cmnLogMask)(this->FunctionFilterData->currentData().toUInt()
                                                     | this->FunctionRunData->currentData().toUInt()));
        };
        QObject::connect(this->FunctionFilterData, QOverload<int>::of(&QComboBox::currentIndexChanged),
                         [=](int){ updateFunction(); });
        QObject::connect(this->FunctionRunData, QOverload<int>::of(&QComboBox::currentIndexChanged),
                         [=](int){ updateFunction(); });

        QObject::connect(setAllBtn, &QPushButton::clicked, [=]() {
            cmnLogMask mask = (cmnLogMask)(setAllInitCombo->currentData().toUInt()
                                           | setAllRunCombo->currentData().toUInt());
            cmnLogger::SetMaskClassAll(mask);
            this->Model->NotifyAllDataChanged();
        });
    }

    // ================================================================
    // Timer — refresh class list when new classes are registered
    // ================================================================
    this->Timer = new QTimer(this->Widget);
    this->Timer->setInterval(1000);
    QObject::connect(this->Timer, &QTimer::timeout, [=]() { this->Model->Refresh(); });
    this->Timer->start();

    this->View->resizeColumnsToContents();
}

// -----------------------------------------------------------------------
// cmnLoggerQtWidget — destructor
// Remove the log channel before destroying the streambuf.
// -----------------------------------------------------------------------
cmnLoggerQtWidget::~cmnLoggerQtWidget()
{
    if (this->LogStream) {
        cmnLogger::RemoveChannel(*(this->LogStream));
        delete this->LogStream;
        this->LogStream = nullptr;
    }
    delete this->Streambuf;
    this->Streambuf = nullptr;
}

#include "cmnLoggerQtWidget.moc"

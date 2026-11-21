#include <cisstVectorQt/vctQtWidgetDynamicVector.h>
#include <cisstVectorQt/vctForwardDeclarationsQt.h>
#include <QApplication>
#include <QCheckBox>
#include <iostream>
#include <stdexcept>

static void Check(bool condition, const char * message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

template <class T, class ReadWidget, class WriteWidget>
static void TestNumeric()
{
    const std::vector<T> values{T(2), T(4)};
    ReadWidget read;
    Check(read.SetValue(values) && read.columnCount() == 2, "read STL vector size");
    Check(read.item(0, 1)->text().toDouble() == 4.0, "read STL vector value");
    Eigen::VectorX<T> eigen(2);
    eigen << T(3), T(5);
    Check(read.SetValue(eigen) && read.item(0, 1)->text().toDouble() == 5.0,
          "read Eigen compatibility");
    Check(read.SetValue(std::vector<T>{}) && read.columnCount() == 0, "read empty STL vector");

    for (const auto mode : {vctQtWidgetDynamicVectorWriteBase::TEXT_WIDGET,
                            vctQtWidgetDynamicVectorWriteBase::SPINBOX_WIDGET,
                            vctQtWidgetDynamicVectorWriteBase::SLIDER_WIDGET}) {
        WriteWidget write(mode);
        write.SetRange(T(0), T(100));
        int signalCount = 0;
        QObject::connect(&write, &vctQtWidgetDynamicVectorWriteBase::valueChanged,
                         [&signalCount]() { ++signalCount; });
        Check(write.SetValue(values) && signalCount == 0, "STL setter blocks signals by default");
        std::vector<T> restored(2);
        Check(write.GetValue(restored) && restored == values, "STL editor round trip");
        Check(write.GetValue(eigen) && eigen[0] == T(2) && eigen[1] == T(4),
              "STL editor to Eigen output");
        eigen << T(3), T(5);
        Check(write.SetValue(eigen) && write.GetValue(restored)
              && restored == std::vector<T>({T(3), T(5)}), "Eigen editor to STL output");
        std::vector<T> wrongSize(1);
        Check(!write.GetValue(wrongSize), "reject incorrectly sized STL output");
        write.blockSignals(true);
        write.SetValue(values);
        Check(write.signalsBlocked(), "preserve signal blocking state");
        write.blockSignals(false);
        Check(write.SetValue(std::vector<T>{}) && write.columnCount() == 0,
              "empty STL editor");
        restored.clear();
        Check(write.GetValue(restored), "read empty STL editor");
    }
}

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);
    try {
        TestNumeric<double, vctQtWidgetDynamicVectorDoubleRead, vctQtWidgetDynamicVectorDoubleWrite>();
        TestNumeric<float, vctQtWidgetDynamicVectorFloatRead, vctQtWidgetDynamicVectorFloatWrite>();
        TestNumeric<int, vctQtWidgetDynamicVectorIntRead, vctQtWidgetDynamicVectorIntWrite>();
        TestNumeric<unsigned int, vctQtWidgetDynamicVectorUIntRead, vctQtWidgetDynamicVectorUIntWrite>();

        vctQtWidgetDynamicVectorBoolRead read;
        const std::vector<bool> values{true, false, true};
        Check(read.SetValue(values) && read.columnCount() == 3
              && read.item(0, 1)->text() == "0", "read packed STL bool vector");
        vctQtWidgetDynamicVectorBoolWrite write;
        Check(write.SetValue(values), "write packed STL bool vector");
        std::vector<bool> restored(3);
        Check(write.GetValue(restored) && restored == values, "packed STL bool round trip");
        auto * box = qobject_cast<QCheckBox *>(write.cellWidget(0, 1));
        Check(box != nullptr, "bool checkbox exists");
        box->setChecked(true);
        Check(write.GetValue(restored) && restored == std::vector<bool>({true, true, true}),
              "checkbox edits reach packed STL output");
        Eigen::VectorX<bool> eigen(3);
        Check(write.GetValue(eigen) && eigen.all(), "bool STL to Eigen output");
        eigen.setConstant(false);
        Check(write.SetValue(eigen) && write.GetValue(restored)
              && restored == std::vector<bool>({false, false, false}), "bool Eigen to STL output");
        restored.resize(2);
        Check(!write.GetValue(restored), "reject incorrectly sized bool output");
        Check(write.SetValue(std::vector<bool>{}) && read.SetValue(std::vector<bool>{}),
              "empty bool vectors");
        restored.clear();
        Check(write.GetValue(restored) && write.columnCount() == 0, "read empty bool editor");

        vctQtWidgetDynamicVectorDoubleWrite slider(vctQtWidgetDynamicVectorWriteBase::SLIDER_WIDGET);
        slider.SetRange(std::vector<double>{-10.0, 10.0}, std::vector<double>{10.0, 30.0});
        const std::vector<double> sample{0.0, 15.0};
        std::vector<double> output(2);
        Check(slider.SetValue(sample) && slider.GetValue(output) && output == sample,
              "STL per-element ranges");
    }
    catch (const std::exception & error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}

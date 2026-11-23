#include <cisstCommon/cmnLogger.h>

#ifdef TEST_QT
#include <QApplication>
#include <QWidget>
#include <EigenQt/VectorView>
#include <array>
#endif

int main(int argc, char ** argv)
{
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
#ifdef TEST_QT
    QApplication application(argc, argv);
    QWidget widget;
    EigenQt::VectorView values;
    values.setValue(std::array<double, 2>{1.0, 2.0});
#else
    (void)argc;
    (void)argv;
#endif
    return 0;
}

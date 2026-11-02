#include <cisstCommon/cmnLogger.h>

#ifdef TEST_QT
#include <QApplication>
#include <QWidget>
#endif

int main(int argc, char ** argv)
{
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
#ifdef TEST_QT
    QApplication application(argc, argv);
    QWidget widget;
#else
    (void)argc;
    (void)argv;
#endif
    return 0;
}

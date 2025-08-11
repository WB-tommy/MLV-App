/*!
 * \file main.cpp
 * \author masc4ii
 * \copyright 2017
 * \brief The main... the start of the horror
 */

#include "MainWindow.h"
#include "MyApplication.h"
#ifdef Q_OS_ANDROID
#include "AOS/Android.h"
#include <QCoreApplication>
#include <QGuiApplication>
#endif

int main(int argc, char *argv[])
{
#ifdef Q_OS_ANDROID
    // High-DPI: use exact fractional scale factors to align input and paint
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    // Synthesize mouse from touch for legacy mouse-based handlers
    // Avoid also synthesizing touch from mouse to prevent duplicate/conflicting events
    QCoreApplication::setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, true);
#endif

    // Enable HiDPI handling before QApplication creation for consistent scaling
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    MyApplication a(argc, argv);
#ifdef Q_OS_WIN
    a.setAttribute(Qt::AA_Use96Dpi);
#endif
    MainWindow w(argc, argv);
    w.show();

#ifdef Q_OS_ANDROID
    // Enter immersive fullscreen and re-apply whenever app regains focus
    enableImmersiveMode();
    QObject::connect(&a, &QGuiApplication::applicationStateChanged, &w, [](Qt::ApplicationState state){
        if (state == Qt::ApplicationActive) {
            enableImmersiveMode();
        }
    });
    QObject::connect(&a, &QGuiApplication::focusWindowChanged, &w, [](QWindow*){
        enableImmersiveMode();
    });
#endif

    return a.exec();
}

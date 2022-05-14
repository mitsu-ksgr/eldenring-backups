#include "MainWindow.h"

#include <QApplication>
#include <QFont>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    // Set locales
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    // Init app
    QApplication app(argc, argv);

    // Font
    QFont font("Meiryo");
    font.setStyleHint(QFont::Helvetica);
    app.setFont(font);

    // Window
    MainWindow window;
    window.show();

    return app.exec();
}

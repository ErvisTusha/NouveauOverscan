#include "mainwindow.h"
#include <QApplication>
#include <QProcess>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QProcess process;
    process.start("which xrandr");
    process.waitForFinished();
    QString output;
    output =  process.readAllStandardOutput();
//    if (output==""){
//        QMessageBox msgBox;
//        msgBox.setIcon(QMessageBox::Critical);
//        msgBox.setText("Please install XRandR");
//        msgBox.setInformativeText("<a href=\"http://www.x.org/wiki/Projects/XRandR/\">http://www.x.org/wiki/Projects/XRandR/</a>");
//        msgBox.setStandardButtons(QMessageBox::Close);
//        msgBox.exec();
//        return 0 ;
//    }

    MainWindow w;
    w.show();

    return a.exec();
}

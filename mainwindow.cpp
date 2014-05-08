#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>
#include <QStringList>
#include <QPixmap>
#include <QScreen>
#include <QFile>
#include <QDebug>
#include <QDesktopServices>
#include <QTextStream>
#include <QDir>
//TODO comment code
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setAttribute( Qt::WA_TranslucentBackground, true );
    this->showFullScreen();
    this->setWindowFlags ( Qt::CustomizeWindowHint | Qt::WindowFullscreenButtonHint | Qt::WindowStaysOnTopHint);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_close_clicked()
{
    this->close();
}

void MainWindow::on_apply_clicked()
{
    QString username;
    QString ASpath ;
    QProcess process;
    QStringList varop;

    QString ASContent;
    QString ASpathSH;
    QString ASContentSH;

    varop << "-c" << "whoami"  ;
    process.start("/bin/sh", varop , QIODevice::ReadOnly);
    process.waitForFinished(-1);


    username= process.readAllStandardOutput();



    ASpath= "/home/";
    ASpath +=username.simplified();
    ASpath +="/.config/autostart/nouveauoverscan.desktop";

    QFile autostart(ASpath);

    ASpathSH= "/home/";
    ASpathSH +=username.simplified();
    ASpathSH +="/.config/nouveauoverscan/nouveauoverscan.sh";


    QString ASpathSHp;
    ASpathSHp= "/home/";
    ASpathSHp +=username.simplified();
    ASpathSHp +="/.config/nouveauoverscan/";

    QFile autostartSH(ASpathSH);


    if(ui->checkBox->isChecked()){
        if(autostart.exists()){
            autostart.remove();
            autostartSH.remove();
        }

        ASContent = "[Desktop Entry]\n";
        ASContent += "Type=Application\n";
        ASContent += "Exec=/home/";
        ASContent += username.simplified();
        ASContent += "/.config/nouveauoverscan/nouveauoverscan.sh";
        ASContent += "\n";
        ASContent += "Hidden=false\n";
        ASContent += "NoDisplay=false\n";
        ASContent += "X-GNOME-Autostart-enabled=true\n";
        ASContent += "Name[en_US]=Nouveau OverScan Fix\n";
        ASContent += "Name=Nouveau OverScan Fix\n";
        ASContent += "Comment[en_US]=Nouveau OverScan Fix\n";
        ASContent += "Comment=Nouveau OverScan Fix\n";

        autostart.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&autostart);
        out << ASContent;
        autostart.close();

        if(!QDir(ASpathSHp).exists()){
            QDir().mkdir(ASpathSHp);
        }

        ASContentSH= "#!/bin/bash \n";
        ASContentSH+= "/usr/bin/xrandr --output ";
        ASContentSH+= QGuiApplication::primaryScreen()->name();
        ASContentSH+= " --set underscan on & /usr/bin/xrandr --output HDMI-1 --set \"underscan hborder\" ";
        ASContentSH+= QString::number( ui->horizontalSlider->value());

        ASContentSH+= "\n";

        autostartSH.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream outSH(&autostartSH);
        outSH << ASContentSH;
        autostartSH.close();

        varop.clear();
        varop << "+x" << ASpathSH  ;
        process.execute("chmod", varop);
        process.waitForFinished(-1);


    }else{
        if(autostart.exists()){
            autostart.remove();
            autostartSH.remove();
        }
    }

}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    this->fixoverscan(position);
}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    this->fixoverscan(arg1);
}

void MainWindow::fixoverscan(int arg1){
    QProcess process;
    QStringList varop;
    QStringList varopl;
    QString  varops;
    varopl << "/usr/bin/xrandr --output "<<QGuiApplication::primaryScreen()->name() << " --set underscan on &  /usr/bin/xrandr --output HDMI-1 --set \"underscan hborder\" " << QString::number(arg1);
    varops = varopl.join(' ');
    varop << "-c" << varops  ;
    process.start("/bin/sh", varop , QIODevice::ReadOnly);
    process.waitForFinished(-1);
}


void MainWindow::on_checkBox_stateChanged(int arg1)
{
    // 2 selected 0 unselected

    if(arg1==2){
        ui->horizontalSlider->setEnabled(true);
        ui->spinBox->setEnabled(true);
    }else{
        ui->spinBox->setEnabled(false);
        ui->horizontalSlider->setEnabled(false);
    }
}

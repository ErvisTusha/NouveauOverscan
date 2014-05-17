#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>
#include <QStringList>
#include <QScreen>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QStandardPaths>

#include <QDebug>
#include <QObject>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QSlider>
#include <QCheckBox>
#include <QSpacerItem>
#include <QPushButton>
//TODO comment code
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setAttribute( Qt::WA_TranslucentBackground, true );
    this->showFullScreen();
    this->setWindowFlags ( Qt::CustomizeWindowHint | Qt::WindowFullscreenButtonHint | Qt::WindowStaysOnTopHint);
    QString outputs;
    QString ScreenName;
    foreach (QScreen *screen, QGuiApplication::screens()) {
        ScreenName = screen->name();
        if(!ScreenName.contains(QRegExp("VGA"))){
            //checkbox
            outputs = "Checkbox";
            outputs +=  ScreenName;
            outputs.replace("-","_");
            QCheckBox *CheckBox = new QCheckBox();
            CheckBox->setObjectName(outputs);
            CheckBox->setText("OverScan " + ScreenName);
            CheckBox->setStyleSheet("QCheckBox {background : white;  text-align: center;font-size:20px;font-weight:bold;}");
            connect (CheckBox,SIGNAL(clicked()),this,SLOT(checkboxchecked()));
            //SpinBox
            outputs = "Spinbox";
            outputs +=  ScreenName;
            outputs.replace("-","_");
            QSpinBox *SpinBox = new QSpinBox();
            SpinBox->setObjectName(outputs);
            SpinBox->setEnabled(false);
            //slider
            outputs = "Slider";
            outputs +=  ScreenName;
            outputs.replace("-","_");
            QSlider *Slider = new QSlider(Qt::Horizontal,this);
            Slider->setObjectName(outputs);
            Slider->setEnabled(false);
            //Layouts
            QHBoxLayout *hlayout = new QHBoxLayout;
            QVBoxLayout *vlayout = new QVBoxLayout;
            QSpacerItem *Spacer = new QSpacerItem(10,10,QSizePolicy::Minimum,QSizePolicy::Maximum);
            hlayout->addWidget(Slider);
            hlayout->addWidget(SpinBox);
            vlayout->addWidget(CheckBox);
            vlayout->insertLayout(1,hlayout);
            vlayout->addSpacerItem(Spacer);
            ui->verticalLayout->insertLayout(0,vlayout);
            connect(SpinBox,SIGNAL(valueChanged(int)),Slider,SLOT(setValue(int)));
            connect(Slider,SIGNAL(valueChanged(int)),SpinBox,SLOT(setValue(int)));
            connect(Slider,SIGNAL(valueChanged(int)),this,SLOT(fixoverscan(int)));
        }
    }
    //Create Apply and Close buttons
    QPushButton *Apply = new QPushButton("Apply");
    QPushButton *Close = new QPushButton ("Close");
    QHBoxLayout *hlbuttons = new QHBoxLayout;
    hlbuttons->addWidget(Apply);
    hlbuttons->addWidget(Close);
    ui->verticalLayout->insertLayout(20,hlbuttons);
    //create signal for Close and Apply buttons
    connect(Close,SIGNAL(clicked()),qApp,SLOT(quit()));
    connect(Apply,SIGNAL(clicked()),this,SLOT(apply()));

}
MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::fixoverscan(int arg1){
    QProcess process;
    QStringList varop;
    QStringList varopl;
    QString  varops;
    QObject* obj = sender();
    QString output ;
    output = obj->objectName();
    output.replace("Slider","");
    output.replace("_","-");
    varopl << "xrandr --output "<< output << " --set underscan on &  xrandr --output" << output << " --set \"underscan hborder\" " << QString::number(arg1);
    varops = varopl.join(' ');
    varop << "-c" << varops  ;
    process.start("/bin/sh", varop , QIODevice::ReadOnly);
    process.waitForFinished(-1);
}


void MainWindow::apply(){
    QString SliderName;
    QString CheckboxName;
    QString ASContentSH;
    QString ASpath ;
    QString ASpathSHp;
    QString ASContent;
    QString ASpathSH;
    QString ScreenName;
    QString homeLocation  = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QStringList varop;
    QProcess process;



    ASpath= homeLocation;
    ASpath +="/.config/autostart/nouveauoverscan.desktop";
    QFile autostart(ASpath);

    ASpathSH= homeLocation;
    ASpathSH +="/.config/nouveauoverscan/nouveauoverscan.sh";

    ASpathSHp= homeLocation;
    ASpathSHp +="/.config/nouveauoverscan/";
    QFile autostartSH(ASpathSH);

    foreach (QScreen *screen, QGuiApplication::screens()) {
        ScreenName = screen->name();
        if(!ScreenName.contains(QRegExp("VGA"))){
            CheckboxName="Checkbox";
            CheckboxName+=ScreenName;
            CheckboxName.replace("-","_");
            QCheckBox *CheckBox = this->findChild<QCheckBox *>(CheckboxName);
            if(CheckBox->isChecked()){
                SliderName="Slider";
                SliderName+=ScreenName;
                SliderName.replace("-","_");
                QSlider *Slider = this->findChild<QSlider *>(SliderName);
                ASContentSH+= "xrandr --output ";
                ASContentSH+= ScreenName;
                ASContentSH+= " --set underscan on & xrandr --output ";
                ASContentSH+=ScreenName;
                ASContentSH+=" --set \"underscan hborder\" ";
                ASContentSH+= QString::number(Slider->value());
                ASContentSH+= "\n";
            }
        }
    }

    if(!ASContentSH.isEmpty()){
        ASContentSH.prepend("#!/bin/bash \n");
        if(autostart.exists()){
            autostart.remove();
            autostartSH.remove();
        }

        ASContent = "[Desktop Entry]\n";
        ASContent += "Type=Application\n";
        ASContent += "Exec=";
        ASContent += homeLocation;
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
        autostartSH.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream outSH(&autostartSH);
        outSH << ASContentSH;
        autostartSH.close();
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





void MainWindow::checkboxchecked(){
    QString outputs;
    QObject* obj = sender();
    outputs = obj->objectName();
    QCheckBox *Checkbox = this->findChild<QCheckBox *>(outputs);
    if(Checkbox->isChecked()){
        outputs.replace("Checkbox","Slider");
        QSlider *Slider = this->findChild<QSlider *>(outputs);
        Slider->setEnabled(true);
        outputs.replace("Slider","Spinbox");
        QSpinBox *Spinbox = this->findChild<QSpinBox *>(outputs);
        Spinbox->setEnabled(true);
    }else{
        outputs.replace("Checkbox","Slider");
        QSlider *Slider = this->findChild<QSlider *>(outputs);
        Slider->setEnabled(false);
        outputs.replace("Slider","Spinbox");
        QSpinBox *Spinbox = this->findChild<QSpinBox *>(outputs);
        Spinbox->setEnabled(false);
    }
}

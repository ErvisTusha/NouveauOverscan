#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_close_clicked();

    void on_apply_clicked();

    void on_horizontalSlider_sliderMoved(int position);

    void fixoverscan(int arg1);

    void on_spinBox_valueChanged(int arg1);

    void on_checkBox_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

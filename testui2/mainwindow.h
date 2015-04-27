#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "freqampdialog.h"
#include "..\ResourceParameter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:
	void setAmps(float p_val1, float p_val2);

private slots:
    void on_pushButton_clicked();

   // void on_comboBox_activated(const QString &arg1);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void openNewWindow();

	

private:
    Ui::MainWindow *ui;   
    FreqAmpDialog *dialog;
	std::ResourceParameter *params;
};

#endif // MAINWINDOW_H

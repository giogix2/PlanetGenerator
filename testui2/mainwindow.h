#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "freqampdialog.h"
#include "meshdialog.h"
#include "../PSphere.h"
#include "../initOgre.h"

#include "../ResourceParameter.h"

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
    void setMeshes(QString p_path, int p_count);

private slots:
    void on_pushButton_clicked();   

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void openNewWindow();

	

    void on_pushButton_9_clicked();

private:
    Ui::MainWindow *ui;   
    FreqAmpDialog *dialog;
    MeshDialog *meshdialog;
	std::ResourceParameter *params;
    PSphere *mySphere;
    initOgre *rendering;
};

#endif // MAINWINDOW_H

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore\QDebug>
#include <QtWidgets\QcolorDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEdit->setValidator( new QIntValidator(0, 100, this) );
    ui->lineEdit_2->setValidator( new QIntValidator(0, 100, this) );   
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    list.clear();
    if(ui->lineEdit != NULL)
    {
        list.append(ui->lineEdit->text());
    }


 //   ui->comboBox->setStyleSheet("QWidget {background-color:QColor(rg)}");


    if(ui->lineEdit_2 != NULL)
    {
        QString text = ui->lineEdit_2->text();
        if(ui->pushButton_2->text() != "Color")
        {
            text += ","+ui->pushButton_2->text();
        }
        if(ui->pushButton_3->text() != "Color")
        {
            text += ","+ui->pushButton_3->text();
        }

        list.append(text);

    }

    QString text = "";
    if(ui->pushButton_4->text() != "Color")
    {
        text += ui->pushButton_4->text();
    }
    if(ui->pushButton_5->text() != "Color")
    {
        text += ","+ui->pushButton_5->text();
    }

    list.append(text);


    if(list.length() != 0)
    {
        auto it = list.end(), end = list.begin();
        while ( it != end ) {
            --it;
           qDebug() << "the list: "+*it;
        }
    }

}

//void MainWindow::on_comboBox_activated(const QString &arg1)
//{
  //  QPalette palette = ui->comboBox->palette();
   // QColor rg = palette.color(QPalette::)

  //  QString text = ui->comboBox
    //ui->comboBox->editable = true;

  /*  QString textToFind = "abc";
    int index = ui->comboBox->findText(textToFind);
    ui->comboBox->setItemText(index, "test");
*/
    //QLineEdit *le = ui->comboBox->lineEdit();
  //  le->setText("test");

 /*   QPalette pal = ui->comboBox->palette();
    pal.setColor(ui->comboBox->backgroundRole(), rg);
    ui->comboBox->setAutoFillBackground( true );
    ui->comboBox->setPalette(pal);

    ui->comboBox->setStyleSheet("QComboBox { background-color:QColor(rg)}");
   // ui->comboBox->setBackgroundRole(rg);

*/
//}

void MainWindow::on_pushButton_2_clicked()
{
    QColor rg = QColorDialog::getColor(Qt::white,this, "Text Color",  QColorDialog::DontUseNativeDialog);
    QString qss = QString("background-color: %1").arg(rg.name());
    ui->pushButton_2->setStyleSheet(qss);
    ui->pushButton_2->setText(""+rg.name());
    qDebug() << "Color chosen: "+rg.name();
}

void MainWindow::on_pushButton_3_clicked()
{
    QColor rg = QColorDialog::getColor(Qt::white,this, "Text Color",  QColorDialog::DontUseNativeDialog);
    QString qss = QString("background-color: %1").arg(rg.name());
    ui->pushButton_3->setStyleSheet(qss);
    ui->pushButton_3->setText(""+rg.name());
    qDebug() << "Color chosen: "+rg.name();
}

void MainWindow::on_pushButton_4_clicked()
{
    QColor rg = QColorDialog::getColor(Qt::white,this, "Text Color",  QColorDialog::DontUseNativeDialog);
    QString qss = QString("background-color: %1").arg(rg.name());
    ui->pushButton_4->setStyleSheet(qss);
    ui->pushButton_4->setText(""+rg.name());
    qDebug() << "Color chosen: "+rg.name();
}

void MainWindow::on_pushButton_5_clicked()
{
    QColor rg = QColorDialog::getColor(Qt::white,this, "Text Color",  QColorDialog::DontUseNativeDialog);
    QString qss = QString("background-color: %1").arg(rg.name());
    ui->pushButton_5->setStyleSheet(qss);
    ui->pushButton_5->setText(""+rg.name());
    qDebug() << "Color chosen: "+rg.name();
}

#include "freqampdialog.h"
#include "ui_freqampdialog.h"
#include "mainwindow.h"
#include <QDebug>

FreqAmpDialog::FreqAmpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FreqAmpDialog)
{
    ui->setupUi(this);
}

FreqAmpDialog::~FreqAmpDialog()
{
    delete ui;
}

void FreqAmpDialog::on_pushButton_clicked()
{
    QString freq = ui->lineEdit->text();//.toFloat();
    QString amp = ui->lineEdit_2->text();//.toFloat();


    ui->listWidget->addItem(""+freq+","+amp);

}

void FreqAmpDialog::on_buttonBox_accepted()
{
	//for(int i=0; i<ui->listWidget->count(); i++)
	//{
	//	//qDebug() << "" +ui->listWidget->item(i)->text();
	//	//connect(, SIGNAL(accepted()), this, SLOT(setAmps(ui->lineEdit->text().toFloat(), ui->lineEdit_2->text().toFloat())) );
	//	//MainWindow::setAmps(ui->lineEdit->text().toFloat(), ui->lineEdit_2->text().toFloat());		
	//}
}

QListWidget* FreqAmpDialog::getThem()
{
	return ui->listWidget;
}

void FreqAmpDialog::on_pushButton_2_clicked()
{
    qDeleteAll(ui->listWidget->selectedItems());
}

void FreqAmpDialog::instantiateList(std::vector< std::pair < float, float > > freqamps)
{
    for (std::vector<std::pair <float, float> >::const_iterator iter = freqamps.begin(); iter != freqamps.end(); ++iter)
    {
        QString freq = QString::number(iter->first);
        QString amp = QString::number(iter->second);
        ui->listWidget->addItem(""+freq+","+amp);
        //qDebug() << iter->first <<", " << iter->second;
    }
}

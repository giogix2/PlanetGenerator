#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "freqampdialog.h"
#include "../ResourceParameter.h"
#include <QDebug>
#include <QColorDialog>
#include <QVectorIterator>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEdit->setValidator( new QIntValidator(0, 100, this) );
    ui->lineEdit_2->setValidator( new QIntValidator(0, 100, this) );
	params = new std::ResourceParameter();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{   
    if(ui->lineEdit != NULL)
    {        
        float radius = ui->lineEdit->text().toFloat();
        params->setRadius(radius);
    }
	
    if(ui->lineEdit_2 != NULL)
    {       
        float waterfraction = (ui->lineEdit_2->text().toFloat())/100;
        params->setWaterFraction(waterfraction);
        if(ui->pushButton_2->text() != "Color")
        {            
            params->setWaterFirstColor(ui->pushButton_2->text().toUtf8().constData());
        }
        if(ui->pushButton_3->text() != "Color")
        {            
            params->setWaterSecondColor(ui->pushButton_3->text().toUtf8().constData());
        }
    }
	    
    if(ui->pushButton_4->text() != "Color")
    {        
        params->setTerrainFirstColor(ui->pushButton_4->text().toUtf8().constData());
    }
    if(ui->pushButton_5->text() != "Color")
    {        
        params->setTerrainSecondColor(ui->pushButton_5->text().toUtf8().constData());
    }
	
	if(ui->pushButton_6->text() != "Color")
    {        
        params->setMountainFirstColor(ui->pushButton_6->text().toUtf8().constData());
    }
    if(ui->pushButton_7->text() != "Color")
    {        
        params->setMountainSecondColor(ui->pushButton_7->text().toUtf8().constData());
    }
		
	//debug messages
	qDebug() << params->getRadius();
    qDebug() << params->getWaterFraction();
	qDebug() << QString::fromStdString(params->getWaterFirstColor());
	qDebug() << QString::fromStdString(params->getWaterSecondColor());
	qDebug() << QString::fromStdString(params->getTerrainFirstColor());
	qDebug() << QString::fromStdString(params->getTerrainSecondColor());
	qDebug() << QString::fromStdString(params->getMountainFirstColor());
	qDebug() << QString::fromStdString(params->getMountainSecondColor());
	
	for (std::vector<std::pair <float, float> >::const_iterator iter = params->getFrequencyAmplitude().begin(); iter != params->getFrequencyAmplitude().end(); ++iter)
	{
		qDebug() << iter->first <<", " << iter->second;
	}
}

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

void MainWindow::on_pushButton_6_clicked()
{
    QColor rg = QColorDialog::getColor(Qt::white,this, "Text Color",  QColorDialog::DontUseNativeDialog);
    QString qss = QString("background-color: %1").arg(rg.name());
    ui->pushButton_6->setStyleSheet(qss);
    ui->pushButton_6->setText(""+rg.name());
    qDebug() << "Color chosen: "+rg.name();
}

void MainWindow::on_pushButton_7_clicked()
{
    QColor rg = QColorDialog::getColor(Qt::white,this, "Text Color",  QColorDialog::DontUseNativeDialog);
    QString qss = QString("background-color: %1").arg(rg.name());
    ui->pushButton_7->setStyleSheet(qss);
    ui->pushButton_7->setText(""+rg.name());
    qDebug() << "Color chosen: "+rg.name();
}

void MainWindow::on_pushButton_8_clicked()
{
    openNewWindow();
}

void MainWindow::openNewWindow()
{
    dialog = new FreqAmpDialog();
    //dialog->show();
	if(dialog->exec())
	{
		for(int i=0; i<dialog->getThem()->count(); i++)
		{			
			QStringList values = dialog->getThem()->item(i)->text().split(",");
			
			//qDebug() << "first val: " + values.value(0);	
			//qDebug() << "second val: " + values.value(1);
			setAmps( values.value(0).toFloat(),  values.value(1).toFloat());
		}		
	}	
}

void MainWindow::setAmps(float p_val1, float p_val2)
{
	params->setFrequencyAmplitude(p_val1, p_val2);
}

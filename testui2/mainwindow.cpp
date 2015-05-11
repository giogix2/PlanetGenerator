#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "freqampdialog.h"
#include "../ResourceParameter.h"
#include <QDebug>
#include <QColorDialog>
#include <QVectorIterator>
#include <QtMath>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEdit->setValidator( new QIntValidator(1, 100, this) );
    ui->lineEdit_2->setValidator( new QIntValidator(0, 100, this) );

    //regex to handle max value for lineEdit to be 2*32-1 (UINT_MAX)
    QRegExp rx("^(\\d|\\d{1,9}|3\\d{1,9}|41\\d{8}|428\\d{7}|4293\\d{6}|42948\\d{5}|429495\\d{4}|4294966\\d{3}|42949671\\d{2}|429496729[0-5])$");
    QValidator *validator = new QRegExpValidator(rx, this);
    ui->lineEdit_3->setValidator(validator);
	
	//set default parameters for now::
	/*vector<float> frequency;
	frequency.push_back(0.4);
	frequency.push_back(0.06666);
	vector <float> amplitude;
	amplitude.push_back(0.02);
	amplitude.push_back(0.006666);*/

	std::string frequencyAmplitude = "0.4 0.02 0.06666 0.006666";

	float waterfraction = 0.6;
	float radius = 7.5;

    params = new std::ResourceParameter((std::string)"#00FF00",(std::string)"#FACD00",(std::string)"#32CDFF"
		,(std::string)"#64FFFF",(std::string)"#B4B4B4",(std::string)"#FFFFFF",waterfraction,radius,60,frequencyAmplitude);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{   
	if(!ui->lineEdit->text().isEmpty())
    {        
        float radius = ui->lineEdit->text().toFloat();
        params->setRadius(radius);
    }
	
    if(!ui->lineEdit_2->text().isEmpty())
    {       
        float waterfraction = (ui->lineEdit_2->text().toFloat())/100;
        params->setWaterFraction(waterfraction);
    }
    if(!ui->lineEdit_3->text().isEmpty())
    {
        int seed = ui->lineEdit_3->text().toInt();
        params->setSeed(seed);
    }

    if(ui->pushButton_2->text() != "Color")
    {
        params->setWaterFirstColor(ui->pushButton_2->text().toUtf8().constData());
    }
    if(ui->pushButton_3->text() != "Color")
    {
        params->setWaterSecondColor(ui->pushButton_3->text().toUtf8().constData());
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
    qDebug() << params->getSeed();
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
 /*   for (std::vector<float>::const_iterator iter = params->getFrequency().begin(); iter != params->getFrequency().end(); ++iter)
    {
        qDebug() << "Frequency:" << *iter;
    }
    for (std::vector<float>::const_iterator iter = params->getAmplitude().begin(); iter != params->getAmplitude().end(); ++iter)
    {
        qDebug() << "Amplitude: " << *iter;
    }*/


    for (std::vector<std::pair <std::string, int> >::const_iterator iter = params->getMeshLocObjAmount().begin(); iter != params->getMeshLocObjAmount().end(); ++iter)
    {
        qDebug() << QString::fromStdString(iter->first) <<", " << iter->second;
    }
 /*   for (std::vector<std::string>::const_iterator iter = params->getMeshLocations().begin(); iter != params->getMeshLocations().end(); ++iter)
    {
        qDebug() << "Mesh path:" << QString::fromStdString(*iter);
    }
    for (std::vector<int>::const_iterator iter = params->getObjectAmount().begin(); iter != params->getObjectAmount().end(); ++iter)
    {
        qDebug() << "Amount: " << *iter;
    }*/
	
    mySphere = new PSphere();
	mySphere->create(100, *params);
	rendering = new initOgre();
	rendering->start();
	rendering->setSceneAndRun(mySphere);
	delete mySphere;
    rendering->cleanup();

	
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
    //add items from ResourceParameter vector to listwidget
    dialog->instantiateList(params->getFrequencyAmplitude());
	std::string frequencyAmplitude;
	if(dialog->exec())
    {
        //clear ResourceParameter vector
        params->emptyFrequencyAmplitude();
		for(int i=0; i<dialog->getThem()->count(); i++)
		{			
			QStringList values = dialog->getThem()->item(i)->text().split(",");
            //put values to ResourceParameter vector
            setAmps( values.value(0).toFloat(),  values.value(1).toFloat());
			frequencyAmplitude += values.value(0).toStdString() + ' ' +  values.value(1).toStdString() + ' ';
		}
		params->setFrequencyAmplitude(frequencyAmplitude, ' ');
	}	
}

void MainWindow::setAmps(float p_val1, float p_val2)
{
    //params->setFrequencyAmplitude(p_val1, p_val2);

    //alternate way:
    //params->setFrequency(p_val1);
    //params->setAmplitude(p_val2);
}

void MainWindow::on_pushButton_9_clicked()
{
    meshdialog = new MeshDialog();
    //add items from ResourceParameter vector to listwidget
    meshdialog->instantiateList(params->getMeshLocObjAmount());
    if(meshdialog->exec())
    {
        //clear ResourceParameter vector
        params->emptyMeshLocObjAmount();
        for(int i=0; i<meshdialog->getMeshes()->count(); i++)
        {
            QStringList values = meshdialog->getMeshes()->item(i)->text().split(",");

            //put values to ResourceParameter vector
            setMeshes( values.value(0),  values.value(1).toInt());
        }
    }
}

void MainWindow::setMeshes(QString p_path, int p_count)
{
    params->setMeshLocObjAmount(p_path.toStdString(), p_count);

    //alternate way:
    //params->setMeshLocation(p_path.toStdString());
    //params->setObjectAmount(p_count);
}

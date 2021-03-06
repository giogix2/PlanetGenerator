/* The MIT License (MIT)
 * 
 * Copyright (c) 2015 Giovanni Ortolani, Taneli Mikkonen, Pingjiang Li, Tommi Puolamaa, Mitra Vahida
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE. */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "freqampdialog.h"
#include "../ResourceParameter.h"
#include <QDebug>
#include <QColorDialog>
#include <QVectorIterator>
#include <QtMath>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setFixedSize(geometry().width(), geometry().height());

    //Regex validators to handle 2 decimal float values from 0-100
    QRegExp rex("[0-9][0-9]\\.\\d{0,2}|[0-9]\\.\\d{0,2}|(100)");
    radiusvalidator = new QRegExpValidator(rex, this);

    ui->lineEdit->setValidator( radiusvalidator );

    QRegExp rex2("[0-9][0-9]\\.\\d{0,2}|[0-9]\\.\\d{0,2}|(100)");
    watervalidator = new QRegExpValidator(rex2, this);

    ui->lineEdit_2->setValidator( watervalidator );

    //regex to handle max value for lineEdit to be (2^32 − 1) (UINT_MAX)
     QRegExp rx("^(\\d|\\d{1,9}|[0-3]\\d{1,9}|4[0-1]\\d{8}|42[0-8]\\d{7}|429[0-3]\\d{6}|4294[0-8]\\d{5}|42949[0-5]\\d{4}|429496[0-6]\\d{3}|4294967[0-1]\\d{2}|42949672[0-8]\\d{1}|429496729[0-5])$");
    validator = new QRegExpValidator(rx, this);
    ui->lineEdit_3->setValidator(validator);
	
	//set default parameters for now::
	/*vector<float> frequency;
	frequency.push_back(0.4);
	frequency.push_back(0.06666);
	vector <float> amplitude;
	amplitude.push_back(0.02);
	amplitude.push_back(0.006666);*/

	std::string frequencyAmplitude = "1.0 0.02 0.3 0.008 0.1 0.005 0.06666 0.006666";
    std::vector < std::pair < std::string, int > > meshlocs;
    //meshlocs.push_back(std::make_pair("ram.mesh", 1));
    //meshlocs.push_back(std::make_pair("asteroid.mesh", 1));

	float waterfraction = 0.6;
	float radius = 7.5;
    unsigned int seed = 60;


    ui->lineEdit->setText(""+QString::number(radius));
    ui->lineEdit_2->setText(""+QString::number(waterfraction*100));
    ui->lineEdit_3->setText(""+QString::number(seed));

    params = new ResourceParameter((std::string)"#00FF00",(std::string)"#FACD00",(std::string)"#32CDFF"
        ,(std::string)"#64FFFF",(std::string)"#B4B4B4",(std::string)"#FFFFFF",waterfraction,radius,seed,frequencyAmplitude, meshlocs);

    scene = new QGraphicsScene();
}

MainWindow::~MainWindow()
{
    delete radiusvalidator;
    delete watervalidator;
    delete validator;
    delete params;
    delete scene;
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    addParameters();

    mySphere = new PSphere(32, 40, *params);
	rendering = new initOgre();
	rendering->start();
	rendering->setSceneAndRun(mySphere);
	delete mySphere;
    rendering->cleanup();
    delete rendering;
}

void MainWindow::on_pushButton_2_clicked()
{
    QColor rg = QColorDialog::getColor(Qt::white,this, "Text Color",  QColorDialog::DontUseNativeDialog);
    if(rg.isValid())
    {
        QString qss = QString("background-color: %1").arg(rg.name());
        ui->pushButton_2->setStyleSheet(qss);
        ui->pushButton_2->setText(""+rg.name());
        qDebug() << "Color chosen: "+rg.name();
    }
    else
        qDebug() << "Color not valid";
}

void MainWindow::on_pushButton_3_clicked()
{
    QColor rg = QColorDialog::getColor(Qt::white,this, "Text Color",  QColorDialog::DontUseNativeDialog);
    if(rg.isValid())
    {
        QString qss = QString("background-color: %1").arg(rg.name());
        ui->pushButton_3->setStyleSheet(qss);
        ui->pushButton_3->setText(""+rg.name());
        qDebug() << "Color chosen: "+rg.name();
    }
    else
        qDebug() << "Color not valid";
}

void MainWindow::on_pushButton_4_clicked()
{
    QColor rg = QColorDialog::getColor(Qt::white,this, "Text Color",  QColorDialog::DontUseNativeDialog);
    if(rg.isValid())
    {
        QString qss = QString("background-color: %1").arg(rg.name());
        ui->pushButton_4->setStyleSheet(qss);
        ui->pushButton_4->setText(""+rg.name());
        qDebug() << "Color chosen: "+rg.name();
    }
    else
        qDebug() << "Color not valid";
}

void MainWindow::on_pushButton_5_clicked()
{
    QColor rg = QColorDialog::getColor(Qt::white,this, "Text Color",  QColorDialog::DontUseNativeDialog);
    if(rg.isValid())
    {
        QString qss = QString("background-color: %1").arg(rg.name());
        ui->pushButton_5->setStyleSheet(qss);
        ui->pushButton_5->setText(""+rg.name());
        qDebug() << "Color chosen: "+rg.name();
    }
    else
        qDebug() << "Color not valid";
}

void MainWindow::on_pushButton_6_clicked()
{
    QColor rg = QColorDialog::getColor(Qt::white,this, "Text Color",  QColorDialog::DontUseNativeDialog);
    if(rg.isValid())
    {
        QString qss = QString("background-color: %1").arg(rg.name());
        ui->pushButton_6->setStyleSheet(qss);
        ui->pushButton_6->setText(""+rg.name());
        qDebug() << "Color chosen: "+rg.name();
    }
    else
        qDebug() << "Color not valid";
}

void MainWindow::on_pushButton_7_clicked()
{
    QColor rg = QColorDialog::getColor(Qt::white,this, "Text Color",  QColorDialog::DontUseNativeDialog);
    if(rg.isValid())
    {
        QString qss = QString("background-color: %1").arg(rg.name());
        ui->pushButton_7->setStyleSheet(qss);
        ui->pushButton_7->setText(""+rg.name());
        qDebug() << "Color chosen: "+rg.name();
    }
    else
        qDebug() << "Color not valid";
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
			frequencyAmplitude += values.value(0).toStdString() + ' ' +  values.value(1).toStdString() + ' ';
		}
		params->setFrequencyAmplitude(frequencyAmplitude, ' ');
	}	
    delete dialog;
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
            params->setMeshLocObjAmount(values.value(0).toStdString(), values.value(1).toInt());
        }
    }
    delete meshdialog;
}

void MainWindow::on_pushButton_10_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save image", "", "*.png" );
	if (!filename.isEmpty())
	{
		QString extension = filename.right(4);

		if (QString::compare(extension, ".png", Qt::CaseInsensitive))
		{
			filename.append(".png");
		}
		qDebug() << "save as : " + filename;

		//unsigned short
		QString string = ui->comboBox->currentText();

		QStringList values = string.split(" x ");

		unsigned short width =  values[0].toUShort();
		unsigned short height =  values[1].toUShort();
		bool returnValue = false;

		qDebug() << "width: " << width << ", height: " << height ;

		//create planet
		addParameters();
        mySphere = new PSphere(100, 0, *params);
		//push to pshere export-method with filename + resolution
		if(ui->comboBox_2->currentIndex() == 0)
		{
			returnValue = mySphere->exportMap(width, height, filename.toStdString(), PSphere::MAP_EQUIRECTANGULAR);
		}
		else if(ui->comboBox_2->currentIndex() == 1)
		{
			returnValue = mySphere->exportMap(width, height, filename.toStdString(), PSphere::MAP_CUBE);
		}
		else
		{
			qDebug() << "Something went wrong in saving image";
		}

		if (!returnValue)
		{
			QMessageBox errorBox;

			errorBox.setWindowTitle("Error");
			errorBox.setText("Saving image failed!");
			errorBox.setStandardButtons(QMessageBox::Ok);
			errorBox.exec();

			qDebug() << "Function exportMap returned false";
		}

        delete mySphere;
	}
}

void MainWindow::on_pushButton_11_clicked()
{    
    addParameters();
    mySphere = new PSphere(100, 0, *params);

	unsigned short width =  196;
	unsigned short height =  98;

	// One scanline must be divisible by 4 bytes
	uchar *array = mySphere->exportMap(width, height, PSphere::MAP_EQUIRECTANGULAR);
	QImage image = QImage(array, width, height, QImage::Format_RGB888);
	image = image.mirrored();

	scene->addPixmap(QPixmap::fromImage(image));

    delete[] array;
	delete mySphere;
	ui->graphicsView->setScene(scene);

    ui->graphicsView->show();

}

void MainWindow::addParameters()
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
        unsigned int seed = ui->lineEdit_3->text().toUInt();
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

	if(ui->pushButton_5->text() != "Color")
    {
		params->setTerrainFirstColor(ui->pushButton_5->text().toUtf8().constData());
    }
	if(ui->pushButton_4->text() != "Color")
    {
		params->setTerrainSecondColor(ui->pushButton_4->text().toUtf8().constData());
    }

	if(ui->pushButton_7->text() != "Color")
    {
		params->setMountainFirstColor(ui->pushButton_7->text().toUtf8().constData());
    }
	if(ui->pushButton_6->text() != "Color")
    {
		params->setMountainSecondColor(ui->pushButton_6->text().toUtf8().constData());
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
}

void MainWindow::on_pushButton_12_clicked()
{
	QString filename = QFileDialog::getSaveFileName(this, "Export mesh", "media/models", "*.mesh" );
    if (!filename.isEmpty())
    {
        QString extension = filename.right(5);

        if (QString::compare(extension, ".mesh", Qt::CaseInsensitive))
        {
            filename.append(".mesh");
        }
        qDebug() << "save as : " + filename;

        //call initogre exportmesh function
		initOgre *temp = new initOgre;
        PSphere *tempPlanet = new PSphere(100, 0, *params);
		temp->savePlanetAsMesh(tempPlanet, filename.toStdString());
		// Must delete PSphere before cleaning initOgre
		delete tempPlanet;
		temp->cleanupSavePlanetAsMesh();
        delete temp;
	}
}

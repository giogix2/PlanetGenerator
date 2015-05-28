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

#include "freqampdialog.h"
#include "ui_freqampdialog.h"
#include "mainwindow.h"
#include <QDebug>

FreqAmpDialog::FreqAmpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FreqAmpDialog)
{
    ui->setupUi(this);

    setFixedSize(geometry().width(), geometry().height());

    QRegExp rex("[0-9]\\.\\d{0,6}|(10)");
    validator = new QRegExpValidator(rex, this);

    ui->lineEdit->setValidator( validator );
    ui->lineEdit_2->setValidator( validator );
}

FreqAmpDialog::~FreqAmpDialog()
{
    delete validator;
    delete ui;
}

void FreqAmpDialog::on_pushButton_clicked()
{
    QString freq = ui->lineEdit->text();//.toFloat();
    QString amp = ui->lineEdit_2->text();//.toFloat();


    ui->listWidget->addItem(""+freq+","+amp);

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
    }
}

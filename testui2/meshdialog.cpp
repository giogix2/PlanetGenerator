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

#include "meshdialog.h"
#include "ui_meshdialog.h"
#include <QFileDialog>

MeshDialog::MeshDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeshDialog)
{
    ui->setupUi(this);    

    setFixedSize(geometry().width(), geometry().height());
}

MeshDialog::~MeshDialog()
{
    delete ui;
}

void MeshDialog::on_pushButton_clicked()
{
    QDir dir("../");
    QString path;

    path = dir.relativeFilePath(QFileDialog::getOpenFileName(this, "Choose mesh", "../media/models", "*.mesh"));

    ui->lineEdit->setText(path);

    //QListWidgetItem *item = new QListWidgetItem(meshpath.toString());
    //item->setTextAlignment(Qt::AlignRight);

}

QListWidget* MeshDialog::getMeshes()
{
    return ui->listWidget;
}

void MeshDialog::on_pushButton_2_clicked()
{
    ui->listWidget->addItem(ui->lineEdit->text()+", "+QString::number(ui->spinBox->value()));
}

void MeshDialog::on_pushButton_3_clicked()
{
    qDeleteAll(ui->listWidget->selectedItems());
}
void MeshDialog::instantiateList(std::vector< std::pair < std::string, int > > meshLocObjAmount)
{
    for (std::vector<std::pair <std::string, int> >::const_iterator iter = meshLocObjAmount.begin(); iter != meshLocObjAmount.end(); ++iter)
    {
        QString meshLoc = QString::fromStdString(iter->first);
        QString objAmount = QString::number(iter->second);
        ui->listWidget->addItem(""+meshLoc+","+objAmount);
        //qDebug() << QString::fromStdString(iter->first) <<", " << QString::number(iter->second);
    }
}

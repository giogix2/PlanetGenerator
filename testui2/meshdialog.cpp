#include "meshdialog.h"
#include "ui_meshdialog.h"
#include <QFileDialog>

MeshDialog::MeshDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeshDialog)
{
    ui->setupUi(this);    
}

MeshDialog::~MeshDialog()
{
    delete ui;
}

void MeshDialog::on_pushButton_clicked()
{
    ui->lineEdit->setText(QFileDialog::getOpenFileName(this, "Choose mesh", "../", "*.mesh"));

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

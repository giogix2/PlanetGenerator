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

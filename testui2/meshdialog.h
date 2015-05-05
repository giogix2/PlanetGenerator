#ifndef MESHDIALOG_H
#define MESHDIALOG_H

#include <QDialog>
#include <QListWidget>

namespace Ui {
class MeshDialog;
}

class MeshDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MeshDialog(QWidget *parent = 0);
    ~MeshDialog();

    QListWidget* getMeshes();
    void instantiateList(std::vector< std::pair < std::string, int > > meshLocObjAmount);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MeshDialog *ui;
};

#endif // MESHDIALOG_H

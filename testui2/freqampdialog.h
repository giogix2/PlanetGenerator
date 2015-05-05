#ifndef FREQAMPDIALOG_H
#define FREQAMPDIALOG_H

#include <QDialog>
#include <QListWidget>
namespace Ui {
class FreqAmpDialog;
}

class FreqAmpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FreqAmpDialog(QWidget *parent = 0);
    ~FreqAmpDialog();
	
	QListWidget* getThem();
    void instantiateList(std::vector< std::pair < float, float > > freqamps);

private slots:
    void on_pushButton_clicked();

    void on_buttonBox_accepted();

    void on_pushButton_2_clicked();

private:
    Ui::FreqAmpDialog *ui;
};

#endif // FREQAMPDIALOG_H

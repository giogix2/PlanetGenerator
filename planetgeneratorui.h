#ifndef PLANETGENERATORUI_H
#define PLANETGENERATORUI_H

#include <QtWidgets/QMainWindow>
#include "ui_planetgeneratorui.h"

class PlanetGeneratorUI : public QMainWindow
{
	Q_OBJECT

public:
	PlanetGeneratorUI(QWidget *parent = 0);
	~PlanetGeneratorUI();

private:
	Ui::PlanetGeneratorUIClass ui;
};

#endif // PLANETGENERATORUI_H

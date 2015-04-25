/********************************************************************************
** Form generated from reading UI file 'planetgeneratorui.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLANETGENERATORUI_H
#define UI_PLANETGENERATORUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PlanetGeneratorUIClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *PlanetGeneratorUIClass)
    {
        if (PlanetGeneratorUIClass->objectName().isEmpty())
            PlanetGeneratorUIClass->setObjectName(QStringLiteral("PlanetGeneratorUIClass"));
        PlanetGeneratorUIClass->resize(600, 400);
        menuBar = new QMenuBar(PlanetGeneratorUIClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        PlanetGeneratorUIClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(PlanetGeneratorUIClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        PlanetGeneratorUIClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(PlanetGeneratorUIClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        PlanetGeneratorUIClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(PlanetGeneratorUIClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        PlanetGeneratorUIClass->setStatusBar(statusBar);

        retranslateUi(PlanetGeneratorUIClass);

        QMetaObject::connectSlotsByName(PlanetGeneratorUIClass);
    } // setupUi

    void retranslateUi(QMainWindow *PlanetGeneratorUIClass)
    {
        PlanetGeneratorUIClass->setWindowTitle(QApplication::translate("PlanetGeneratorUIClass", "PlanetGeneratorUI", 0));
    } // retranslateUi

};

namespace Ui {
    class PlanetGeneratorUIClass: public Ui_PlanetGeneratorUIClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLANETGENERATORUI_H

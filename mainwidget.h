#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QTableView>
#include <QComboBox>
#include <QLabel>

#include <QTime>


#include "mymodel.h"

class mainWidget : public QWidget
{
    Q_OBJECT

public:
    mainWidget(QWidget *parent = 0);
    ~mainWidget();

private:
    QLabel         *connectComBoxLabel;
    QComboBox      *connectComBox;

    QLabel         *inputSourceComBoxLabel;
    QComboBox      *inputSourceComBox;

    QComboBox      *arithComBox;
    QPushButton    *logWriteBtn;

    QLabel         *timeCount;

    QTableView     *udpDataTable;
    QHBoxLayout    *TopLayout;
    QGridLayout    *TopBtnLayout;
    QHBoxLayout    *BottomLayout;

public:
    MyModel myModel;

    QDateTime startime;
    QString desTimeStr;

public slots:
    void on_pushButtonlogWriteBtn_clicked();
    void on_connectComBox_changed(int);
    void on_inputSourceComBox_changed(int);
    void timerHit();
};

#endif // MAINWIDGET_H

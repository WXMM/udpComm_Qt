#include "mainwidget.h"

#include <QDebug>

mainWidget::mainWidget(QWidget *parent)
    : QWidget(parent)
{

    connectComBoxLabel = new QLabel(tr("连接器选择："));
    connectComBoxLabel->setAlignment(Qt::AlignRight);
    connectComBox = new QComboBox(); 
    for(int i=1;i<=10;i++)
        connectComBox->addItem(QString::asprintf("X3%02d",i));

    inputSourceComBoxLabel = new QLabel(tr("源输入选择："));
    inputSourceComBoxLabel->setAlignment(Qt::AlignRight);
    inputSourceComBox = new QComboBox();
    for(int i=0;i<6;i++)
        inputSourceComBox->addItem(QString::asprintf("%dV",i));


    arithComBox   = new QComboBox();
    logWriteBtn   = new QPushButton(tr("开始测试"));

    timeCount     = new QLabel(tr("0"));

    TopBtnLayout    = new QGridLayout();
    TopBtnLayout->setSpacing(50);
    TopBtnLayout->setMargin(50);

    connectComBoxLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    TopBtnLayout->addWidget(connectComBoxLabel,0,0,Qt::AlignVCenter);
    connectComBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    TopBtnLayout->addWidget(connectComBox,0,1,Qt::AlignVCenter);
    inputSourceComBoxLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    TopBtnLayout->addWidget(inputSourceComBoxLabel,0,2,Qt::AlignVCenter);
    inputSourceComBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    TopBtnLayout->addWidget(inputSourceComBox,0,3,Qt::AlignVCenter);
    logWriteBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    TopBtnLayout->addWidget(logWriteBtn,0,4,Qt::AlignVCenter);
    timeCount->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    TopBtnLayout->addWidget(timeCount,0,5,Qt::AlignVCenter);

    TopBtnLayout->setColumnStretch(0,1);
    TopBtnLayout->setColumnStretch(1,2);
    TopBtnLayout->setColumnStretch(2,1);
    TopBtnLayout->setColumnStretch(3,2);
    TopBtnLayout->setColumnStretch(4,2);

    udpDataTable = new QTableView();
    udpDataTable->setModel(&myModel);

    TopLayout       = new QHBoxLayout();
    TopLayout->addLayout(TopBtnLayout);
    BottomLayout    = new QHBoxLayout();
    BottomLayout->addWidget(udpDataTable);

    QGridLayout  *mainLayout = new QGridLayout(this);
    mainLayout->addLayout(TopLayout,0,0,1,1);
    mainLayout->addLayout(BottomLayout,1,0,1,1);

    connect(logWriteBtn,SIGNAL(clicked()),this,SLOT(on_pushButtonlogWriteBtn_clicked()));
    connect(logWriteBtn,SIGNAL(clicked()),&myModel,SLOT(startTest()));
    connect(connectComBox,SIGNAL(currentIndexChanged(int)),this,SLOT(on_connectComBox_changed(int)));
    connect(inputSourceComBox,SIGNAL(currentIndexChanged(int)),this,SLOT(on_inputSourceComBox_changed(int)));

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout , this, &mainWidget::timerHit);
    timer->start(1000);

}

mainWidget::~mainWidget()
{

}

void mainWidget::on_pushButtonlogWriteBtn_clicked(){
    if(logWriteBtn->text() == tr("开始测试")){
        startime = QDateTime::currentDateTime();
//        myModel.isStartWriteLog = 1;
        logWriteBtn->setText(tr("停止测试"));
    }else{
//        myModel.isStartWriteLog = 0;
        myModel.timeLag = desTimeStr;
        logWriteBtn->setText(tr("开始测试"));
    };
}

void mainWidget::on_connectComBox_changed(int index){
    myModel.commIndex = index;
    myModel.updateViewTable();
}

void mainWidget::on_inputSourceComBox_changed(int index){
    myModel.inputSourceIndex = index;
}

void mainWidget::timerHit(){
    QDateTime nowTime = QDateTime::currentDateTime();

    uint stime = startime.toTime_t();
    uint etime = nowTime.toTime_t();

    QDateTime desTime = QDateTime::fromTime_t(etime-stime);

    desTimeStr = desTime.toString("mm:ss");

    if(myModel.isStartWriteLog){
        timeCount->setText(desTimeStr);
    }else{
        timeCount->setText(tr("00:00"));
    }

}

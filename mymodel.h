#ifndef MYMODEL_H
#define MYMODEL_H


// mymodel.h
#include <QAbstractTableModel>
#include <QTimer>
#include <QTime>

#include <QObject>
#include <QUdpSocket>
#include <QFile>
#include <QDir>
#include <QCoreApplication>

struct UdpDtaFrame{
    uchar headFrame[2]   = {0x00};
    uchar lengthFrame[2] = {0x00};
    uchar sourceFrame[2] = {0x00};
    uchar dateFrame[4]   = {0x00};
    uchar timeFrame[4]   = {0x00};
    uchar countFrame     = 0x00;
    uchar checkFrame     = 0x00;
    uchar dataFrame[200][4];
};

struct commConfig{
    int beginIndex;
    int endIndex;
    int length;
};

class MyModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    MyModel(QObject *parent = nullptr);
    ~MyModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

public:
    void initTableViewModel();
    void initUdpSet();
    void dataProc(QByteArray *arrRes);
    void udpToFloat(uchar *udpS,float *dtaFloat);
    void updateViewTable();

private:
    QStringList header;
    QStringList commIndexlist;

public slots:
    void timerHit();
    void dataReceived();
    void startTest();

private:
    //udp通信相关
    int        port;
    QUdpSocket *udpSocket;
public:
    bool       isStartWriteLog;
    int        count;
    int        udpCount;

public:
    QByteArray   arrayRes;
    QHostAddress address;
    quint16      targetPort;

    //起始通道号-结束通道号
    int dataFrameLength;
    uint commIndex;
    uint commBeginIndex;
    uint commEndIndex;
    uint commIndexLength;
    commConfig commConfigArr[10];

    //源信号选择
    uint inputSourceIndex;

    //帧处理结构体及输出数组
    UdpDtaFrame udpDtaFrame;
    float       outDtaArray[100][10];  //0表示主机数据 1表示从机数据 2-3主机数据的最大最小值  4-5从机数据的最大最小值

    QDir   logDir;
    QFile  *logFile1;
    QFile  *logFile2;
    QFile  *logFile;

    bool   logFileRes1;
    bool   logFileRes2;
    bool   logFileRes;


public:
    QTimer *timer;
    QString  timeLag;//测试时长
};

#endif // MYMODEL_H

// mymodel.cpp
#include "mymodel.h"
#include <QDebug>

MyModel::MyModel(QObject *parent)
    : QAbstractTableModel(parent)
    , timer(new QTimer(this))
{
    //初始化表格样式
    initTableViewModel();
    initUdpSet();
}

MyModel::~MyModel(){
    if(logFileRes1)
        logFile1->close();
    if(logFileRes2)
        logFile2->close();
    if(logFileRes)
        logFile->close();
}

int MyModel::rowCount(const QModelIndex & /*parent*/) const
{
   return 16;
}

int MyModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 8;
}

QVariant MyModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();

    QString tempStr;
    if(!index.isValid())
        return QVariant();
    if(role==Qt::DisplayRole){
        switch(col){
        case 0:
            if(row<commIndexLength)
            return tempStr.sprintf("通道%d",commBeginIndex+index.row());
        case 1:
            if(row<commIndexLength)
            return tempStr.sprintf("%.4f",outDtaArray[index.row()][0]);
//            return tempStr.sprintf("%.02X",udpDtaFrame.dataFrame[index.row()][0]);
            break;
        case 2:
            if(row<commIndexLength)
            return tempStr.sprintf("%.4f",outDtaArray[index.row()][1]);
            break;
        case 3:
            if(row<commIndexLength)
            return tempStr.sprintf("%.4f",outDtaArray[index.row()][2]);
            break;
        case 4:
            if(commIndexLength>16 && row<commIndexLength-16)
                return tempStr.sprintf("通道%d",commBeginIndex+index.row()+16);
        case 5:
            if(commIndexLength>16 && row<commIndexLength-16)
                return tempStr.sprintf("%.4f",outDtaArray[index.row()+16][0]);
            break;
        case 6:
            if(commIndexLength>16 && row<commIndexLength-16)
                return tempStr.sprintf("%.4f",outDtaArray[index.row()+16][1]);
            break;
        case 7:
            if(commIndexLength>16 && row<commIndexLength-16)
                return tempStr.sprintf("%.4f",outDtaArray[index.row()+16][2]);
            break;
        default:
            return QVariant();
        }
    }
    return QVariant();
}

QVariant MyModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return header[section];
    return QAbstractTableModel::headerData(section,orientation,role);
}

void MyModel::initTableViewModel(){

    //配置通道数据
    commConfigArr[0].beginIndex = 1;
    commConfigArr[0].endIndex   = 19;
    commConfigArr[0].length     = 19;

    commConfigArr[1].beginIndex = 20;
    commConfigArr[1].endIndex   = 33;
    commConfigArr[1].length     = 14;

    commConfigArr[2].beginIndex = 34;
    commConfigArr[2].endIndex   = 56;
    commConfigArr[2].length     = 23;

    commConfigArr[3].beginIndex = 57;
    commConfigArr[3].endIndex   = 68;
    commConfigArr[3].length     = 12;

    commConfigArr[4].beginIndex = 69;
    commConfigArr[4].endIndex   = 88;
    commConfigArr[4].length     = 20;


    commConfigArr[5].beginIndex = 89;
    commConfigArr[5].endIndex   = 107;
    commConfigArr[5].length     = 19;

    commConfigArr[6].beginIndex = 108;
    commConfigArr[6].endIndex   = 127;
    commConfigArr[6].length     = 20;

    commConfigArr[7].beginIndex = 128;
    commConfigArr[7].endIndex   = 146;
    commConfigArr[7].length     = 19;

    commConfigArr[8].beginIndex = 147;
    commConfigArr[8].endIndex   = 167;
    commConfigArr[8].length     = 21;

    commConfigArr[9].beginIndex = 168;
    commConfigArr[9].endIndex   = 188;
    commConfigArr[9].length     = 21;

    //设置表头
    QString tempStr;
    header<<tr("通道号")<<tr("主机数据")<<tr("从机数据")<<tr("数据处理")<<tr("通道号")<<tr("主机数据")<<tr("从机数据")<<tr("数据处理");
    commIndex = 0;         //初始化连接器选择
    inputSourceIndex = 0;  //初始化源输入选择
    commBeginIndex  = commConfigArr[commIndex].beginIndex;
    commEndIndex    = commConfigArr[commIndex].endIndex;
    commIndexLength = commConfigArr[commIndex].length;

    //初始化输出数据数组
    for(int i=0;i<100;i++){
        for(int j=0;j<10;j++){
            outDtaArray[i][j] = 0;
        }
    }


    //创建日志文件
    udpCount = 0;
    isStartWriteLog = 0;
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("测试文件yyyyMMddhhmmss");

    QString dir_str = QCoreApplication::applicationDirPath()+"/log/"+current_date;
    if (!logDir.exists(dir_str))
    {
        bool res = logDir.mkdir(dir_str);
        qDebug() << "不存在该目录"<<res;
    }

    logFile1 = new QFile;
    logFile1->setFileName(dir_str+"/主机数据.txt");
    logFileRes1 = logFile1->open(QIODevice::WriteOnly | QIODevice::Text);

    logFile2 = new QFile;
    logFile2->setFileName(dir_str+"/从机数据.txt");
    logFileRes2 = logFile2->open(QIODevice::WriteOnly | QIODevice::Text);

    logFile = new QFile;
    logFile->setFileName(dir_str+"/输出结果.txt");
    logFileRes = logFile->open(QIODevice::WriteOnly | QIODevice::Text);

}

void MyModel::initUdpSet(){
    //初始化udp通信
    port      = 2711;
    udpSocket = new QUdpSocket();

    bool res = udpSocket->bind(QHostAddress::AnyIPv4,port);
    QHostAddress mcast_addr("227.1.240.5");
    udpSocket->joinMulticastGroup(mcast_addr);//这句是关键，加入组播地址

    if(!res){
        qDebug()<<QObject::tr("creat error!!!");
    }else{
        qDebug()<<QObject::tr("creat success!!!");
    }

    qDebug()<<tr("线程开启");
    qDebug()<<tr("信号槽建立，等待输入数据");

    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(dataReceived()));

}

void MyModel::dataReceived(){
    while(udpSocket->hasPendingDatagrams()){
        qDebug()<<tr("收到数据")<<udpCount++;
        arrayRes.resize(udpSocket->bytesAvailable());//根据可读数据来设置空间大小
        udpSocket->readDatagram(arrayRes.data(),arrayRes.size(),&address,&targetPort); //读取数据

        //处理接收到的数据帧
        dataProc(&arrayRes);
        //更新页面
        updateViewTable();
    }

}

void MyModel::dataProc(QByteArray *arrRes){

    qDebug()<<tr("处理数据");
    dataFrameLength = 189;
    //帧头数据提取
    udpDtaFrame.headFrame[0] = (uchar)((*arrRes).data()[0]);
    udpDtaFrame.headFrame[1] = (uchar)((*arrRes).data()[1]);

    //信源数据提取
    udpDtaFrame.sourceFrame[0] = (uchar)((*arrRes).data()[7]);
    udpDtaFrame.sourceFrame[1] = (uchar)((*arrRes).data()[8]);

    for(int i=0;i<dataFrameLength;i++){
        udpDtaFrame.dataFrame[i][0] = (uchar)(arrRes->data()[36+i*12]);
        udpDtaFrame.dataFrame[i][1] = (uchar)(arrRes->data()[37+i*12]);
        udpDtaFrame.dataFrame[i][2] = (uchar)(arrRes->data()[38+i*12]);
        udpDtaFrame.dataFrame[i][3] = (uchar)(arrRes->data()[39+i*12]);
    }

    uchar datS[4];
    QString logTempStr,tempStr;
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("hh:mm:ss");
    logTempStr += current_date;
    if(udpDtaFrame.sourceFrame[0] == 0x04 && udpDtaFrame.sourceFrame[1] == 0x0b){
        //表示此帧为主机数据
        for(int i=0;i<=commEndIndex-commBeginIndex;i++){
            //这里完成数据帧转换为真实数值
            datS[0] = udpDtaFrame.dataFrame[commBeginIndex+i-1][0];
            datS[1] = udpDtaFrame.dataFrame[commBeginIndex+i-1][1];
            datS[2] = udpDtaFrame.dataFrame[commBeginIndex+i-1][2];
            datS[3] = udpDtaFrame.dataFrame[commBeginIndex+i-1][3];
            udpToFloat(datS,&outDtaArray[i][0]);

            //处理最大最小值，未开始之前填充实际值
            if(isStartWriteLog==1){
                outDtaArray[i][2] = outDtaArray[i][0]>outDtaArray[i][2]?outDtaArray[i][0]:outDtaArray[i][2];
                outDtaArray[i][3] = outDtaArray[i][0]<outDtaArray[i][3]?outDtaArray[i][0]:outDtaArray[i][3];
            }else{
               outDtaArray[i][2] = outDtaArray[i][0];
               outDtaArray[i][3] = outDtaArray[i][0];
            }

            logTempStr += tempStr.sprintf("%12.4f",outDtaArray[i][0]);
        }
        logTempStr += "\n";
        if(isStartWriteLog==1)
            logFile1->write(logTempStr.toUtf8());
    }else{
        //表示此帧为从机数据
        for(int i=0;i<=commEndIndex-commBeginIndex;i++){
            //这里完成数据帧转换为真实数值
            datS[0] = udpDtaFrame.dataFrame[commBeginIndex+i-1][0];
            datS[1] = udpDtaFrame.dataFrame[commBeginIndex+i-1][1];
            datS[2] = udpDtaFrame.dataFrame[commBeginIndex+i-1][2];
            datS[3] = udpDtaFrame.dataFrame[commBeginIndex+i-1][3];
            udpToFloat(datS,&outDtaArray[i][1]);

            //处理最大最小值，未开始之前填充实际值
            if(isStartWriteLog==1){
                outDtaArray[i][4] = outDtaArray[i][1]>outDtaArray[i][4]?outDtaArray[i][1]:outDtaArray[i][4];
                outDtaArray[i][5] = outDtaArray[i][1]<outDtaArray[i][5]?outDtaArray[i][1]:outDtaArray[i][5];
            }else{
               outDtaArray[i][4] = outDtaArray[i][1];
               outDtaArray[i][5] = outDtaArray[i][1];
            }

            logTempStr += tempStr.sprintf("%12.4f",outDtaArray[i][1]);
        }
        logTempStr += "\n";
        if(isStartWriteLog==1)
            logFile2->write(logTempStr.toUtf8());
    }
}

void MyModel::timerHit()
{
    QTime::currentTime().toString();
}

void MyModel::startTest(){
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss");
    QString logTempStr,tempStr;
    if(isStartWriteLog==0){
        //开始测试
        logTempStr  = tempStr.sprintf("开始测试时间：");
        logTempStr  = logTempStr + current_date + "   \n";
        logTempStr  += tempStr.sprintf("连接器选择：X3%02d\n",commIndex+1);
        logTempStr  += tempStr.sprintf("源输出：%dV\n",inputSourceIndex);
        logTempStr  += tempStr.sprintf("测试通道：通道%d~通道%d\n",commBeginIndex,commEndIndex);
//        logTempStr += tempStr.sprintf("帧计数：%d",udpCount);
        logTempStr += tempStr.sprintf("\n%-16s"," ");
        for(int i=0;i<=commEndIndex-commBeginIndex;i++){
            logTempStr += tempStr.sprintf("通道%-8d ",commBeginIndex+i);
        }
        logTempStr += "\n";
        logFile1->write(logTempStr.toUtf8());
        logFile2->write(logTempStr.toUtf8());
        logFile->write(logTempStr.toUtf8());


        qDebug()<<"开始测试";
        isStartWriteLog=1;

    }else{
        //停止测试
        logTempStr  = tempStr.sprintf("\n\n\n结束测试时间：");
        logTempStr  += current_date + "\n";
        logTempStr  += "----------------停止测试------------\n\n\n";
        logFile1->write(logTempStr.toUtf8());
        logFile2->write(logTempStr.toUtf8());

        //写入结果
        logTempStr = "";
        logTempStr += tempStr.sprintf("\n%-10s","主机max值");
        for(int i=0;i<=commEndIndex-commBeginIndex;i++){
            logTempStr += tempStr.sprintf("%12.4f",outDtaArray[i][2]);
        }
        logTempStr += tempStr.sprintf("\n%-10s","主机min值");
        for(int i=0;i<=commEndIndex-commBeginIndex;i++){
            logTempStr += tempStr.sprintf("%12.4f",outDtaArray[i][3]);
        }
        logTempStr += tempStr.sprintf("\n%-10s","从机max值");
        for(int i=0;i<=commEndIndex-commBeginIndex;i++){
            logTempStr += tempStr.sprintf("%12.4f",outDtaArray[i][4]);
        }
        logTempStr += tempStr.sprintf("\n%-10s","从机min值");
        for(int i=0;i<=commEndIndex-commBeginIndex;i++){
            logTempStr += tempStr.sprintf("%12.4f",outDtaArray[i][5]);
        }

        logTempStr  += tempStr.sprintf("\n\n测试时长：");
        logTempStr  += timeLag + "\n";
        logTempStr  += tempStr.sprintf("结束测试时间：");
        logTempStr  += current_date + "\n";     
        logTempStr  += "----------------停止测试------------\n\n\n";
        logFile->write(logTempStr.toUtf8());
        qDebug()<<"停止测试";
        isStartWriteLog=0;
    }
}


void MyModel::updateViewTable(){

    commBeginIndex  = commConfigArr[commIndex].beginIndex;
    commEndIndex    = commConfigArr[commIndex].endIndex;
    commIndexLength = commConfigArr[commIndex].length;

    //更新页面数据
    for(int i=0;i<16;i++){
        //we identify the top left cell
        QModelIndex lIndex = createIndex(i,0);
        //emit a signal to make the view reread identified data
        emit dataChanged(lIndex, lIndex, {Qt::DisplayRole});
        //we identify the top left cell
        QModelIndex lIndex1 = createIndex(i,1);
        //emit a signal to make the view reread identified data
        emit dataChanged(lIndex1, lIndex1, {Qt::DisplayRole});
        //we identify the top left cell
        QModelIndex lIndex2 = createIndex(i,2);
        //emit a signal to make the view reread identified data
        emit dataChanged(lIndex2, lIndex2, {Qt::DisplayRole});
        //we identify the top left cell
        QModelIndex lIndex3 = createIndex(i,3);
        //emit a signal to make the view reread identified data
        emit dataChanged(lIndex3, lIndex3, {Qt::DisplayRole});

        //we identify the top left cell
        QModelIndex rIndex = createIndex(i,4);
        //emit a signal to make the view reread identified data
        emit dataChanged(rIndex, rIndex, {Qt::DisplayRole});
        //we identify the top left cell
        QModelIndex rIndex1 = createIndex(i,5);
        //emit a signal to make the view reread identified data
        emit dataChanged(rIndex1, rIndex1, {Qt::DisplayRole});
        //we identify the top left cell
        QModelIndex rIndex2 = createIndex(i,6);
        //emit a signal to make the view reread identified data
        emit dataChanged(rIndex2, rIndex2, {Qt::DisplayRole});
        //we identify the top left cell
        QModelIndex rIndex3 = createIndex(i,7);
        //emit a signal to make the view reread identified data
        emit dataChanged(rIndex3, rIndex3, {Qt::DisplayRole});
    }
}

void MyModel::udpToFloat(uchar *udpS,float *dtaFloat){
    memcpy(dtaFloat,udpS,4);
}


#include "mainwindow.h"
#include <QFileDialog>
#include <QDateTime>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , serial(new QSerialPort(this))
    , autoSendTimer(new QTimer(this))
    , sendBytes(0)
    , receiveBytes(0)
{
    setWindowTitle("Qt6 串口工具");
    setGeometry(0, 0, 800, 600);
    
    // 初始化UI组件，但不连接保存设置的信号槽
    initUI();
    
    // 连接信号槽
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    connect(autoSendTimer, &QTimer::timeout, this, &MainWindow::autoSendData);
    
    // 初始化配置
    updateSerialPorts();
    updateCodecList();
    
    // 加载设置，使用INI文件替代注册表，并指定为应用程序目录
    QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";
    QSettings settings(configPath, QSettings::IniFormat);
    
    // 设置初始状态
    pushButton_open->setText("打开串口");
    label_status->setText("串口未打开");
    label_status->setStyleSheet("color: red;");
    label_sendCount->setText("发送: 0 字节");
    label_receiveCount->setText("接收: 0 字节");
    
    // 应用设置（默认显示时间戳和日志模式）
    checkBox_timestamp->setChecked(settings.value("timestamp", true).toBool());
    checkBox_logMode->setChecked(settings.value("logMode", true).toBool());
    checkBox_hexReceive->setChecked(settings.value("hexReceive", false).toBool());
    checkBox_hexSend->setChecked(settings.value("hexSend", false).toBool());
    
    // 在应用初始设置后，连接保存设置的信号槽，避免初始设置被覆盖
    connect(checkBox_timestamp, &QCheckBox::stateChanged, this, &MainWindow::saveSettings);
    connect(checkBox_logMode, &QCheckBox::stateChanged, this, &MainWindow::saveSettings);
    connect(checkBox_hexReceive, &QCheckBox::stateChanged, this, &MainWindow::saveSettings);
    connect(checkBox_hexSend, &QCheckBox::stateChanged, this, &MainWindow::saveSettings);
}

MainWindow::~MainWindow()
{
    delete serial;
    delete autoSendTimer;
}

void MainWindow::initUI()
{
    // 创建中央部件和主布局
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    setCentralWidget(centralWidget);
    
    // 串口配置区域
    groupBox_serialConfig = new QGroupBox("串口配置", centralWidget);
    gridLayout_serialConfig = new QGridLayout(groupBox_serialConfig);
    
    label_portName = new QLabel("端口:", groupBox_serialConfig);
    comboBox_portName = new QComboBox(groupBox_serialConfig);
    pushButton_refresh = new QPushButton("刷新", groupBox_serialConfig);
    pushButton_open = new QPushButton("打开", groupBox_serialConfig);
    label_status = new QLabel("串口未打开", groupBox_serialConfig);
    
    label_baudRate = new QLabel("波特率:", groupBox_serialConfig);
    comboBox_baudRate = new QComboBox(groupBox_serialConfig);
    comboBox_baudRate->addItems({"9600", "19200", "38400", "57600", "115200", "230400", "460800", "921600"});
    comboBox_baudRate->setCurrentText("115200");
    
    label_dataBits = new QLabel("数据位:", groupBox_serialConfig);
    comboBox_dataBits = new QComboBox(groupBox_serialConfig);
    comboBox_dataBits->addItems({"5", "6", "7", "8"});
    comboBox_dataBits->setCurrentText("8");
    
    label_stopBits = new QLabel("停止位:", groupBox_serialConfig);
    comboBox_stopBits = new QComboBox(groupBox_serialConfig);
    comboBox_stopBits->addItems({"1", "1.5", "2"});
    comboBox_stopBits->setCurrentText("1");
    
    label_parity = new QLabel("校验位:", groupBox_serialConfig);
    comboBox_parity = new QComboBox(groupBox_serialConfig);
    comboBox_parity->addItems({"无", "奇校验", "偶校验", "标记校验", "空格校验"});
    comboBox_parity->setCurrentText("无");
    
    label_flowControl = new QLabel("流控制:", groupBox_serialConfig);
    comboBox_flowControl = new QComboBox(groupBox_serialConfig);
    comboBox_flowControl->addItems({"无", "硬件流控", "软件流控"});
    comboBox_flowControl->setCurrentText("无");
    
    // 布局串口配置区域
    gridLayout_serialConfig->addWidget(label_portName, 0, 0);
    gridLayout_serialConfig->addWidget(comboBox_portName, 0, 1);
    gridLayout_serialConfig->addWidget(pushButton_refresh, 0, 2);
    gridLayout_serialConfig->addWidget(pushButton_open, 0, 3);
    gridLayout_serialConfig->addWidget(label_status, 0, 4);
    
    gridLayout_serialConfig->addWidget(label_baudRate, 1, 0);
    gridLayout_serialConfig->addWidget(comboBox_baudRate, 1, 1);
    gridLayout_serialConfig->addWidget(label_dataBits, 1, 2);
    gridLayout_serialConfig->addWidget(comboBox_dataBits, 1, 3);
    gridLayout_serialConfig->addWidget(label_stopBits, 1, 4);
    gridLayout_serialConfig->addWidget(comboBox_stopBits, 1, 5);
    
    gridLayout_serialConfig->addWidget(label_parity, 2, 0);
    gridLayout_serialConfig->addWidget(comboBox_parity, 2, 1);
    gridLayout_serialConfig->addWidget(label_flowControl, 2, 2);
    gridLayout_serialConfig->addWidget(comboBox_flowControl, 2, 3);
    
    mainLayout->addWidget(groupBox_serialConfig);
    
    // 编码配置区域
    groupBox_codecConfig = new QGroupBox("编码配置", centralWidget);
    horizontalLayout_codec = new QHBoxLayout(groupBox_codecConfig);
    
    label_sendCodec = new QLabel("发送编码:", groupBox_codecConfig);
    comboBox_sendCodec = new QComboBox(groupBox_codecConfig);
    label_receiveCodec = new QLabel("接收编码:", groupBox_codecConfig);
    comboBox_receiveCodec = new QComboBox(groupBox_codecConfig);
    
    horizontalLayout_codec->addWidget(label_sendCodec);
    horizontalLayout_codec->addWidget(comboBox_sendCodec);
    horizontalLayout_codec->addWidget(label_receiveCodec);
    horizontalLayout_codec->addWidget(comboBox_receiveCodec);
    
    mainLayout->addWidget(groupBox_codecConfig);
    
    // 主数据区域（接收和发送）
    QGridLayout *gridLayout_main = new QGridLayout;
    
    // 接收区域
    groupBox_receive = new QGroupBox("接收", centralWidget);
    verticalLayout_receive = new QVBoxLayout(groupBox_receive);
    
    horizontalLayout_receiveOptions = new QHBoxLayout;
    checkBox_hexReceive = new QCheckBox("十六进制显示", groupBox_receive);
    checkBox_timestamp = new QCheckBox("显示时间戳", groupBox_receive);
    checkBox_logMode = new QCheckBox("日志模式", groupBox_receive);
    pushButton_clearReceive = new QPushButton("清空", groupBox_receive);
    pushButton_save = new QPushButton("保存", groupBox_receive);
    
    horizontalLayout_receiveOptions->addWidget(checkBox_hexReceive);
    horizontalLayout_receiveOptions->addWidget(checkBox_timestamp);
    horizontalLayout_receiveOptions->addWidget(checkBox_logMode);
    horizontalLayout_receiveOptions->addWidget(pushButton_clearReceive);
    horizontalLayout_receiveOptions->addWidget(pushButton_save);
    
    plainTextEdit_receive = new QPlainTextEdit(groupBox_receive);
    plainTextEdit_receive->setReadOnly(true);
    
    verticalLayout_receive->addLayout(horizontalLayout_receiveOptions);
    verticalLayout_receive->addWidget(plainTextEdit_receive);
    
    gridLayout_main->addWidget(groupBox_receive, 0, 0);
    
    // 发送区域
    groupBox_send = new QGroupBox("发送", centralWidget);
    verticalLayout_send = new QVBoxLayout(groupBox_send);
    
    horizontalLayout_sendOptions = new QHBoxLayout;
    checkBox_hexSend = new QCheckBox("十六进制发送", groupBox_send);
    checkBox_autoSend = new QCheckBox("自动发送", groupBox_send);
    spinBox_autoSendInterval = new QSpinBox(groupBox_send);
    spinBox_autoSendInterval->setMinimum(100);
    spinBox_autoSendInterval->setMaximum(10000);
    spinBox_autoSendInterval->setValue(1000);
    label_ms = new QLabel("毫秒", groupBox_send);
    
    horizontalLayout_sendOptions->addWidget(checkBox_hexSend);
    horizontalLayout_sendOptions->addWidget(checkBox_autoSend);
    horizontalLayout_sendOptions->addWidget(spinBox_autoSendInterval);
    horizontalLayout_sendOptions->addWidget(label_ms);
    
    plainTextEdit_send = new QPlainTextEdit(groupBox_send);
    plainTextEdit_send->setMaximumBlockCount(1000);
    
    horizontalLayout_sendButtons = new QHBoxLayout;
    pushButton_send = new QPushButton("发送", groupBox_send);
    pushButton_send->setDefault(true);
    pushButton_clearSend = new QPushButton("清空", groupBox_send);
    
    horizontalLayout_sendButtons->addWidget(pushButton_send);
    horizontalLayout_sendButtons->addWidget(pushButton_clearSend);
    
    verticalLayout_send->addLayout(horizontalLayout_sendOptions);
    verticalLayout_send->addWidget(plainTextEdit_send);
    verticalLayout_send->addLayout(horizontalLayout_sendButtons);
    
    gridLayout_main->addWidget(groupBox_send, 0, 1);
    
    mainLayout->addLayout(gridLayout_main);
    
    // 状态统计区域
    horizontalLayout_status = new QHBoxLayout;
    label_sendCount = new QLabel("发送: 0 字节", centralWidget);
    QSpacerItem *spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    label_receiveCount = new QLabel("接收: 0 字节", centralWidget);
    
    horizontalLayout_status->addWidget(label_sendCount);
    horizontalLayout_status->addItem(spacer);
    horizontalLayout_status->addWidget(label_receiveCount);
    
    mainLayout->addLayout(horizontalLayout_status);
    
    // 连接UI信号槽
    connect(pushButton_refresh, &QPushButton::clicked, this, &MainWindow::on_pushButton_refresh_clicked);
    connect(pushButton_open, &QPushButton::clicked, this, &MainWindow::on_pushButton_open_clicked);
    connect(pushButton_send, &QPushButton::clicked, this, &MainWindow::on_pushButton_send_clicked);
    connect(pushButton_clearReceive, &QPushButton::clicked, this, &MainWindow::on_pushButton_clearReceive_clicked);
    connect(pushButton_clearSend, &QPushButton::clicked, this, &MainWindow::on_pushButton_clearSend_clicked);
    connect(pushButton_save, &QPushButton::clicked, this, &MainWindow::on_pushButton_save_clicked);
    connect(checkBox_hexSend, &QCheckBox::stateChanged, this, &MainWindow::on_checkBox_hexSend_stateChanged);
    connect(checkBox_hexReceive, &QCheckBox::stateChanged, this, &MainWindow::on_checkBox_hexReceive_stateChanged);
    connect(checkBox_autoSend, &QCheckBox::stateChanged, this, &MainWindow::on_checkBox_autoSend_stateChanged);
    connect(comboBox_baudRate, &QComboBox::currentIndexChanged, this, &MainWindow::on_comboBox_baudRate_currentIndexChanged);
    connect(comboBox_dataBits, &QComboBox::currentIndexChanged, this, &MainWindow::on_comboBox_dataBits_currentIndexChanged);
    connect(comboBox_stopBits, &QComboBox::currentIndexChanged, this, &MainWindow::on_comboBox_stopBits_currentIndexChanged);
    connect(comboBox_parity, &QComboBox::currentIndexChanged, this, &MainWindow::on_comboBox_parity_currentIndexChanged);
    connect(comboBox_flowControl, &QComboBox::currentIndexChanged, this, &MainWindow::on_comboBox_flowControl_currentIndexChanged);
    connect(comboBox_sendCodec, &QComboBox::currentIndexChanged, this, &MainWindow::on_comboBox_sendCodec_currentIndexChanged);
    connect(comboBox_receiveCodec, &QComboBox::currentIndexChanged, this, &MainWindow::on_comboBox_receiveCodec_currentIndexChanged);
    // 保存设置的信号槽连接已移至构造函数中，在应用初始设置后连接
}

void MainWindow::updateSerialPorts()
{
    comboBox_portName->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        comboBox_portName->addItem(info.portName());
    }
}

void MainWindow::updateCodecList()
{
    // 常用编码列表，将这些编码排在前面
    QStringList commonCodecs = {"UTF-8", "GBK", "GB2312", "GB18030", "ASCII", "Latin-1", "UTF-16", "UTF-16BE", "UTF-16LE"};
    
    // 获取所有可用编码
    QList<QByteArray> allCodecBytes = QTextCodec::availableCodecs();
    QStringList allCodecs;
    for (const QByteArray &codecByte : allCodecBytes) {
        allCodecs.append(QString(codecByte));
    }
    
    // 创建最终编码列表，先添加常用编码
    QStringList finalCodecs;
    
    // 添加常用编码
    for (const QString &commonCodec : commonCodecs) {
        if (allCodecs.contains(commonCodec)) {
            finalCodecs.append(commonCodec);
            allCodecs.removeAll(commonCodec);
        }
    }
    
    // 添加剩余编码（排序后）
    allCodecs.sort();
    finalCodecs.append(allCodecs);
    
    comboBox_sendCodec->addItems(finalCodecs);
    comboBox_receiveCodec->addItems(finalCodecs);
    
    // 设置默认编码
    comboBox_sendCodec->setCurrentText("UTF-8");
    comboBox_receiveCodec->setCurrentText("UTF-8");
}

QString MainWindow::byteArrayToHexString(const QByteArray &data)
{
    QString hexString;
    for (const char &byte : data) {
        hexString += QString("%1 ").arg(static_cast<unsigned char>(byte), 2, 16, QLatin1Char('0')).toUpper();
    }
    return hexString.trimmed();
}

QByteArray MainWindow::hexStringToByteArray(const QString &hex)
{
    QByteArray byteArray;
    QStringList hexList = hex.split(' ', Qt::SkipEmptyParts);
    for (const QString &hexItem : hexList) {
        bool ok;
        uchar byte = hexItem.toUInt(&ok, 16);
        if (ok) {
            byteArray.append(static_cast<char>(byte));
        }
    }
    return byteArray;
}

QByteArray MainWindow::encodeData(const QString &text, const QString &codecName)
{
    // 使用QTextCodec实现真正的编码转换
    QTextCodec *codec = QTextCodec::codecForName(codecName.toUtf8());
    if (codec) {
        return codec->fromUnicode(text);
    }
    return text.toUtf8(); // 默认UTF-8
}

QString MainWindow::decodeData(const QByteArray &data, const QString &codecName)
{
    // 使用QTextCodec实现真正的解码转换
    QTextCodec *codec = QTextCodec::codecForName(codecName.toUtf8());
    if (codec) {
        return codec->toUnicode(data);
    }
    return QString::fromUtf8(data); // 默认UTF-8
}

void MainWindow::on_pushButton_refresh_clicked()
{
    updateSerialPorts();
}

void MainWindow::on_pushButton_open_clicked()
{
    if (serial->isOpen()) {
        // 关闭串口
        serial->close();
        pushButton_open->setText("打开");
        label_status->setText("串口未打开");
        label_status->setStyleSheet("color: red;");
    } else {
        // 打开串口
        serial->setPortName(comboBox_portName->currentText());
        serial->setBaudRate(comboBox_baudRate->currentText().toInt());
        
        // 设置数据位
        switch (comboBox_dataBits->currentText().toInt()) {
        case 5: serial->setDataBits(QSerialPort::Data5); break;
        case 6: serial->setDataBits(QSerialPort::Data6); break;
        case 7: serial->setDataBits(QSerialPort::Data7); break;
        case 8: serial->setDataBits(QSerialPort::Data8); break;
        default: serial->setDataBits(QSerialPort::Data8); break;
        }
        
        // 设置停止位
        switch (comboBox_stopBits->currentIndex()) {
        case 0: serial->setStopBits(QSerialPort::OneStop); break;
        case 1: serial->setStopBits(QSerialPort::OneAndHalfStop); break;
        case 2: serial->setStopBits(QSerialPort::TwoStop); break;
        default: serial->setStopBits(QSerialPort::OneStop); break;
        }
        
        // 设置校验位
        switch (comboBox_parity->currentIndex()) {
        case 0: serial->setParity(QSerialPort::NoParity); break;
        case 1: serial->setParity(QSerialPort::OddParity); break;
        case 2: serial->setParity(QSerialPort::EvenParity); break;
        case 3: serial->setParity(QSerialPort::MarkParity); break;
        case 4: serial->setParity(QSerialPort::SpaceParity); break;
        default: serial->setParity(QSerialPort::NoParity); break;
        }
        
        // 设置流控制
        switch (comboBox_flowControl->currentIndex()) {
        case 0: serial->setFlowControl(QSerialPort::NoFlowControl); break;
        case 1: serial->setFlowControl(QSerialPort::HardwareControl); break;
        case 2: serial->setFlowControl(QSerialPort::SoftwareControl); break;
        default: serial->setFlowControl(QSerialPort::NoFlowControl); break;
        }
        
        if (serial->open(QIODevice::ReadWrite)) {
            pushButton_open->setText("关闭");
            label_status->setText("串口已打开");
            label_status->setStyleSheet("color: green;");
        } else {
            label_status->setText("打开失败");
            label_status->setStyleSheet("color: red;");
        }
    }
}

void MainWindow::on_pushButton_send_clicked()
{
    if (!serial->isOpen()) {
        return;
    }
    
    QString text = plainTextEdit_send->toPlainText();
    QByteArray sendData;
    
    if (checkBox_hexSend->isChecked()) {
        sendData = hexStringToByteArray(text);
    } else {
        sendData = encodeData(text, comboBox_sendCodec->currentText());
    }
    
    qint64 bytesWritten = serial->write(sendData);
    if (bytesWritten > 0) {
        sendBytes += bytesWritten;
        label_sendCount->setText(QString("发送: %1 字节").arg(sendBytes));
        
        // 添加日志模式处理，显示发送数据
        if (checkBox_logMode->isChecked()) {
            QString logText;
            if (checkBox_hexSend->isChecked()) {
                logText = byteArrayToHexString(sendData);
            } else {
                logText = text;
            }
            
            // 添加日志前缀
            logText = "[TX] " + logText;
            
            // 添加时间戳
            if (checkBox_timestamp->isChecked()) {
                QDateTime currentTime = QDateTime::currentDateTime();
                QString timestamp = currentTime.toString("[yyyy-MM-dd HH:mm:ss] ");
                logText = timestamp + logText;
            }
            
            plainTextEdit_receive->insertPlainText(logText + "\n");
            plainTextEdit_receive->moveCursor(QTextCursor::End);
        }
    }
}

void MainWindow::readData()
{
    if (!serial->isOpen()) {
        return;
    }
    
    QByteArray data = serial->readAll();
    receiveBytes += data.size();
    label_receiveCount->setText(QString("接收: %1 字节").arg(receiveBytes));
    
    QString displayText;
    if (checkBox_hexReceive->isChecked()) {
        displayText = byteArrayToHexString(data);
    } else {
        displayText = decodeData(data, comboBox_receiveCodec->currentText());
    }
    
    // 添加日志模式处理
    if (checkBox_logMode->isChecked()) {
        displayText = "[RX] " + displayText;
    }
    
    // 添加时间戳
    if (checkBox_timestamp->isChecked()) {
        QDateTime currentTime = QDateTime::currentDateTime();
        QString timestamp = currentTime.toString("[yyyy-MM-dd HH:mm:ss] ");
        displayText = timestamp + displayText;
    }
    
    plainTextEdit_receive->insertPlainText(displayText + "\n");
    plainTextEdit_receive->moveCursor(QTextCursor::End);
}

void MainWindow::on_pushButton_clearReceive_clicked()
{
    plainTextEdit_receive->clear();
}

void MainWindow::on_pushButton_clearSend_clicked()
{
    plainTextEdit_send->clear();
}

void MainWindow::on_pushButton_save_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "保存接收数据", "./serial_data.txt", "文本文件 (*.txt);;所有文件 (*)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << plainTextEdit_receive->toPlainText();
            file.close();
        }
    }
}

void MainWindow::on_checkBox_hexSend_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    // 可以添加一些逻辑来处理十六进制发送状态变化
}

void MainWindow::on_checkBox_hexReceive_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    // 可以添加一些逻辑来处理十六进制接收状态变化
}

void MainWindow::on_checkBox_autoSend_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {
        int interval = spinBox_autoSendInterval->value();
        autoSendTimer->start(interval);
    } else {
        autoSendTimer->stop();
    }
}

void MainWindow::autoSendData()
{
    on_pushButton_send_clicked();
}

void MainWindow::on_comboBox_baudRate_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    // 波特率变化，在打开串口时会重新设置
}

void MainWindow::on_comboBox_dataBits_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    // 数据位变化，在打开串口时会重新设置
}

void MainWindow::on_comboBox_stopBits_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    // 停止位变化，在打开串口时会重新设置
}

void MainWindow::on_comboBox_parity_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    // 校验位变化，在打开串口时会重新设置
}

void MainWindow::on_comboBox_flowControl_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    // 流控制变化，在打开串口时会重新设置
}

void MainWindow::on_comboBox_sendCodec_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    // 发送编码变化
}

void MainWindow::on_comboBox_receiveCodec_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    // 接收编码变化
}

void MainWindow::saveSettings()
{
    // 使用与加载时相同的配置文件路径
    QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";
    QSettings settings(configPath, QSettings::IniFormat);
    settings.setValue("timestamp", checkBox_timestamp->isChecked());
    settings.setValue("logMode", checkBox_logMode->isChecked());
    settings.setValue("hexReceive", checkBox_hexReceive->isChecked());
    settings.setValue("hexSend", checkBox_hexSend->isChecked());
    settings.sync(); // 强制写入文件，确保设置立即保存
}
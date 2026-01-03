#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QtCore5Compat/QTextCodec>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QPlainTextEdit>
#include <QStatusBar>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_refresh_clicked();
    void on_pushButton_open_clicked();
    void on_pushButton_send_clicked();
    void on_pushButton_clearReceive_clicked();
    void on_pushButton_clearSend_clicked();
    void on_pushButton_save_clicked();
    void on_checkBox_hexSend_stateChanged(int arg1);
    void on_checkBox_hexReceive_stateChanged(int arg1);
    void on_checkBox_autoSend_stateChanged(int arg1);
    void on_comboBox_baudRate_currentIndexChanged(int index);
    void on_comboBox_dataBits_currentIndexChanged(int index);
    void on_comboBox_stopBits_currentIndexChanged(int index);
    void on_comboBox_parity_currentIndexChanged(int index);
    void on_comboBox_flowControl_currentIndexChanged(int index);
    void on_comboBox_sendCodec_currentIndexChanged(int index);
    void on_comboBox_receiveCodec_currentIndexChanged(int index);
    
    void readData();
    void autoSendData();
    void saveSettings();
    
private:
    QSerialPort *serial;
    QTimer *autoSendTimer;
    qint64 sendBytes;
    qint64 receiveBytes;
    
    // UI组件
    QGroupBox *groupBox_serialConfig;
    QGridLayout *gridLayout_serialConfig;
    QLabel *label_portName;
    QComboBox *comboBox_portName;
    QPushButton *pushButton_refresh;
    QPushButton *pushButton_open;
    QLabel *label_status;
    QLabel *label_baudRate;
    QComboBox *comboBox_baudRate;
    QLabel *label_dataBits;
    QComboBox *comboBox_dataBits;
    QLabel *label_stopBits;
    QComboBox *comboBox_stopBits;
    QLabel *label_parity;
    QComboBox *comboBox_parity;
    QLabel *label_flowControl;
    QComboBox *comboBox_flowControl;
    
    QGroupBox *groupBox_codecConfig;
    QHBoxLayout *horizontalLayout_codec;
    QLabel *label_sendCodec;
    QComboBox *comboBox_sendCodec;
    QLabel *label_receiveCodec;
    QComboBox *comboBox_receiveCodec;
    
    QGroupBox *groupBox_receive;
    QVBoxLayout *verticalLayout_receive;
    QHBoxLayout *horizontalLayout_receiveOptions;
    QCheckBox *checkBox_hexReceive;
    QCheckBox *checkBox_timestamp;
    QCheckBox *checkBox_logMode;
    QPushButton *pushButton_clearReceive;
    QPushButton *pushButton_save;
    QPlainTextEdit *plainTextEdit_receive;
    
    QGroupBox *groupBox_send;
    QVBoxLayout *verticalLayout_send;
    QHBoxLayout *horizontalLayout_sendOptions;
    QCheckBox *checkBox_hexSend;
    QCheckBox *checkBox_autoSend;
    QSpinBox *spinBox_autoSendInterval;
    QLabel *label_ms;
    QPlainTextEdit *plainTextEdit_send;
    QHBoxLayout *horizontalLayout_sendButtons;
    QPushButton *pushButton_send;
    QPushButton *pushButton_clearSend;
    
    QHBoxLayout *horizontalLayout_status;
    QLabel *label_sendCount;
    QLabel *label_receiveCount;
    
    void initUI();
    void updateSerialPorts();
    void updateCodecList();
    QString byteArrayToHexString(const QByteArray &data);
    QByteArray hexStringToByteArray(const QString &hex);
    QByteArray encodeData(const QString &text, const QString &codecName);
    QString decodeData(const QByteArray &data, const QString &codecName);
};
#endif // MAINWINDOW_H
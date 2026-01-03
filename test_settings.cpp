#include <QCoreApplication>
#include <QSettings>
#include <QDebug>
#include <QCheckBox>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    // 创建测试配置文件路径
    QString configPath = QCoreApplication::applicationDirPath() + "/test_config.ini";
    
    // 模拟程序启动时的设置加载
    qDebug() << "=== 测试程序启动时加载设置 ===";
    QSettings settings(configPath, QSettings::IniFormat);
    
    // 模拟UI组件
    QCheckBox checkBox_timestamp;
    QCheckBox checkBox_logMode;
    QCheckBox checkBox_hexReceive;
    QCheckBox checkBox_hexSend;
    
    // 应用默认设置
    bool timestamp = settings.value("timestamp", true).toBool();
    bool logMode = settings.value("logMode", true).toBool();
    bool hexReceive = settings.value("hexReceive", false).toBool();
    bool hexSend = settings.value("hexSend", false).toBool();
    
    qDebug() << "从配置文件加载的设置：";
    qDebug() << "timestamp: " << timestamp;
    qDebug() << "logMode: " << logMode;
    qDebug() << "hexReceive: " << hexReceive;
    qDebug() << "hexSend: " << hexSend;
    
    // 设置复选框状态
    checkBox_timestamp.setChecked(timestamp);
    checkBox_logMode.setChecked(logMode);
    checkBox_hexReceive.setChecked(hexReceive);
    checkBox_hexSend.setChecked(hexSend);
    
    qDebug() << "设置复选框状态后：";
    qDebug() << "timestamp: " << checkBox_timestamp.isChecked();
    qDebug() << "logMode: " << checkBox_logMode.isChecked();
    
    // 模拟用户修改设置
    qDebug() << "\n=== 测试用户修改设置 ===";
    checkBox_logMode.setChecked(false);
    
    // 模拟保存设置
    qDebug() << "\n=== 测试保存设置 ===";
    settings.setValue("timestamp", checkBox_timestamp.isChecked());
    settings.setValue("logMode", checkBox_logMode.isChecked());
    settings.setValue("hexReceive", checkBox_hexReceive.isChecked());
    settings.setValue("hexSend", checkBox_hexSend.isChecked());
    settings.sync();
    
    // 再次加载设置，验证保存是否成功
    qDebug() << "\n=== 测试重新加载设置 ===";
    QSettings settings2(configPath, QSettings::IniFormat);
    qDebug() << "timestamp: " << settings2.value("timestamp", true).toBool();
    qDebug() << "logMode: " << settings2.value("logMode", true).toBool();
    
    // 测试默认值功能
    qDebug() << "\n=== 测试默认值功能 ===";
    QSettings settings3("non_existent.ini", QSettings::IniFormat);
    qDebug() << "不存在的配置文件 - timestamp默认值: " << settings3.value("timestamp", true).toBool();
    qDebug() << "不存在的配置文件 - logMode默认值: " << settings3.value("logMode", true).toBool();
    
    qDebug() << "\n=== 测试完成 ===";
    return 0;
}
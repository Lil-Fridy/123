#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QTimer>
#include <QScopedPointer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void scanDevice();          // 扫描蓝牙/模拟设备
    void turnOnLight();         // 开灯（模拟）
    void turnOffLight();        // 关灯
    void clearData();           // 清空波形和数据
    void generateWaveData();    // 产生模拟压力数据（可替换为真实传感器）

private:
    QScopedPointer<Ui::MainWindow> ui;
    QScopedPointer<QtCharts::QLineSeries> series;
    QScopedPointer<QtCharts::QChart> chart;
    QScopedPointer<QtCharts::QValueAxis> axisX;
    QScopedPointer<QtCharts::QValueAxis> axisY;
    QScopedPointer<QTimer> waveTimer;

    static constexpr int MAX_POINTS = 60;      // 波形显示点数
    static constexpr int TRIGGER_THRESHOLD = 70; // 打击触发阈值
    int dataCount = 0;                         // 总采样计数
    double maxPressure = 0.0;                  // 本次会话最大压力
    int punchCount = 0;                        // 打击次数（超过阈值计一次）
    bool lastTriggered = false;                // 防抖动标志
};

#endif
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRandomGenerator>
#include <QMessageBox>
#include <QtCharts/QChartView>

using namespace QtCharts;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("智能打击训练器 Pro");
    setMinimumSize(380, 680);

    // ---------- 初始化图表 ----------
    series.reset(new QLineSeries());
    chart.reset(new QChart());
    chart->addSeries(series.data());
    chart->setTitle("实时压力波形");
    chart->legend()->hide();
    chart->setTheme(QChart::ChartThemeBlueCerulean);
    chart->setAnimationOptions(QChart::SeriesAnimations); // 平滑动画

    axisX.reset(new QValueAxis());
    axisY.reset(new QValueAxis());
    axisX->setRange(0, MAX_POINTS);
    axisX->setLabelFormat("%d");
    axisY->setRange(0, 100);
    axisY->setLabelFormat("%.0f");
    axisY->setTitleText("压力值");

    chart->addAxis(axisX.data(), Qt::AlignBottom);
    chart->addAxis(axisY.data(), Qt::AlignLeft);
    series->attachAxis(axisX.data());
    series->attachAxis(axisY.data());

    ui->graphicsView->setChart(chart.data());
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

    // ---------- 按钮信号连接 ----------
    connect(ui->scanBtn, &QPushButton::clicked, this, &MainWindow::scanDevice);
    connect(ui->lightOnBtn, &QPushButton::clicked, this, &MainWindow::turnOnLight);
    connect(ui->lightOffBtn, &QPushButton::clicked, this, &MainWindow::turnOffLight);
    connect(ui->clearBtn, &QPushButton::clicked, this, &MainWindow::clearData);

    // ---------- 波形定时器（模拟传感器数据）----------
    waveTimer.reset(new QTimer(this));
    connect(waveTimer.data(), &QTimer::timeout, this, &MainWindow::generateWaveData);
    waveTimer->start(200);  // 5Hz 更新，流畅且省电
}

void MainWindow::scanDevice()
{
    ui->listWidget->clear();
    ui->listWidget->addItem("🔍 正在扫描...");
    // 模拟延时扫描
    QTimer::singleShot(500, [this]() {
        ui->listWidget->clear();
        ui->listWidget->addItem("✅ ESP32_SmartPunch (模拟设备)");
        ui->listWidget->addItem("💡 蓝牙已就绪，可扩展真实硬件");
        QMessageBox::information(this, "扫描完成", "发现 1 个设备\n点击可连接（模拟模式）");
    });
}

void MainWindow::turnOnLight()
{
    ui->maxLab->setText("💡 峰值: 灯已开启");
    // 此处可发送蓝牙指令: sendCommand("LIGHT_ON");
}

void MainWindow::turnOffLight()
{
    ui->maxLab->setText("🌙 峰值: 灯已关闭");
}

void MainWindow::clearData()
{
    series->clear();
    dataCount = 0;
    maxPressure = 0.0;
    punchCount = 0;
    lastTriggered = false;
    ui->pressLab->setText("压力: 0.0");
    ui->maxLab->setText("峰值: 0.0");
    ui->countLab->setText("次数: 0");
    axisX->setRange(0, MAX_POINTS);
}

void MainWindow::generateWaveData()
{
    // 模拟压力值（范围 20~98，偶尔出现峰值）
    int rawValue = QRandomGenerator::global()->bounded(25, 85);
    // 随机产生 10% 概率的高峰（模拟打击）
    if (QRandomGenerator::global()->bounded(100) < 15)
        rawValue += QRandomGenerator::global()->bounded(15, 35);
    int value = qBound(0, rawValue, 100);

    // 更新波形（滚动窗口）
    series->append(dataCount, value);
    if (series->count() > MAX_POINTS) {
        series->remove(0);
    }
    // 动态调整 X 轴范围，实现滚动
    if (dataCount >= MAX_POINTS) {
        axisX->setRange(dataCount - MAX_POINTS, dataCount);
    }

    // 更新压力显示
    ui->pressLab->setText(QString("📊 压力: %1").arg(value));

    // 更新峰值
    if (value > maxPressure) {
        maxPressure = value;
        ui->maxLab->setText(QString("🏆 峰值: %1").arg(maxPressure));
    }

    // 打击检测（上升沿超过阈值，且防抖）
    bool nowTriggered = (value >= TRIGGER_THRESHOLD);
    if (nowTriggered && !lastTriggered) {
        punchCount++;
        ui->countLab->setText(QString("🥊 次数: %1").arg(punchCount));
        // 可选：播放触感反馈（iOS 支持震动）
        #ifdef Q_OS_IOS
        // 可调用震动 API，此处略
        #endif
    }
    lastTriggered = nowTriggered;

    dataCount++;
}

MainWindow::~MainWindow() = default;
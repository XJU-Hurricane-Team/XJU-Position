#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QMainWindow>
#include <QPainter>
#include <QString>
#include <QTextStream>
#include <QTimer>
#include <QWidget>
#include <QtCharts>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

#define SEND_PREFIX "ACT"

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

  public slots:

    void resizeEvent(QResizeEvent *event);

    /* Serial port events. */
    void searchSerialPort();
    void serialRead();
    void parseSerialByte(uint8_t byte);

    /* Button clicked events. */
    void on_switchSerialClicked();
    void on_setXClicked();
    void on_setYClicked();
    void on_setYawClicked();
    void on_setFreqClicked();
    void on_setZeroClicked();
    void on_setCaliGyroClicked();
    void on_setCaliEncoderClicked();
    void on_recordClicked();
    void on_showGraphClicked();

    /* Using Timer to draw graph. */
    void drawTimeHandler();
    void editDrawTime();
    void on_changeVisibleClicked();
    void on_pauseDrawClicked();

  private:
    Ui::MainWindow *ui;

    struct {
        float yaw;     /*!< Yaw angle */
        float pitch;   /*!< Pitch angle */
        float roll;    /*!< Roll angle */
        float posX;    /*!< Position X */
        float posY;    /*!< Position Y */
        float yawGyro; /*!< Yaw angle speed */
    } __attribute__((packed)) recvData;

    bool recvComplete;
    QSerialPort *serialPort;
    QFile *recordFile;
    QTextStream *recordContent;

    QChart *chart;
    QValueAxis *axisTime;
    QValueAxis *axisPos;
    QValueAxis *axisAngle;
    QTimer drawTimer;
    /* x, y, yaw, pitch, roll, yawGyro */
    QSplineSeries *series[6];
    float graphTime;
    int minTimeIndex;
};

#endif /* MAINWINDOW_H */

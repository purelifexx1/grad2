#ifndef CALIB_H
#define CALIB_H

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <QTime>
#include <QThread>
#include <QPixmap>
#include <QDebug>

#define ID_CAMERA 0

class Calib: public QThread
{
    Q_OBJECT
public:
    Calib(QObject *parent = nullptr);
    QPixmap frame() const
    {
        return calibFrame;
    }
    std::vector<std::vector<double>> buffer;
    std::vector<std::vector<double>> Trans_buffer;
    std::vector<std::vector<int>> org_point; // 0: notFoundCount, 1: Found, 2: x pixel, 3: y pixel
    bool Set;
    double precTick, ticks, dT_1 = 0 ;
    double dT;


signals:
    void newPixmapCaptured();
protected:
    void run() override;
private:
    QTime m_time;
    QPixmap calibFrame;
    cv::Mat mFrame;


};

#endif // CALIB_H

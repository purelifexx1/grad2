#include "calib.h"

Calib::Calib(QObject *parent)
    :QThread { parent }
{

}


using namespace cv;
using namespace dnn;
using namespace std;


Mat cameraMatrix = (Mat_<double>(3,3) <<   701.9020,        0, 325.9663,
                                       0, 693.9010,  148.046,
                                       0,        0,        1);

Mat distCoeffs = (Mat_<double>(1,5) <<   -0.0815, 1.2361, -0.0116, 0.0135, -4.3423);
UMat U_cameraMatrix = cameraMatrix.getUMat(ACCESS_READ);
UMat U_distCoeffs = distCoeffs.getUMat(ACCESS_READ);

void Transfer_CorRobot(float x_cam, float y_cam, float z_cam,
                      float &x_robot, float &y_robot, float &z_robot);

void Calib::run()
{

   while (true)
   {
        if ((!buffer.empty())&& (Set==1))
        {
            Set = 0;
            m_time.start();
            Trans_buffer.clear();
            for (size_t i = 0; i < buffer.size(); ++i)
            {

                //Set distort center point
                Mat_<Point2f> center_org(1,1), center_undis(1,1);
                center_org(0) = Point2f(buffer[i][1],buffer[i][2]);
                //undistort
                undistortPoints(center_org, center_undis, U_cameraMatrix, U_distCoeffs, noArray(), U_cameraMatrix);
                //cout<<center_undis<<endl;
                //Transfer to Coordinate of Robot
                //with z = 1
                float x_robot, y_robot, z_robot = 0;
                Transfer_CorRobot(center_undis(0).x,center_undis(0).y,1,
                                  x_robot,y_robot,z_robot);
                vector<double> buffer_tmp;
                buffer_tmp.push_back(buffer[i][0]);
                buffer_tmp.push_back(x_robot);
                buffer_tmp.push_back(y_robot);
                buffer_tmp.push_back(z_robot);
                buffer_tmp.push_back(buffer[i][3]);

                Trans_buffer.push_back(buffer_tmp);


            }



            time_h = (double)m_time.elapsed();

            if (!Trans_buffer.empty())
            {
                emit newPixmapCaptured();
            }

        }

    }
}


void Transfer_CorRobot(float x_cam, float y_cam, float z_cam,
                      float &x_robot, float &y_robot, float &z_robot)
{
    //Tranfer Matrix
    float A[4][4] =
      {
        {     1,     0,     0,      238 },
        {     0,     0,    -1,     -303 },
        {     0,     1,     0,     145.7},
        {     0,     0,     0,        1 },
      };
     // Position of Object in Camera Coordinate
    float B[4][1]=
      {
        {x_cam},
        {y_cam},
        {z_cam},
        {1}
      };
    // Multiply 2 Matrix : C = A*B
      float C[4][1] ;
      int i,j,k;
      for(i=0;i<4;i++)
        for(j=0;j<1;j++)
        {
          C[i][j]=0;
          for(k=0;k<4;k++)
          {
            C[i][j]+=A[i][k]*B[k][j];
          }
        }
      //Object Position in Robot Coordinate
      x_robot = C[0][0];
      y_robot = C[1][0];
      z_robot = C[2][0];
}

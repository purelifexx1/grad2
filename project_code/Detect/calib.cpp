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

float z_cam = 290 ;//z camera = 287 mm
float scale = (16.0/44.685);

//Condition for receive center point
double R_0 = 70;
// Set multi Kalman Filter
int numberOfObject = 3;
vector<KalmanFilter> KF;
vector<Mat> state;
vector<Mat> meas;

void Transfer_CorRobot(float x_cam, float y_cam, float z_cam,
                      float &x_robot, float &y_robot, float &z_robot);

void Calib::run()
{
   for (int i = 0; i < numberOfObject; i++)
   {
       //Set Kalman Filter
       KF.push_back(KalmanFilter(4, 2, 0, CV_32F));
       state.push_back(Mat(4, 1, CV_32F));  // [x,y,v_x,v_y]
       meas.push_back(Mat(2, 1, CV_32F));   // [z_x,z_y]
   }
   for (size_t i = 0; i < 3; i++)
   {
       // Transition State Matrix A
       // Note: set dT at each processing step!
           // [ 1 0 dT 0  ]
           // [ 0 1 0  dT ]
           // [ 0 0 1  0  ]
           // [ 0 0 0  1  ]
       setIdentity(KF[i].transitionMatrix);
       // Measure Matrix H
       // [ 1 0 0 0 ]
       // [ 0 1 0 0 ]
       setIdentity(KF[i].measurementMatrix);
       // Process Noise Covariance Matrix Q
       setIdentity(KF[i].processNoiseCov, cv::Scalar::all(1e-0));
       // Measures Noise Covariance Matrix R
       KF[i].measurementNoiseCov.at<float>(0) = 30*1e-0;
       KF[i].measurementNoiseCov.at<float>(3) = 5*1e-0;// The smaller scale value, the faster it change

   }
   while (true)
   {
       if (Set==true)
        {
           //Predict with Kalman Filter
           for (size_t i = 0; i < org_point.size(); ++i)
           {
               if(org_point[i][1] != 0)   // if Found == true
               {
                   // >>>> Matrix A
                   KF[i].transitionMatrix.at<float>(2) = dT;
                   KF[i].transitionMatrix.at<float>(7) = dT;
                   state[i] = KF[i].predict();
                   //cout << "X_predict = " << state[i].at<float>(0) << "Y_predict = " << state[i].at<float>(1)<< endl;
                   //Update Trans_buffer
                   Trans_buffer[i][1] = state[i].at<float>(0);
                   Trans_buffer[i][2] = state[i].at<float>(1);
                   //cout<<"X_pre"<<i<<"= "<<Trans_buffer[i][1]<<"Y_pre = "<<Trans_buffer[i][2]<<endl;
               }
           }


           if (!buffer.empty())
           {
               if (org_point.size() < buffer.size())
               {
                   org_point.clear();
                   for (size_t i = 0; i < buffer.size(); ++i)
                   {
                       vector<int> tmp_point;
                       tmp_point.push_back(0);
                       tmp_point.push_back(0);
                       tmp_point.push_back(int(buffer[i][1]));
                       tmp_point.push_back(int(buffer[i][2]));
                       org_point.push_back(tmp_point);
                       Trans_buffer.clear();
                   }
               }
               else if (org_point.size() > buffer.size())
               {
                   org_point.clear();
                   for (size_t i = 0; i < buffer.size(); ++i)
                   {
                       vector<int> tmp_point;
                       tmp_point.push_back(0);
                       tmp_point.push_back(0);
                       tmp_point.push_back(int(buffer[i][1]));
                       tmp_point.push_back(int(buffer[i][2]));
                       org_point.push_back(tmp_point);
                       Trans_buffer.clear();
                   }
               }
               else
               {
                   //Trans_buffer.clear();
                   Set = false;
                    // Set notFoundCount
                   for (size_t i = 0; i < org_point.size(); ++i)
                   {
                       org_point[i][0]  = 0; // notFoundCount = 0

                   }

                   for (size_t i = 0; i < buffer.size(); ++i)
                   {
                       float R = sqrt((buffer[i][1] - org_point[i][2])*(buffer[i][1] - org_point[i][2])
                               + (buffer[i][2] - org_point[i][3])*(buffer[i][2] - org_point[i][3]));
                       org_point[i][2] = buffer[i][1];
                       org_point[i][3] = buffer[i][2];
                       //cout<< "R =" << R << endl;
                       if ((R < R_0) && (-140 < buffer[i][1]) && (buffer[i][1] < 140))
                       {
                           //cout<< "R =" << i << endl;
                           //Set distort center point
                           Mat_<Point2f> center_org(1,1), center_undis(1,1);
                           center_org(0) = Point2f(buffer[i][1],buffer[i][2]);
                           //undistort
                           undistortPoints(center_org, center_undis, U_cameraMatrix, U_distCoeffs, noArray(), U_cameraMatrix);
                           //cout<<center_undis<<endl;
                           //Transfer to Coordinate of Robot
                           //with z = 290 mm
                           float x_robot, y_robot, z_robot = 0;
                           float x_cam = (center_undis(0).x)*scale;
                           float y_cam = (center_undis(0).y)*scale;
                           Transfer_CorRobot(x_cam,y_cam,z_cam,
                                             x_robot,y_robot,z_robot);

                           // Add in measurement matrix of Kalman Filter
                           meas[i].at<float>(0) = x_robot;
                           meas[i].at<float>(1) = y_robot;

                           // Add in buffer
                           /*vector<double> buffer_tmp;
                           buffer_tmp.push_back(buffer[i][0]);
                           buffer_tmp.push_back(x_robot);
                           buffer_tmp.push_back(y_robot);
                           buffer_tmp.push_back(z_robot);
                           buffer_tmp.push_back(buffer[i][3]);
                           Trans_buffer.push_back(buffer_tmp);*/
                           //cout<<" X = "<< x_robot<< " Y = "<< y_robot <<" Z = "<<z_robot<<endl;

                           // Set the Initialization for Kalman Filter
                           if (org_point[i][1] == 0) // if Found == false
                           {
                               setIdentity(KF[i].errorCovPost);
                               state[i].at<float>(0) = meas[i].at<float>(0);
                               state[i].at<float>(1) = meas[i].at<float>(1);
                               state[i].at<float>(2) = 0;
                               state[i].at<float>(3) = 0;
                               // <<<< Initialization
                               KF[i].statePost = state[i];
                               org_point[i][1] = 1;         // Found = true
                               // Trans_buffer
                               vector<double> buffer_tmp;
                               buffer_tmp.push_back(buffer[i][0]);
                               buffer_tmp.push_back(x_robot);
                               buffer_tmp.push_back(y_robot);
                               buffer_tmp.push_back(z_robot);
                               buffer_tmp.push_back(buffer[i][3]);
                               Trans_buffer.push_back(buffer_tmp);
                           }
                           else
                               KF[i].correct(meas[i]);
                               //Trans_buffer
                               Trans_buffer[i][0] = buffer[i][0];
                               Trans_buffer[i][4] = buffer[i][3];
                       }
                   }
               }

           }
           else
           {
               //Check not found object
               for (size_t i = 0; i < org_point.size(); ++i)
               {
                   org_point[i][0] ++; // notFoundCount ++
                   if( org_point[i][0] >= 20 )
                   {
                       org_point[i][1] = 0; // Found = false
                       Trans_buffer.clear();
                   }
               }
           }
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
    {     -1,     0,    0,      327.3 },
    {     0,     1,     0,      150.7 },
    {     0,     0,     -1,     416.5 },
    {     0,     0,     0,          1 },
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

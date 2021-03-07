#include "detect.h"

detect::detect(QObject *parent)
    :QThread { parent }
    , mVideoCap{ ID_CAMERA}
{

}

using namespace cv;
using namespace dnn;
using namespace std;

// Initialize the parameters
float confThreshold = 0.6; // Confidence threshold
float nmsThreshold = 0.4;  // Non-maximum suppression threshold
int inpWidth = 416;  // Width of network's input image
int inpHeight = 416; // Height of network's input image
vector<string> classes;

// Remove the bounding boxes with low confidence using non-maxima suppression
void postprocess(Mat& frame, const vector<Mat>& out, vector<vector<double>>& buffer);

// Draw the predicted bounding box
void drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat& frame,
              Point& center, double& theta);

//Image Processing in Croped Image and Calculate Theta
void imageProcess(Mat input, Mat& output, int x_crop, int y_crop, Point& center_final, double& theta_final);

// Get the names of the output layers
vector<String> getOutputsNames(const Net& net);


void detect::run()
{
    if (mVideoCap.isOpened())
    {
        // Load names of classes
        string classesFile = "model/obj.names";     //Path of file .names
        ifstream ifs(classesFile.c_str());
        string line;
        while (getline(ifs, line)) classes.push_back(line);



        // Give the configuration and weight files for the model
        string modelConfiguration = "model/yolov3prn-obj.cfg";
        string modelWeights = "model/yolov3prn-obj_best.weights";


        // Load the network
        Net net = readNetFromDarknet(modelConfiguration, modelWeights);
        net.setPreferableBackend(DNN_BACKEND_DEFAULT);
        net.setPreferableTarget(DNN_TARGET_OPENCL);
        while (true)
        {
            mVideoCap >> mFrame;

            U_mFrame = mFrame.getUMat(ACCESS_READ) ;
            resize(U_mFrame,U_mFrame_resize,Size(416,416));

            //mFrame = imread("D:/Downloads/YASKAWA/LV/Code/50.jpg");
            // Create a 4D blob from a frame.

            blobFromImage(U_mFrame_resize, blob, 1/255.0, cv::Size(inpWidth, inpHeight), Scalar(0,0,0), true, false);

            //Sets the input to the network
            net.setInput(blob);

            // Runs the forward pass to get output of the output layers

            vector<Mat> outs;
            net.forward(outs, getOutputsNames(net));


            //cout<<"Width "<<mFrame.cols<< "Height"<<mFrame.rows<< endl;
            // Remove the bounding boxes with low confidence
            m_time.start();
            buffer.clear();
            postprocess(mFrame, outs, buffer);

            // Put efficiency information. The function getPerfProfile returns the overall time for inference(t) and the timings for each of the layers(in layersTimes)
            //vector<double> layersTimes;
            //double freq = getTickFrequency() ;
            //double t = freq/net.getPerfProfile(layersTimes)  ;
            //string label = format("FPS : %.2f ", t);

            string label = format("FPS : %f ", (double)m_time.elapsed());
            putText(mFrame, label, Point(0, 15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255));

            if (!mFrame.empty())
            {
                mPixmap = cvMatToQPixmap(mFrame);
                emit newPixmapCaptured();
            }
        }
    }
}

// Remove the bounding boxes with low confidence using non-maxima suppression
void postprocess(Mat& frame, const vector<Mat>& outs, vector<vector<double>>& buffer)
{
    vector<int> classIds;
    vector<float> confidences;
    vector<Rect> boxes;

    for (size_t i = 0; i < outs.size(); ++i)
    {
        // Scan through all the bounding boxes output from the network and keep only the
        // ones with high confidence scores. Assign the box's class label as the class
        // with the highest score for the box.
        float* data = (float*)outs[i].data;
        for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
        {
            Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
            Point classIdPoint;
            double confidence;
            // Get the value and location of the maximum score
            minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
            if (confidence > confThreshold)
            {
                int centerX = (int)(data[0] * frame.cols);
                int centerY = (int)(data[1] * frame.rows);
                int width = (int)(data[2] * frame.cols);
                int height = (int)(data[3] * frame.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;

                classIds.push_back(classIdPoint.x);
                confidences.push_back((float)confidence);
                boxes.push_back(Rect(left, top, width, height));
            }
        }
    }

    // Perform non maximum suppression to eliminate redundant overlapping boxes with
    // lower confidences
    vector<int> indices;
    NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);
    for (size_t i = 0; i < indices.size(); ++i)
    {
        vector<double> buffer_tmp;
        Point center;
        double theta;
        int idx = indices[i];
        Rect box = boxes[idx];
        drawPred(classIds[idx], confidences[idx], box.x, box.y,
                 box.x + box.width, box.y + box.height, frame, center, theta);
        //Add data
        buffer_tmp.push_back(classIds[idx]);
        buffer_tmp.push_back(center.x);
        buffer_tmp.push_back(center.y);
        buffer_tmp.push_back(theta);

        buffer.push_back(buffer_tmp);
    }
}

// Draw the predicted bounding box
void drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat& frame,
              Point& center, double& theta)
{
    //Cut Image
    Mat imageCrop;
    Rect2d r;
    r.x = left ;
    r.y = top  ;
    r.width = right - left  ;  //right-left is %d //r.width  is float
    r.height = bottom - top ;

    if ((r.x >= 0) && (r.y >= 0)
            && (r.width >= 0) && (r.height >= 0)
            && (r.x + r.width <= frame.cols) &&  (r.y + r.height <= frame.rows))
    {
        imageCrop = frame(r);
    }

    //Image Processing in Croped Image and Calculate Theta
    if (!imageCrop.empty()){
    imageProcess(imageCrop,frame,r.x,r.y, center, theta);
    }
    //Draw a rectangle displaying the bounding box
    rectangle(frame, Point(left, top), Point(right, bottom), Scalar(255, 178, 50), 3);

    //Get the label for the class name and its confidence
    string label = format("%.2f", conf);
    if (!classes.empty())
    {
        CV_Assert(classId < (int)classes.size());
        label = classes[classId] + ":" + label;
    }

    //Display the label at the top of the bounding box
    int baseLine;
    Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
    top = max(top, labelSize.height);
    rectangle(frame, Point(left, top - round(1.5*labelSize.height)), Point(left + round(1.5*labelSize.width), top + baseLine), Scalar(255, 255, 255), FILLED);
    putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,0),1);
}
//Image Processing in Croped Image and Calculate Theta
void imageProcess(Mat input, Mat& output, int x_crop, int y_crop, Point& center_final, double& theta_final)
{
    Mat hsv, hsv_changed[3],s_image; UMat blur, th, edges;
    //Split S channel in HSV image
    cvtColor(input,hsv, COLOR_BGR2HSV);
    split(hsv,hsv_changed);
    s_image = hsv_changed[1];
    UMat u_s_image = s_image.getUMat(ACCESS_RW);
    //Blur and Threshold
    GaussianBlur(u_s_image, blur, Size(3, 3), 0);
    threshold(blur, th, 0, 255, THRESH_BINARY + THRESH_OTSU);
    //Find edges
    Canny(th, edges, 66, 133, 3);
    circle(edges,Point(input.cols/2,input.rows/2),3,Scalar(255, 255, 255),-1);
    //Find Contour
    vector<vector<Point> > contours;
    findContours(edges, contours, RETR_LIST, CHAIN_APPROX_SIMPLE );

    vector<vector<Point> > hull( contours.size() );
    vector<Point> center;
    vector<double>theta_edges(4);
    vector<double>theta_total;
    double pi = atan2(1,1)*4;
    for( size_t i = 0; i< contours.size(); i++ ) // iterate through each contour.
    {

        if (contourArea(contours[i]) > 1000)  //remove small areas like noise etc
        {
            convexHull(contours[i], hull[i],false);
            approxPolyDP(hull[i], hull[i],  0.1*(arcLength(hull[i], true)), true);
            if (hull[i].size() == 4)
            {
                //Convert coordinates in crop image to original image
                for (size_t j = 0 ; j< hull[i].size(); j++)
                {
                    hull[i][j].x += x_crop;
                    hull[i][j].y += y_crop;
                }
                // Draw Contours and center
                drawContours(output, hull, i, Scalar(0, 255, 0),1 );
                center.push_back((Point(hull[i][0] + hull[i][1] + hull[i][2] + hull[i][3])/4)) ;
                //Calculate Theta
                theta_edges[0] = (-atan2((hull[i][1].y - hull[i][0].y),(hull[i][1].x-hull[i][0].x))/pi * 180);
                theta_edges[1] = (-atan2((hull[i][1].y - hull[i][2].y), (hull[i][1].x - hull[i][2].x)) / pi * 180);
                theta_edges[2] = (-atan2((hull[i][3].y - hull[i][2].y), (hull[i][3].x - hull[i][2].x)) / pi * 180);
                theta_edges[3] = (-atan2((hull[i][3].y - hull[i][0].y), (hull[i][3].x - hull[i][0].x)) / pi * 180);

                double sum = 0;
                for( size_t i = 0; i< theta_edges.size(); i++ )
                {
                    if (theta_edges[i] >= 135)
                        theta_edges[i] = theta_edges[i] - 180;
                    else if (theta_edges[i] <= -135 )
                        theta_edges[i] = theta_edges[i] + 180;
                    else if (theta_edges[i] >= 45)
                        theta_edges[i] = theta_edges[i] - 90;
                    else if (theta_edges[i] <= -45)
                        theta_edges[i] = theta_edges[i] + 90;
                    sum += theta_edges[i];
                }
                theta_total.push_back(sum/4);
            }
        }
    }
    //Calculate mean center point
    center_final = Point(0,0);
    theta_final = 0;
    if (center.size()>0)
    {
        for( size_t i = 0; i< center.size(); i++ )
        {
            center_final += center[i];
            theta_final += theta_total[i];
        }

        center_final.x = int(center_final.x/center.size());
        center_final.y = int(center_final.y/center.size());
        circle(output,center_final,1,Scalar(0, 255, 0),2);
        theta_final = theta_final/theta_total.size();
    }


}



// Get the names of the output layers
vector<String> getOutputsNames(const Net& net)
{
    static vector<String> names;
    if (names.empty())
    {
        //Get the indices of the output layers, i.e. the layers with unconnected outputs
        vector<int> outLayers = net.getUnconnectedOutLayers();

        //get the names of all the layers in the network
        vector<String> layersNames = net.getLayerNames();

        // Get the names of the output layers in names
        names.resize(outLayers.size());
        for (size_t i = 0; i < outLayers.size(); ++i)
        names[i] = layersNames[outLayers[i] - 1];
    }
    return names;
}

QImage detect:: cvMatToQImage( const cv::Mat &inMat )  //MyVideoCapture
   {
      switch ( inMat.type() )
      {
         // 8-bit, 4 channel
         case CV_8UC4:
         {
            QImage image( inMat.data,
                          inMat.cols, inMat.rows,
                          static_cast<int>(inMat.step),
                          QImage::Format_ARGB32 );

            return image;
         }

         // 8-bit, 3 channel
         case CV_8UC3:
         {
            QImage image( inMat.data,
                          inMat.cols, inMat.rows,
                          static_cast<int>(inMat.step),
                          QImage::Format_RGB888 );

            return image.rgbSwapped();
         }

         // 8-bit, 1 channel
         case CV_8UC1:
         {
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
            QImage image( inMat.data,
                          inMat.cols, inMat.rows,
                          static_cast<int>(inMat.step),
                          QImage::Format_Grayscale8 );
#else
            static QVector<QRgb>  sColorTable;

            // only create our color table the first time
            if ( sColorTable.isEmpty() )
            {
               sColorTable.resize( 256 );

               for ( int i = 0; i < 256; ++i )
               {
                  sColorTable[i] = qRgb( i, i, i );
               }
            }

            QImage image( inMat.data,
                          inMat.cols, inMat.rows,
                          static_cast<int>(inMat.step),
                          QImage::Format_Indexed8 );

            image.setColorTable( sColorTable );
#endif

            return image;
         }

         default:
            qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
            break;
      }

      return QImage();
   }

QPixmap detect:: cvMatToQPixmap( const cv::Mat &inMat )  //MyVideoCapture
   {
      return QPixmap::fromImage( cvMatToQImage( inMat ) );
   }

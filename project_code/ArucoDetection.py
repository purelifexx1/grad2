import cv2
import numpy as np
# Create mark
#dictionary = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_6X6_250)
#markerImage = cv2.aruco.drawMarker(dictionary,25,200, borderBits= 1)
#cv2.imshow('Marker',markerImage)
#cv2.imwrite("marker25.png", markerImage);
#cv2.waitKey(0)

parameters = cv2.aruco.DetectorParameters_create()
dictionary = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_6X6_250)

mtx = np.zeros(shape=(3,3))
mtx[0,0] = 709.610699
mtx[0,2] = 309.137357
mtx[1,1] = 945.254236
mtx[1,2] = 209.405394
mtx[2,2] = 1

dist = np.zeros(shape=(1,5))
dist[0,0] = -1.061549e-01
dist[0,1] = 2.486004e+00
dist[0,2] = -1.159369e-02
dist[0,3] = 7.784208e-03
dist[0,4] = -8.79829e+00


cap = cv2.VideoCapture(0)
cap.set(3,640)
cap.set(4,480)
center = []
while(True):
    # Capture frame-by-frame
    ret, frame = cap.read()

    markerCorners, markerIds, rejectedCandidates = cv2.aruco.detectMarkers(frame, dictionary,
                                                                           parameters=parameters)
    rvecs, tvecs, objPoints = cv2.aruco.estimatePoseSingleMarkers(markerCorners, 0.055, mtx, dist)
    outputImage = frame
    if markerIds is not None:
        outputImage = cv2.aruco.drawDetectedMarkers(outputImage, markerCorners, markerIds)
        for i in range(0,len(markerIds)):


            outputImage = cv2.aruco.drawAxis(outputImage , mtx, dist, rvecs[i], tvecs[i], 0.1)

            rot_mat = cv2.Rodrigues(rvecs[i])
            rot_mat_t = rot_mat[0]
            camWorldE = (rot_mat_t[0][0]*0.0275,rot_mat_t[1][0]*0.0275,rot_mat_t[2][0]*0.0275)
            theta = -np.arctan2(camWorldE[1], camWorldE[0])/np.pi * 180
            label = 'x = %.2f, y = %.2f, z = %.2f, %.2f o' % (tvecs[i][0][0]*100,tvecs[i][0][1]*100,tvecs[i][0][2]*100,theta)
            cv2.putText(outputImage, label, (0, 15), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255))
    # Display the resulting frame
    cv2.imshow('Output',outputImage)

    key = cv2.waitKey(33)
    if key == ord('c'):
        center.append((tvecs[i][0][0],tvecs[i][0][1]))
    elif key == ord('r'):
        if (len(center) > 1):
            theta_final = 0
            for i in range(1,len(center)):
                theta_final += (-np.arctan2((center[0][1]-center[i][1]), (center[0][0]-center[i][0])) / np.pi * 180)
            theta_final = theta_final/(len(center)-1)
            print("theta_fianl = ",(theta_final+90))
    elif key == ord('v'):
        center.clear()
    elif key == ord('q'):
        break

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()
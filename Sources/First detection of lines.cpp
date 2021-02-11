#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>


using namespace cv;
using namespace std;

int main()
{
    //read the image file
    Mat img = imread("./Images/parking3.jpg");

    if (img.empty()) // Check for failure
    {
        cout << "Could not open or find the image" << endl;
        system("pause"); //wait for any key press
        return -1;
    }
    
    // Convert to gray-scale
    Mat greyMat;
    cvtColor(img, greyMat, cv::COLOR_RGB2GRAY);

    // Store the edges   
    Mat FirstEdges;

    // Find the edges in the image using canny detector
    Canny(greyMat, FirstEdges, 200, 255);

    // Create a vector to store lines of the image
    vector<Vec4i> FirstLines;

    // Apply First Hough Transform
    HoughLinesP(FirstEdges, FirstLines, 1, CV_PI / 180, 100, 20, 20);
    for (size_t i = 0; i < FirstLines.size(); i++) {

        Vec4i l = FirstLines[i];
        
        line(greyMat, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 0, 0), 3, LINE_AA);
    }

    // image in black and white only with the lines detected + random white pixels
    Mat BW_mat;
    // to erase the cars and keep only the whites lines + random white pixels
    threshold(greyMat, BW_mat, 254, 255, THRESH_BINARY);
    // to erase random white pixels
    erode(BW_mat, BW_mat, Mat());
    // to get beautiful lines
    dilate(BW_mat, BW_mat, Mat(), Point(-1,-1),3);


    // Store the edges   
    Mat SecondEdges;
    // Create a vector to store lines of the image
    vector<Vec4i> SecondLines;

    // Apply Second Hough Transform
    HoughLinesP(SecondEdges, SecondLines, 1, CV_PI / 180, 100, 20, 20);
    for (size_t i = 0; i < SecondLines.size(); i++) {

        Vec4i l = SecondLines[i];

        line(BW_mat, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 0, 0), 3, LINE_AA);
        circle(BW_mat, Point(l[0], l[1]), 2, Scalar(255,0,0), 10);
        circle(BW_mat, Point(l[2], l[3]), 2, Scalar(255,0,0), 10);
    }

    imshow("img", img);
    imshow("GREY", greyMat);
    imshow("Image parking", BW_mat);
        


    //imshow("Mon Parking Jacky", img);

    
    waitKey(0); // Wait for any keystroke in the window
    return 0;
}
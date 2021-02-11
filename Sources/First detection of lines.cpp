#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>


using namespace cv;
using namespace std;

int main()
{
    //read the image file
    Mat img = imread("./Images/parking1.jpg");

    if (img.empty()) // Check for failure
    {
        cout << "Could not open or find the image" << endl;
        system("pause"); //wait for any key press
        return -1;
    }

    // Convert to gray-scale
    Mat greyMat, colorMat;
    cvtColor(img, greyMat, cv::COLOR_RGB2GRAY);

    // Store the edges   
    Mat edges;

    // Find the edges in the image using canny detector
    Canny(greyMat, edges, 200, 255);
    //cout << edges << endl;
    // Create a vector to store lines of the image
    vector<Vec4i> lines;
    
    // Apply Hough Transform
    HoughLinesP(edges, lines, 1, CV_PI / 180, 100, 20, 20);
    for (size_t i = 0; i < lines.size(); i++) {

        Vec4i l = lines[i];

        line(greyMat, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 0, 0), 3, LINE_AA);
    }
    imshow("Image parking", greyMat);
        


    //imshow("Mon Parking Jacky", img);
    waitKey(0); // Wait for any keystroke in the window

    return 0;
}
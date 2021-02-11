#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>


using namespace cv;
using namespace std;



int  dist(int x, int y)
{
    int res = x - y;

    if (res > 0)
    {
        return res;
    }
    else
    {
        return -res;
    }
}



int main()
{
    //read the image file
    Mat img = imread("C:/Users/Julie/Pictures/Images_OpenCV/parking9.png");

    if (img.empty()) // Check for failure
    {
        cout << "Could not open or find the image" << endl;
        system("pause"); //wait for any key press
        return -1;
    }

    //------Read Pictures with lines-----///

    // Convert to gray-scale
    Mat greyMat, colorMat;
    cvtColor(img, greyMat, cv::COLOR_RGB2GRAY);

    // Store the edges   
    Mat FirstEdges;

    // Find the edges in the image using canny detector
    Canny(greyMat, FirstEdges, 200, 255);
    //cout << edges << endl;

    // Create a vector to store lines of the image
    vector<Vec4i> FirstLines;

    // Apply First Hough Transform
    HoughLinesP(FirstEdges, FirstLines, 1, CV_PI / 180, 100, 20, 20);
    for (size_t i = 0; i < FirstLines.size(); i++) {

        Vec4i l = FirstLines[i];

        line(greyMat, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 0, 0), 3, LINE_AA);
        //circle(greyMat, Point(l[0], l[1]), 2, Scalar(255, 0, 0), 10);
        //circle(greyMat, Point(l[2], l[3]), 2, Scalar(255, 0, 0), 10);
    }

    // image in black and white only with the lines detected + random white pixels
    Mat BW_mat;

    // to erase the cars and keep only the whites lines + random white pixels
    threshold(greyMat, BW_mat, 254, 255, THRESH_BINARY);

    // to erase random white pixels
    erode(BW_mat, BW_mat, Mat());

    // to get beautiful lines
    dilate(BW_mat, BW_mat, Mat(), Point(-1, -1), 1);

    // Store the edges   
    Mat SecondEdges;

    // Find the edges in the image using canny detector
    Canny(BW_mat, SecondEdges, 200, 255);

    // Create a vector to store lines of the image
    vector<Vec4i> SecondLines;



    // Apply Second Hough Transform
    HoughLinesP(SecondEdges, SecondLines, 1, CV_PI / 180, 100, 20, 10000);

    for (size_t j = 0; j < SecondLines.size(); j++) {

        Vec4i l = SecondLines[j];
        line(BW_mat, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 0, 0), 3, LINE_AA);


        circle(BW_mat, Point(l[0], l[1]), 2, Scalar(200), 10);
        circle(BW_mat, Point(l[2], l[3]), 2, Scalar(100, 255, 255), 10);
    }

    circle(BW_mat, Point(0, 0), 10, Scalar(100, 255, 255), 10);

    bool Pd_confondue; 
    bool Pf_confondue;
    bool PdPf_confondue;

    // check the points between them to keep only one by line
    for (int u = 0; u < SecondLines.size(); u++) {

        for (int v = u; v < SecondLines.size(); v++)
        {
            Pd_confondue = false;
            Pf_confondue = false;
            PdPf_confondue = false; 

            Vec4i l = SecondLines[u];
            Vec4i l2 = SecondLines[v];

            int marge_erreur = 20;

            //point x depart ligne 1
            int Pdx1 = l[0];
            //point y départ ligne 1
            int Pdy1 = l[1];
            //point x fin ligne 1
            int Pfx1 = l[3];
            //point y fin ligne 1
            int Pfy1 = l[4];

            //point x depart ligne 2
            int Pdx2 = l2[0];
            //point y départ ligne 2
            int Pdy2 = l2[1];
            //point x fin ligne 2
            int Pfx2 = l2[3];
            //point y fin ligne 2
            int Pfy2 = l2[4];


            if (dist(Pdx1, Pdx2) < marge_erreur && dist(Pdy1, Pdy2) < marge_erreur)
                Pd_confondue = true;
             

            if (dist(Pfx1, Pfx2) < marge_erreur && dist(Pfy1, Pfy2) < marge_erreur)
                Pf_confondue = true;

            if(dist(Pdx1, Pfx2) < marge_erreur && dist(Pdy2, Pfy1) < marge_erreur)
                PdPf_confondue = true;


            if (Pd_confondue && !Pf_confondue)
            {
                //TODO
                // 1 check s'ils sont collinéaire + longeur du segment 
                // 2 si collineaire prendre le plus grand 
            }
            else if (!Pd_confondue && Pf_confondue)
            {
                //TODO
                //1 check s'ils sont collinéaire + longeur du segment 
                // 2 si collineaire prendre le plus grand 
            }
            else if (Pd_confondue && Pf_confondue)
            {
                //TODO
                // moyenne des pf et pd / les deux vecteurs pareils

            }
            else if (PdPf_confondue)
            {
                //TODO
                // 1 Si collinéaire 
                // 2 vérifier les points confondues et creer un nouveau vecteur avec les point restant 

                //Sinon rien car supp dans les autres itérations
            }
            else 
            {
                cout << "OUII NTM" << endl;
            }
        }


        //Vec4i l = SecondLines[j];
        //line(BW_mat, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 0, 0), 3, LINE_AA);


    }


    imshow("img", img);
    imshow("GREY", greyMat);
    imshow("Image parking", BW_mat);

    //imshow("Mon Parking Jacky", img);

    waitKey(0); // Wait for any keystroke in the window
    return 0;
}


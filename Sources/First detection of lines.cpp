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


Vec4i average_line(Vec4i seg1, Vec4i seg2)
{
    Vec4i average;
    average[0] = (seg1[0] + seg2[0]) / 2;
    average[1] = (seg1[1] + seg2[1]) / 2;
    average[2] = (seg1[2] + seg2[2]) / 2;
    average[3] = (seg1[3] + seg2[3]) / 2;

    return average;
}


int* point_to_vector(Vec4i segment)
{
    int* tab = new int[2];
    tab[0] = segment[2] - segment[0];
    tab[1] = segment[3] - segment[1];

    cout << "vector X = " << tab[0] << ", vector Y = " << tab[1] << "\n";
    return tab;
}

bool areColinear(int* v1, int* v2)
{
    // no need to to check if the vectors are null because openCV doesn t create/detect null lines with Hought
    float k = (float)v1[0] / (float)v2[0];

    // we don t need exactly colinear vectors but more a tendency (+/- 20%)
    float kmin = k * 0.5;
    float kmax = k * 1.5;

    if (v2[1] * kmin <= v1[1] && v2[1] * kmax >= v1[1])
        return true;
    else
        return false;
}

void display_lines(vector<Vec4i> lines, Mat img) 
{
    for (size_t j = 0; j < lines.size(); j++) {

        Vec4i l = lines[j];
        line(img, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 0, 0), 3, LINE_AA);

        circle(img, Point(l[0], l[1]), 2, Scalar(200), 10);
        circle(img, Point(l[2], l[3]), 2, Scalar(100, 255, 255), 10);
    }
}

int length(int* vector)
{
    return sqrt(vector[0] * vector[0] + vector[1] * vector[1]);
}


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

    //------Read Pictures with lines-----///

    // Convert to gray-scale
    Mat greyMat;
    cvtColor(img, greyMat, cv::COLOR_RGB2GRAY);

    // Store the edges   
    Mat FirstEdges;

    // Find the edges in the image using canny detector
    Canny(greyMat, FirstEdges, 200, 255);

    // Create a vector to store lines of the image
    vector<Vec4i> all_lines;

    // Apply First Hough Transform
    HoughLinesP(FirstEdges, all_lines, 1, CV_PI / 180, 100, 20, 20);
    for (size_t i = 0; i < all_lines.size(); i++) 
    {
        Vec4i l = all_lines[i];
        line(greyMat, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 0, 0), 3, LINE_AA);
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

    ///////////////////////////TEST//////////////////////////

     // image in black and white only with the lines detected + random white pixels
    Mat BW_mat2;

    // to erase the cars and keep only the whites lines + random white pixels
    threshold(greyMat, BW_mat2, 254, 255, THRESH_BINARY);

    // to erase random white pixels
    erode(BW_mat2, BW_mat2, Mat());

    // to get beautiful lines
    dilate(BW_mat2, BW_mat2, Mat(), Point(-1, -1), 1);

    // Store the edges   
    Mat SecondEdges2;

    // Find the edges in the image using canny detector
    Canny(BW_mat2, SecondEdges2, 200, 255);

    ///////////////////////////FIN TEST///////////////////////

    // Create a vector to store lines of the image
    vector<Vec4i> parking_lines;



    // Apply Second Hough Transform
    HoughLinesP(SecondEdges, parking_lines, 1, CV_PI / 180, 100, 20, 10000);


    // ----------- creation of third image ----------------

    // To store all lines/points without points in the same near area (we don t know how to delete so we create a new one)
    vector<Vec4i> best_lines = parking_lines;
    cout << "longeur = " << best_lines.size() << "\n";
    bool Pd_confondue;
    bool Pf_confondue;
    bool PdPf_confondue;

    // check the points between them to keep only one by line
    for (size_t u = 0; u < best_lines.size() - 1; u++) {

        for (size_t v = u + 1; v < best_lines.size(); v++)
        {

            Pd_confondue = false;
            Pf_confondue = false;
            PdPf_confondue = false;

            Vec4i ligne = best_lines[u];
            Vec4i ligne2 = best_lines[v];

            //Distance max entre deux points pour etre confondues
            int marge_erreur = 50;

            //point x depart ligne 1
            int Pdx1 = ligne[0];
            //point y départ ligne 1
            int Pdy1 = ligne[1];
            //point x fin ligne 1
            int Pfx1 = ligne[2];
            //point y fin ligne 1
            int Pfy1 = ligne[3];

            //point x depart ligne 2
            int Pdx2 = ligne2[0];
            //point y départ ligne 2
            int Pdy2 = ligne2[1];
            //point x fin ligne 2
            int Pfx2 = ligne2[2];
            //point y fin ligne 2
            int Pfy2 = ligne2[3];


            if (dist(Pdx1, Pdx2) < marge_erreur && dist(Pdy1, Pdy2) < marge_erreur)
                Pd_confondue = true;

            if (dist(Pfx1, Pfx2) < marge_erreur && dist(Pfy1, Pfy2) < marge_erreur)
                Pf_confondue = true;

            if (dist(Pdx1, Pfx2) < marge_erreur && dist(Pdy2, Pfy1) < marge_erreur)
                PdPf_confondue = true;


            if (Pd_confondue && !Pf_confondue)
            {
                // 1 check s'ils sont collinéaire + longeur du segment 
                // 2 si collineaire prendre le plus grand 

                int* Vect1 = point_to_vector(ligne);
                int* Vect2 = point_to_vector(ligne2);

                if (areColinear(Vect1, Vect2))
                {
                    if (length(Vect1) > length(Vect2)) {

                        best_lines.erase(best_lines.begin() + v);
                        cout << "delete \n";
                    }
                    else {
                        cout << "delete \n";
                        best_lines.erase(best_lines.begin() + u);
                    }
                }

            }
            else if (!Pd_confondue && Pf_confondue)
            {
                //TODO
                //1 check s'ils sont collinéaire + longeur du segment 
                // 2 si collineaire prendre le plus grand 

                int* Vect1 = point_to_vector(ligne);
                int* Vect2 = point_to_vector(ligne2);

                if (areColinear(Vect1, Vect2))
                {
                    if (length(Vect1) > length(Vect2)) {

                        best_lines.erase(best_lines.begin() + v);
                        cout << "delete 2 \n";
                    }
                    else {
                        cout << "delete 2 \n";
                        best_lines.erase(best_lines.begin() + u);
                    }
                }

            }
            else if (Pd_confondue && Pf_confondue)
            {
                //TODO
                // moyenne des pf et pd / les deux vecteurs pareils
                Vec4i average_segment = average_line(ligne, ligne2);

                best_lines.erase(best_lines.begin() + v);
                best_lines.erase(best_lines.begin() + u);

                //add to the list
                best_lines.push_back(average_segment);

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
                //cout << "Julie est contente" << endl;
            }

        }
    }

    cout << "2nd line size = " << parking_lines.size() << " \n";
    cout << "3rd line size = " << best_lines.size() << " \n";

    imshow("img", img);
    imshow("GREY", greyMat);

    display_lines(parking_lines, BW_mat);
    display_lines(best_lines, BW_mat2);

    imshow("Image parking lines", BW_mat);
    imshow("Image parking best_lines", BW_mat2);


    waitKey(0); // Wait for any keystroke in the window
    return 0;
}


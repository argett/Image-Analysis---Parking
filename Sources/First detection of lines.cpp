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
    return tab;
}

bool areColinear(int* v1, int* v2)
{
    // no need to to check if the vectors are null because openCV doesn t create/detect null lines with Hought
    // u = k * v
    float k = (float)v1[0] / (float)v2[0];

    // we don t need exactly colinear vectors but more a tendency (+/- 20%)
    float kmin = k * 0.8;
    float kmax = k * 1.2;

    cout << "k = " << k << "\n";
    cout << "kmin = " << kmin << "\n";
    cout << "kmax = " << kmax << "\n";

    if (v2[1] * kmin >= v1[1] && v2[1] * kmax <= v1[1])
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

bool areSame_startPoint(Vec4i l1, Vec4i l2, int margin_error)
{
    //point x depart ligne 1
    int Pdx1 = l1[0];
    //point y départ ligne 1
    int Pdy1 = l1[1];

    //point x depart ligne 2
    int Pdx2 = l2[0];
    //point y départ ligne 2
    int Pdy2 = l2[1];


    if (dist(Pdx1, Pdx2) < margin_error && dist(Pdy1, Pdy2) < margin_error)
        return true;
    return false;
}

bool areSame_endPoint(Vec4i l1, Vec4i l2, int margin_error)
{
    //point x fin ligne 1
    int Pfx1 = l1[2];
    //point y fin ligne 1
    int Pfy1 = l1[3];

    //point x fin ligne 2
    int Pfx2 = l2[2];
    //point y fin ligne 2
    int Pfy2 = l2[3];

    if (dist(Pfx1, Pfx2) < margin_error && dist(Pfy1, Pfy2) < margin_error)
        return true;
    return false;
}

bool areSame_startEnd_Points(Vec4i l1, Vec4i l2, int margin_error)
{
    //point x depart ligne 1
    int Pdx1 = l1[0];
    //point y fin ligne 1
    int Pfy1 = l1[3];

    //point y départ ligne 2
    int Pdy2 = l2[1];
    //point x fin ligne 2
    int Pfx2 = l2[2];

    if (dist(Pdx1, Pfx2) < margin_error && dist(Pdy2, Pfy1) < margin_error)
        return true;
    return false;
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

    // Find the edges in the image using canny detector, every pixel with color < 200 is black
    Canny(BW_mat, SecondEdges, 200, 255);

     // image in black and white only with the lines detected + random white pixels
    Mat BW_mat2;

    // to erase the cars and keep only the whites lines + random white pixels
    threshold(greyMat, BW_mat2, 254, 255, THRESH_BINARY);

    // to erase random white pixels
    erode(BW_mat2, BW_mat2, Mat());

    // to get beautiful lines
    dilate(BW_mat2, BW_mat2, Mat(), Point(-1, -1), 1);


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
    for (size_t u = 0; u < best_lines.size() - 1; u++) 
    {
        // flag, we repeat the state u n-times to remove all occurencies
        bool allClear = false;
        while (!allClear) 
        {
            // by definition, everything is clear, if there is an occurence we put to false
            allClear = true;

            for (size_t v = u + 1; v < best_lines.size(); v++)
            {
                //Distance max entre deux points pour etre confondues
                int margin_error = 70;

                Vec4i ligne = best_lines[u];
                Vec4i ligne2 = best_lines[v];

                Pd_confondue = areSame_startPoint(ligne, ligne2, margin_error);
                Pf_confondue = areSame_endPoint(ligne, ligne2, margin_error);
                PdPf_confondue = areSame_startEnd_Points(ligne, ligne2, margin_error);

                if (Pd_confondue && !Pf_confondue)
                {
                    // 1 check s'ils sont collinéaire + longeur du segment 
                    // 2 si collineaire prendre le plus grand 

                    int* Vect1 = point_to_vector(ligne);
                    int* Vect2 = point_to_vector(ligne2);

                    if (areColinear(Vect1, Vect2))
                    {
                        if (length(Vect1) > length(Vect2)) 
                        {
                            best_lines.erase(best_lines.begin() + v);
                            cout << "delete \n";
                        }
                        else 
                        {
                            cout << "delete \n";
                            best_lines.erase(best_lines.begin() + u);
                        }
                        allClear = false;
                    }
                    else
                    {
                        cout << "erreor : !Pf_confondue & Pd_confondue = HORTOGONAL\n";
                    }

                }
                else if (!Pd_confondue && Pf_confondue)
                {
                    //1 check s'ils sont collinéaire + longeur du segment 
                    // 2 si collineaire prendre le plus grand 

                    int* Vect1 = point_to_vector(ligne);
                    int* Vect2 = point_to_vector(ligne2);

                    if (areColinear(Vect1, Vect2))
                    {
                        if (length(Vect1) > length(Vect2)) 
                        {

                            best_lines.erase(best_lines.begin() + v);
                            cout << "delete \n";
                        }
                        else 
                        {
                            cout << "delete \n";
                            best_lines.erase(best_lines.begin() + u);
                        }
                        allClear = false;
                    }
                    else
                    {
                        cout << "erreor : Pf_confondue & !Pd_confondue = HORTOGONAL\n";
                    }

                }
                else if (Pd_confondue && Pf_confondue)
                {
                    // moyenne des pf et pd / les deux vecteurs pareils
                    Vec4i average_segment = average_line(ligne, ligne2);
                    best_lines.erase(best_lines.begin() + v);
                    best_lines.erase(best_lines.begin() + u);

                    //add to the list
                    best_lines.push_back(average_segment);
                    allClear = false;

                }
                else if (PdPf_confondue)
                {
                    // 1 Si collinéaire 
                    // 2 vérifier les points confondues et creer un nouveau vecteur avec les point restant 

                    int* Vect1 = point_to_vector(ligne);
                    int* Vect2 = point_to_vector(ligne2);

                    if (areColinear(Vect1, Vect2))
                    {
                        Vec4i fusioned_vector;
                        fusioned_vector[0] = ligne[0];
                        fusioned_vector[1] = ligne[1];
                        fusioned_vector[2] = ligne2[2];
                        fusioned_vector[3] = ligne2[3];

                        best_lines.erase(best_lines.begin() + v);
                        best_lines.erase(best_lines.begin() + u);

                        best_lines.push_back(fusioned_vector);

                        allClear = false;
                    }
                    else
                    {
                        cout << "erreor : PDPF_CONFONDUE = HORTOGONAL\n";
                    }
                }
                else
                {
                    //cout << "Julie est contente" << endl;
                }

            }
        }
        
    }

    cout << "2nd line size = " << parking_lines.size() << " \n";
    cout << "3rd line size = " << best_lines.size() << " \n";

    imshow("img", img);
    imshow("GREY", greyMat);

    display_lines(parking_lines, BW_mat);
    display_lines(best_lines, BW_mat2);
    /*
    circle(BW_mat2, Point(best_lines[0][0], best_lines[0][1]), 2, Scalar(100, 255, 255), 10);
    circle(BW_mat2, Point(best_lines[0][2], best_lines[0][3]), 2, 255, 10);

    circle(BW_mat2, Point(best_lines[5][0], best_lines[5][1]), 2, Scalar(100, 255, 255), 10);
    circle(BW_mat2, Point(best_lines[5][2], best_lines[5][3]), 2, 255, 10);

    // 3 et 5 sont sur une même  ligne, un des deux doit etre DEGAGE CE FDP
    
    cout << "---------\n";

    cout << "x0 depart = " << best_lines[0][0] << " \n";
    cout << "y0 depart = " << best_lines[0][1] << " \n";
    cout << "x0 fin = " << best_lines[0][2] << " \n";
    cout << "y0 fin = " << best_lines[0][3] << " \n";

    cout << "x5 depart = " << best_lines[5][0] << " \n";
    cout << "y5 depart = " << best_lines[5][1] << " \n";
    cout << "x5 fin = " << best_lines[5][2] << " \n";
    cout << "y5 fin = " << best_lines[5][3] << " \n";
    
    cout << "depart confondu = " << areSame_startPoint(best_lines[0], best_lines[5], 50) << " \n";
    cout << "fin confondu = " << areSame_endPoint(best_lines[0], best_lines[5], 50) << " \n";
    cout << "tout confondu = " << areSame_startEnd_Points(best_lines[0], best_lines[5], 50) << " \n";
    cout << "sont colineaire = " << areColinear(point_to_vector(best_lines[0]), point_to_vector(best_lines[5])) << " \n";
    */
    imshow("Image parking lines - BW_mat", BW_mat);
    imshow("Image parking best_lines - BW_Mat2", BW_mat2);


    waitKey(0); // Wait for any keystroke in the window
    return 0;
}


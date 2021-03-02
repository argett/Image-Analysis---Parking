#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>


using namespace cv;
using namespace std;


/*

    The image must contain only one big white line with a lot of shorts perpendiculars to it
    The background must be more or less at the same color

*/


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

    if (v2[1] * kmin >= v1[1] && v2[1] * kmax <= v1[1])
        return true;
    else
        return false;
}

bool AreTrueColinear(int* v1, int* v2)
{
    float k = (float)v1[0] / (float)v2[0];
    //lilian the nazi Arrrr
    if (v2[1] * k == v1[1])
        return true;
    else
        return false;
}

void display_lines(vector<Vec4i> lines, Mat img)
{
    for (size_t j = 0; j < lines.size(); j++) {

        Vec4i l = lines[j];
        line(img, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 0, 0), 3, LINE_AA);
    }
}

bool areSame_startPoint(Vec4i l1, Vec4i l2, int margin_error)
{
    //point x depart ligne 1
    int Pdx1 = l1[0];
    //point y d�part ligne 1
    int Pdy1 = l1[1];

    //point x depart ligne 2
    int Pdx2 = l2[0];
    //point y d�part ligne 2
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

    //point y d�part ligne 2
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

Point intersection(Vec4i l1, Vec4i l2)
{
    //we want the equation of the line of type y = ax + b
    float a1 = ((float)l1[3] - (float)l1[1]) / ((float)l1[2] - (float)l1[0]);
    //we use e point of the line to find b
    float b1 = (float)l1[1] - (a1 * (float)l1[0]);

    float a2 = ((float)l2[3] - (float)l2[1]) / ((float)l2[2] - (float)l2[0]);
    float b2 = (float)l2[1] - (a2 * (float)l2[0]);

    //we are looking for the intersection point     
    int x = (b2 - b1) / (a1 - a2);
    int y = (a1 * x) + b1;

    return Point(x, y);
}


int main()
{
    //read the image file
    Mat img = imread("./Images/parking3.jpg");
    int imgWidth = img.cols;

    if (img.empty()) // Check for failure
    {
        cout << "Could not open or find the image" << endl;
        system("pause"); //wait for any key press
        return -1;
    }

    //------Read Pictures with lines-----///

    // Convert to gray-scale
    Mat img_gray_scale;
    cvtColor(img, img_gray_scale, cv::COLOR_RGB2GRAY);

    // Store the edges   
    Mat FirstEdges;

    // Find the edges in the image using canny detector
    Canny(img_gray_scale, FirstEdges, 200, 255);

    // Create a vector to store lines of the image
    vector<Vec4i> all_lines;

    // Apply First Hough Transform
    HoughLinesP(FirstEdges, all_lines, 1, CV_PI / 180, 100, 20, 20);
    for (size_t i = 0; i < all_lines.size(); i++)
    {
        Vec4i l = all_lines[i];
        line(img_gray_scale, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 0, 0), 3, LINE_AA);
    }

    // image in black and white only with the lines detected + random white pixels
    Mat img_bw_lines;

    // to erase the cars and keep only the whites lines + random white pixels
    threshold(img_gray_scale, img_bw_lines, 254, 255, THRESH_BINARY);

    // to erase random white pixels
    erode(img_bw_lines, img_bw_lines, Mat());

    // to get beautiful lines
    dilate(img_bw_lines, img_bw_lines, Mat(), Point(-1, -1), 1);

    // Store the edges   
    Mat SecondEdges;

    // Find the edges in the image using canny detector, every pixel with color < 200 is black
    Canny(img_bw_lines, SecondEdges, 200, 255);

    // image in black and white only with the lines detected + random white pixels
    Mat img_bw_lines2;

    // to erase the cars and keep only the whites lines + random white pixels
    threshold(img_gray_scale, img_bw_lines2, 254, 255, THRESH_BINARY);

    // to erase random white pixels
    erode(img_bw_lines2, img_bw_lines2, Mat());

    // to get beautiful lines
    dilate(img_bw_lines2, img_bw_lines2, Mat(), Point(-1, -1), 1);


    // Create a vector to store lines of the image
    vector<Vec4i> parking_lines;

    // Apply Second Hough Transform
    HoughLinesP(SecondEdges, parking_lines, 1, CV_PI / 180, 100, 20, 10000);



    // ----------- creation of third image ----------------
    //We creat multiple images to show every steps
    Mat img_falsePoints = img.clone();
    Mat img_rightPoints = img.clone();
    //We print the result of the HoughLines
    display_lines(parking_lines, img_falsePoints);
    for (size_t i = 0; i < parking_lines.size(); i++) {
        circle(img_falsePoints, Point(parking_lines[i][0], parking_lines[i][1]), 2, Scalar(255, 255, 255), 10);
        circle(img_falsePoints, Point(parking_lines[i][2], parking_lines[i][3]), 2, Scalar(255, 255, 255), 10);

    }
    // To store all lines/points without points in the same near area (we don t know how to delete so we create a new one)
    vector<Vec4i> best_lines = parking_lines;
    cout << parking_lines[0];
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
                    // 1 check s'ils sont collin�aire + longeur du segment 
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
                        cout << "erreor : !Pf_confondue & Pd_confondue = ORTOGONAL\n";
                    }

                }
                else if (!Pd_confondue && Pf_confondue)
                {
                    //1 check s'ils sont collin�aire + longeur du segment 
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
                        cout << "error : Pf_confondue & !Pd_confondue = ORTOGONAL\n";
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
                    // 1 if collinear 
                    // 2 verify the confused points and create a new vector with the other points

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
                        cout << "erreor : PDPF_CONFONDUE = ORTOGONAL\n";
                    }
                }
                else
                {
                    //cout << "Julie est contente" << endl;
                }
            }
        }
    }
    //We print the result in a dedicate image
    display_lines(best_lines, img_rightPoints);
    for (size_t j = 0; j < best_lines.size(); j++) {
        circle(img_rightPoints, Point(best_lines[j][0], best_lines[j][1]), 2, Scalar(255, 255, 255), 10);
        circle(img_rightPoints, Point(best_lines[j][2], best_lines[j][3]), 2, Scalar(255, 255, 255), 10);

    }

    //--------------    We analyse all the lines to find the intersections point between the places's white lines and the middle white line   -----------------------------------

    int Nb_Intersections = 0;
    int lenght = static_cast<int>(best_lines.size());;
    Point* intersections_points = new Point[lenght];

    //we creat a 2d array to store the lines that will be sorted
    Vec4i** intersections_lines = (Vec4i**)malloc(lenght * sizeof(Vec4i*));
    for (int i = 0; i < lenght; i++)
        intersections_lines[i] = (Vec4i*)malloc(2 * sizeof(Vec4i));

    //we find the intersection and put them in an array, we also store the lines that intersect
    for (size_t u = 0; u < best_lines.size() - 1; u++)
    {
        for (size_t v = u + 1; v < best_lines.size(); v++)
        {
            if (!AreTrueColinear(point_to_vector(best_lines[u]), point_to_vector(best_lines[v])))
            {
                // because some lines are quasi-parallels, we reject intersection at very higth/low position (register the ones in the image only)

                Point inter = intersection(best_lines[u], best_lines[v]);
                if ((inter.x < img.size().width && inter.x > 0) && (inter.y < img.size().height && inter.y>0))
                {
                    intersections_points[Nb_Intersections] = inter;
                    //we store the affiliate line in the same position
                    intersections_lines[Nb_Intersections][0] = best_lines[u];
                    intersections_lines[Nb_Intersections][1] = best_lines[v];

                    Nb_Intersections++;
                }
            }
        }
    }

    //we create the array with just the exact lenght
    Point* final_intersections_points = new Point[(Nb_Intersections)];
    //The intersection point where final_intersections_points[extreme_left] has the minimal X
    int extreme_left = 0;
    for (int i = 0; i < Nb_Intersections; i++)
    {
        final_intersections_points[i] = intersections_points[i];
        if (final_intersections_points[i].x <= final_intersections_points[extreme_left].x) {
            extreme_left = i;
        }
    }
    //We want to show the intersections in a dedicate image
    Mat imgIntersections = img_rightPoints.clone();
    for (int i = 0; i < Nb_Intersections; i++)
    {
        circle(imgIntersections, final_intersections_points[i], 2, Scalar(0, 255, 255), 10);

    }
    //We sort the extreme_left point as the start of the array
    Point temp_Inter = final_intersections_points[0];
    final_intersections_points[0] = final_intersections_points[extreme_left];
    final_intersections_points[extreme_left] = temp_Inter;
    //We do the same for the corresponding line array
    Vec4i* temp_Line = intersections_lines[0];
    intersections_lines[0] = intersections_lines[extreme_left];
    intersections_lines[extreme_left] = temp_Line;

    //we want to sort the array of intersection and affiliate lines
    for (int i = 0; i < Nb_Intersections - 1; i++)
    {
        float minDist = INTMAX_MAX;
        int minIndex;
        for (int j = i + 1; j < Nb_Intersections; j++)
        {
            // We compute the max distance possible between the differents intersections to find the extremities
            Vec4i temp_Vect;
            temp_Vect[0] = final_intersections_points[i].x;
            temp_Vect[1] = final_intersections_points[i].y;
            temp_Vect[2] = final_intersections_points[j].x;
            temp_Vect[3] = final_intersections_points[j].y;

            int* vect = point_to_vector(temp_Vect);
            float dist = length(vect);
            if (dist < minDist) {
                minDist = dist;
                minIndex = j;
            }
        }

        //We sort the array
        Point temp_Inter = final_intersections_points[i + 1];
        final_intersections_points[i + 1] = final_intersections_points[minIndex];
        final_intersections_points[minIndex] = temp_Inter;
        //We do the same for the corresponding line array
        Vec4i* temp_Line = intersections_lines[i + 1];
        intersections_lines[i + 1] = intersections_lines[minIndex];
        intersections_lines[minIndex] = temp_Line;
    }

    int NB_places = (Nb_Intersections - 1) * 2;
    Point** parkingPlaces = (Point**)malloc(NB_places * sizeof(Point));
    for (int i = 0; i < (Nb_Intersections - 1) * 2; i++)
        parkingPlaces[i] = (Point*)malloc(4 * sizeof(Point));

    //--------------    We sort the intersection points from left to right & same for the intersection lines in order to get the places  -----------------------------------

    //Ensuite, une fois les deux array bien tri�es, on fais un boucle for avec i et i + 1 qui du coup sont deux intersections proche, 
    //on calcule le vecteur entre ces deux intersections qui permetra d'identifier la ligne commune a toute les intersections.
    //Une fois cette ligne trouv� on prend le point de d�part ou de fin en fonction du quadrilataire qu'on veut traiter.
    //Meme chose pour le point de l'autre intersection
    int ParkingPlaceindex = 0;
    /*
    0 | 2 | 4 ...
    -------------
    1 | 3 | 5 ...
    */

    for (int i = 0; i < Nb_Intersections - 1; i++)
    {
        //We creat a vector between the two intersections
        Vec4i temp_VectInter;
        temp_VectInter[0] = final_intersections_points[i].x;
        temp_VectInter[1] = final_intersections_points[i].y;
        temp_VectInter[2] = final_intersections_points[i + 1].x;
        temp_VectInter[3] = final_intersections_points[i + 1].y;
        int* vectInter = point_to_vector(temp_VectInter);

        //I take the two affiliated lines
        //intersection lines[i][one of the 2 lines which create the intersection][x/y of point 1, x/y of point 2]
        int* vectline1 = point_to_vector(intersections_lines[i][0]);
        int* vectline2 = point_to_vector(intersections_lines[i][1]);

        //We do the same thing with the second intersection point
        int* vect2line1 = point_to_vector(intersections_lines[i + 1][0]);
        int* vect2line2 = point_to_vector(intersections_lines[i + 1][1]);

        if (areColinear(vectInter, vectline2))
        {
            /*
            We take the start point of the other affiliated line
            The representation of the array parkingPlaces[i][0/1/2/3], each number represent a point a the square
            |  p  0  p  2  p  |
            |  l  |  l  |  l  |
            |  a  |  a  |  a  |
            |  c  |  c  |  c  |
            |  e  1  e  3  e  |
            --------------------separation line
            */
            parkingPlaces[ParkingPlaceindex][0] = Point(intersections_lines[i][0][0], intersections_lines[i][0][1]);

            //We also take the two intersection points
            parkingPlaces[ParkingPlaceindex][1] = Point(temp_VectInter[0], temp_VectInter[1]);
            parkingPlaces[ParkingPlaceindex][2] = Point(temp_VectInter[2], temp_VectInter[3]);

            //We need to find the second start point using the second intersection point
            if (areColinear(vectInter, vect2line2)) {
                parkingPlaces[ParkingPlaceindex][3] = Point(intersections_lines[i + 1][0][0], intersections_lines[i + 1][0][1]);
                //Here we take the second end point
                parkingPlaces[ParkingPlaceindex + 1][3] = Point(intersections_lines[i + 1][0][2], intersections_lines[i + 1][0][3]);
            }
            else if (areColinear(vectInter, vect2line1)) {
                parkingPlaces[ParkingPlaceindex][3] = Point(intersections_lines[i + 1][1][1], intersections_lines[i + 1][1][1]);
                parkingPlaces[ParkingPlaceindex + 1][3] = Point(intersections_lines[i + 1][1][2], intersections_lines[i + 1][1][3]);
            }
            //We do the same thing using the ends point to find the other place using the same lines and intersections 
            /*
            |ParkingPlaceindex|
            ---------------------
            |ParkingPlaceindex+1|
            */
            parkingPlaces[ParkingPlaceindex + 1][0] = Point(intersections_lines[i][0][2], intersections_lines[i][0][3]);
            parkingPlaces[ParkingPlaceindex + 1][1] = Point(temp_VectInter[0], temp_VectInter[1]);
            parkingPlaces[ParkingPlaceindex + 1][2] = Point(temp_VectInter[2], temp_VectInter[3]);

        }
        else if (areColinear(vectInter, vectline1))
        {
            parkingPlaces[ParkingPlaceindex][0] = Point(intersections_lines[i][1][0], intersections_lines[i][1][1]);
            parkingPlaces[ParkingPlaceindex][1] = Point(temp_VectInter[0], temp_VectInter[1]);
            parkingPlaces[ParkingPlaceindex][2] = Point(temp_VectInter[2], temp_VectInter[3]);



            if (areColinear(vectInter, vect2line2)) {
                parkingPlaces[ParkingPlaceindex][3] = Point(intersections_lines[i + 1][0][0], intersections_lines[i + 1][0][1]);
                parkingPlaces[ParkingPlaceindex + 1][3] = Point(intersections_lines[i + 1][0][2], intersections_lines[i + 1][0][3]);
            }
            else if (areColinear(vectInter, vect2line1)) {
                parkingPlaces[ParkingPlaceindex][3] = Point(intersections_lines[i + 1][1][1], intersections_lines[i + 1][1][1]);
                parkingPlaces[ParkingPlaceindex + 1][3] = Point(intersections_lines[i + 1][1][2], intersections_lines[i + 1][1][3]);
            }

            parkingPlaces[ParkingPlaceindex + 1][0] = Point(intersections_lines[i][1][2], intersections_lines[i][1][3]);
            parkingPlaces[ParkingPlaceindex + 1][1] = Point(temp_VectInter[0], temp_VectInter[1]);
            parkingPlaces[ParkingPlaceindex + 1][2] = Point(temp_VectInter[2], temp_VectInter[3]);
        }
        else {
            cout << "on a un probleme d'ordre \n";
        }
        ParkingPlaceindex += 2;
    }

    //----------------    We are going to check the mean background color value    -----------------------------------

    imshow("img début", img);

    // convert to float & reshape to a [3 x W*H] Mat 
    //  (so every pixel is on a row of it's own)

    /*
    * CV_32F is float - the pixel can have any value between 0-1.0, this is useful for some sets of calculations on data -
    * but it has to be converted into 8bits to save or display by multiplying each pixel by 255.
    */
    Mat img_reduced_background_color;
    img.convertTo(img_reduced_background_color, CV_32F);
    img_reduced_background_color = img_reduced_background_color.reshape(1, img_reduced_background_color.total());

    // do kmeans
    /*
    * Finds centers of clusters and groups input samples around the clusters.
    * The function kmeans implements a k-means algorithm that finds the centers of cluster_count clusters
    * and groups the input samples around the clusters.
    * As an output, bestLabelsi contains a 0-based cluster index for the sample stored in the ith row of the samples matrix.
    *
    * labels : Input/output integer array that stores the cluster indices for every sample.
    * centers : 	Output matrix of the cluster centers, one row per each cluster center.
    */

    Mat labels, mean_background_value;

    //we keep 2 for the second argument, because we want a lower margin error -> 2 is the number of mean colors
    kmeans(img_reduced_background_color, 2, labels, TermCriteria(TermCriteria::COUNT, 10, 1.0), 3, KMEANS_PP_CENTERS, mean_background_value);

    cout << " centers  : " << (int)mean_background_value.at<float>(0, 0) << "\n";
    cout << " centers  : " << (int)mean_background_value.at<float>(0, 1) << "\n";
    cout << " centers  : " << (int)mean_background_value.at<float>(0, 2) << "\n";
    cout << " centers  : " << mean_background_value.row(0) << "\n";


    // reshape both to a single row of Vec3f pixels
    mean_background_value = mean_background_value.reshape(3, mean_background_value.rows);
    img_reduced_background_color = img_reduced_background_color.reshape(3, img_reduced_background_color.rows);


    // back to 2d, and uchar:
    img = img_reduced_background_color.reshape(3, img.rows);
    img.convertTo(img, CV_8U);

    int siz = 64;
    Mat cent = mean_background_value.reshape(3, mean_background_value.rows);
    // make  a horizontal bar of K color patches:
    Mat draw(siz, siz * cent.rows, cent.type(), Scalar::all(0));
    for (int i = 0; i < cent.rows; i++) {
        // set the resp. ROI to that value (just fill it):
        draw(Rect(i * siz, 0, siz, siz)) = cent.at<Vec3f>(i, 0);
    }
    draw.convertTo(draw, CV_8U);

    //We want to show the final result in a dedicate image
    Mat img_Final = imgIntersections.clone();


    int MarginColor_error = 10;
    bool flag = false;
    int Nb_takenPlaces = 0;

    for (int i = 0; i < NB_places; i++)
    {
        Vec4i diagonal1;
        diagonal1[0] = parkingPlaces[i][0].x;
        diagonal1[1] = parkingPlaces[i][0].y;
        diagonal1[2] = parkingPlaces[i][2].x;
        diagonal1[3] = parkingPlaces[i][2].y;

        Vec4i diagonal2;
        diagonal2[0] = parkingPlaces[i][1].x;
        diagonal2[1] = parkingPlaces[i][1].y;
        diagonal2[2] = parkingPlaces[i][3].x;
        diagonal2[3] = parkingPlaces[i][3].y;

        Point middle = intersection(diagonal1, diagonal2);


        for (int m = -3; m < 4; m++) {
            for (int n = -3; n < 4; n++) {
                Vec3b pixel_temp = img.at<Vec3b>(middle.y + n, middle.x + m);
                //We compare the two pixel with an error margin in BGR

                if (!(pixel_temp[0] >= (int)mean_background_value.at<float>(0, 0) - MarginColor_error && pixel_temp[0] <= (int)mean_background_value.at<float>(0, 0) + MarginColor_error)
                    || !(pixel_temp[1] >= (int)mean_background_value.at<float>(0, 1) - MarginColor_error && pixel_temp[1] <= (int)mean_background_value.at<float>(0, 1) + MarginColor_error)
                    || !(pixel_temp[2] >= (int)mean_background_value.at<float>(0, 2) - MarginColor_error && pixel_temp[2] <= (int)mean_background_value.at<float>(0, 2) + MarginColor_error)) {
                    flag = true;
                }
            }
        }
        if (flag) {
            Nb_takenPlaces++;
            circle(img_Final, middle, 2, Scalar(255, 0, 0), 10);
            flag = false;
        }
        else {
            circle(img_Final, middle, 2, Scalar(0, 0, 255), 10);
        }

    }


    cout << "il y a : " << NB_places << " places dont " << Nb_takenPlaces << " prises \n";


    //circle(img_bw_lines2, final_intersections_points[0], 2, new Scalar(100, 255, 255), 10);
    /*
    cout << "intersections = \n";
    for (int aaaaa = 0; aaaaa <= (lenght/2); aaaaa++)
    {
        cout << "x = " << (int)intersections[aaaaa].x << ", y = " << (int)intersections[aaaaa].y << "\n";
    }

    cout << "final_intersections = \n";
    for (int aaaaa = 0; aaaaa < cpt; aaaaa++)
    {
        cout << "x = " << (int)final_intersections[aaaaa].x << ", y = " << (int)intersections[aaaaa].y << "\n";
    }
    */
    


    imshow("img", img);
    //imshow("GREY", img_gray_scale);
    
    display_lines(parking_lines, img_bw_lines);
    display_lines(best_lines, img_bw_lines2);
    

    
   
    

    //imshow("Image parking lines - img_bw_lines", img_bw_lines);
    imshow("Image parking best_lines - img_bw_lines2", img_bw_lines2);

    //We show the first stage
    imshow("Wrong points", img_falsePoints);

    
    //We show the second step with the right points and the right lines
    imshow("Right Points", img_rightPoints);

    //We show the third step, the intersections
    imshow("Intersections", imgIntersections);


    //We show the final step, the available parking places
    imshow("Image finale", img_Final);

    // free space
    for (int f = 0; f < lenght; f++)
        free(intersections_lines[f]);
    free(intersections_lines);

    for (int f = 0; f < NB_places; f++)
        free(parkingPlaces[f]);
    free(parkingPlaces);

    waitKey(0); // Wait for any keystroke in the window
    return 0;

    // test code
    /*
    circle(img_bw_lines2, Point(best_lines[0][0], best_lines[0][1]), 2, new Scalar(100, 255, 255), 10);
    circle(img_bw_lines2, Point(best_lines[0][2], best_lines[0][3]), 2, 255, 10);

    circle(img_bw_lines2, Point(best_lines[5][0], best_lines[5][1]), 2, new Scalar(100, 255, 255), 10);
    circle(img_bw_lines2, Point(best_lines[5][2], best_lines[5][3]), 2, 255, 10);

    // 3 et 5 sont sur une m�me  ligne, un des deux doit etre DEGAGE CE FDP

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

}


// Projet Analyse d'image - parking.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

namespace creationLines 
{
    void create_lines()
    {
        char wndname[] = "Drawing Demo";
        const int DELAY = 5;
        int lineType = LINE_AA; // change it to LINE_8 to see non-antialiased graphics
        int i, width = 600, height = 300;
        int x1 = -width / 2, x2 = width * 3 / 2, y1 = -height / 2, y2 = height * 3 / 2;
        RNG rng(0xFFFFFFFF);

        Mat image = Mat::zeros(height, width, CV_8UC3);
        int r = 100;
        int v = 100;
        for (i = 0; i < 5; i++)
        {
            Point pt1, pt2;
            pt1.x = rng.uniform(x1, x2);
            pt1.y = rng.uniform(y1, y2);
            pt2.x = rng.uniform(x1, x2);
            pt2.y = rng.uniform(y1, y2);

            line(image, pt1, pt2, Scalar(r, v, 255), rng.uniform(1, 1), lineType);
            r += 100;
            if (r > 255) {
                r = 255;
                v += 100;
                if (v > 255)
                    v = 255;
            }
        }

        imshow("Display Window", image);

        //HoughLines() A FAIRE EN PRENANT LE FICHIER DE JULIE 

        waitKey(0);
    }
}


// Exécuter le programme : Ctrl+F5 ou menu Déboguer > Exécuter sans débogage
// Déboguer le programme : F5 ou menu Déboguer > Démarrer le débogage

// Astuces pour bien démarrer : 
//   1. Utilisez la fenêtre Explorateur de solutions pour ajouter des fichiers et les gérer.
//   2. Utilisez la fenêtre Team Explorer pour vous connecter au contrôle de code source.
//   3. Utilisez la fenêtre Sortie pour voir la sortie de la génération et d'autres messages.
//   4. Utilisez la fenêtre Liste d'erreurs pour voir les erreurs.
//   5. Accédez à Projet > Ajouter un nouvel élément pour créer des fichiers de code, ou à Projet > Ajouter un élément existant pour ajouter des fichiers de code existants au projet.
//   6. Pour rouvrir ce projet plus tard, accédez à Fichier > Ouvrir > Projet et sélectionnez le fichier .sln.

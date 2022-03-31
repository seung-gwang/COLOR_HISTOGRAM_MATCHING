#include <iostream>
#include "opencv2/opencv.hpp"

#define WINDOW_SIZE_X 378*2
#define WINDOW_SIZE_Y 504*2
#define PATCH_SIDE_LEN 9*5
#define FILE_NAME_1 "Source.jpg"
#define FILE_NAME_2 "Target.jpg"

using namespace std;
using namespace cv;

void on_mouse(int event, int x, int y, int falgs, void* userdata);


int main(int argc, char** argv)
{   
    //Load Image
    Mat trg = imread(FILE_NAME_1);
    Mat src = imread(FILE_NAME_2);

    if (trg.empty() || src.empty()) {
        cerr << "Image load failed!" << endl;

        return -1;
    }

    //Select Keypoints by mouse click
    Ptr<Feature2D> feature = SIFT::create(4);
    vector<KeyPoint> trgKeypoints, srcKeypoints;

    namedWindow("Image#1", WINDOW_NORMAL);
    resizeWindow("Image#1", WINDOW_SIZE_X, WINDOW_SIZE_Y);
    setMouseCallback("Image#1", on_mouse, &trgKeypoints);
    imshow("Image#1", trg);
    waitKey();
    destroyWindow("Image#1");

    namedWindow("Image#2", WINDOW_NORMAL);
    resizeWindow("Image#2", WINDOW_SIZE_X, WINDOW_SIZE_Y);
    setMouseCallback("Image#2", on_mouse, &srcKeypoints);
    imshow("Image#2", src);
    waitKey();
    destroyWindow("Image#2");

    cout << "Processing... Pleas Wait." << endl;

    //SIFT processing
    Mat trgDesc, srcDesc;
    feature->compute(trg, trgKeypoints, trgDesc);
    feature->compute(src, srcKeypoints, srcDesc);
    
    Ptr<DescriptorMatcher> matcher = BFMatcher::create(NORM_L1);
    
    vector<DMatch> matches;
    matcher->match(trgDesc, srcDesc, matches);


    sort(matches.begin(), matches.end());
    vector<DMatch> good_matches(matches.begin(), matches.begin() + 4);

    

    Mat dst;
    drawMatches(trg, trgKeypoints, src, srcKeypoints, good_matches, dst, 25,
        Scalar::all(-1), Scalar(-1), vector<char>(), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    namedWindow("RESULT", WINDOW_NORMAL);
    resizeWindow("RESULT", WINDOW_SIZE_X, WINDOW_SIZE_Y);
    imshow("RESULT", dst);

    waitKey();

    return 0;
}

void on_mouse(int event, int x, int y, int falgs, void* userdata) {

    switch (event) {

    case EVENT_LBUTTONDOWN:
        break;
    case EVENT_LBUTTONUP:
        cout << "(" << x << "," << y << ")" << "is Clicked and saved as a Keypoint!" << endl;
        KeyPoint clicked((float)x, (float)y, PATCH_SIDE_LEN);
        ((vector<KeyPoint>*)userdata)->push_back(clicked);
        break;

    }

}


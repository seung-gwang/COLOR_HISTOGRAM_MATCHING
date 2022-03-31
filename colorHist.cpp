#include "opencv2//highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <stdio.h>


#define WINDOW_SIZE_X 378*2
#define WINDOW_SIZE_Y 504*2
#define PATCH_SIDE_LEN 9*10
#define FILE_NAME_1 "Source.jpg"
#define FILE_NAME_2 "Target.jpg"

using namespace std;
using namespace cv;

void on_mouse(int event, int x, int y, int falgs, void* userdata);

struct Userdata {
    vector<Mat>* ROIs;
    Mat* img;
};

struct BGR_Hists {
    Mat b_hist;
    Mat g_hist;
    Mat r_hist;
};

int main(int argc, char** argv)
{   //이미지 불러오기
    Mat trg = imread(FILE_NAME_1);
    Mat src = imread(FILE_NAME_2);
    cout << trg.rows << "X" << trg.cols << endl;
    cout << src.rows << "X" << src.cols << endl;

    if (trg.empty() || src.empty()) {
        cerr << "Image load failed!" << endl;

        return -1;
    }


    vector<Mat> trgMats, srcMats;
    vector<KeyPoint> trgKeypoints, srcKeypoints;
    struct Userdata data1, data2;

    data2.ROIs = &srcMats;
    data2.img = &src;
    namedWindow("Image#2", WINDOW_NORMAL);
    resizeWindow("Image#2", WINDOW_SIZE_X, WINDOW_SIZE_Y);
    setMouseCallback("Image#2", on_mouse, &data2);
    imshow("Image#2", src);
    waitKey();
    destroyWindow("Image#2");

    data1.ROIs = &trgMats;
    data1.img = &trg;
    namedWindow("Image#1", WINDOW_NORMAL);
    resizeWindow("Image#1", WINDOW_SIZE_X, WINDOW_SIZE_Y);
    setMouseCallback("Image#1", on_mouse, &data1);
    imshow("Image#1", trg);
    waitKey();
    destroyWindow("Image#1");


    cout << "Processing... Please Wait." << endl << endl;


    int channels[3] = { 0 ,1, 2 }; //B G R

    struct BGR_Hists trgHists[4];
    struct BGR_Hists srcHists[4];

    const int ch_B[] = { 0 }, ch_G[] = { 1 }, ch_R[] = { 2 };
    int histSize = 256;
    float range[] = { 0.0, 256.0 };
    const float* histRange[] = { range };
    bool uniform = true;
    bool accumulate = false;

    //각 패치별 히스토그램 생성, normalize
    int hist_w = 512, hist_h = 400;
    int bin_w = cvRound((double)hist_w / histSize);
    Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

    vector<Mat> bgr_planes;
    for (int i = 0; i < 4; i++) {
        split(trgMats[i], bgr_planes);
        calcHist(&bgr_planes[0], 1, 0, Mat(), trgHists[i].b_hist, 1, &histSize, histRange);
        calcHist(&bgr_planes[1], 1, 0, Mat(), trgHists[i].g_hist, 1, &histSize, histRange);
        calcHist(&bgr_planes[2], 1, 0, Mat(), trgHists[i].r_hist, 1, &histSize, histRange);

        normalize(trgHists[i].b_hist, trgHists[i].b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
        normalize(trgHists[i].g_hist, trgHists[i].g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
        normalize(trgHists[i].r_hist, trgHists[i].r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

        split(srcMats[i], bgr_planes);
        calcHist(&bgr_planes[0], 1, 0, Mat(), srcHists[i].b_hist, 1, &histSize, histRange);
        calcHist(&bgr_planes[1], 1, 0, Mat(), srcHists[i].g_hist, 1, &histSize, histRange);
        calcHist(&bgr_planes[2], 1, 0, Mat(), srcHists[i].r_hist, 1, &histSize, histRange);

        normalize(srcHists[i].b_hist, srcHists[i].b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
        normalize(srcHists[i].g_hist, srcHists[i].g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
        normalize(srcHists[i].r_hist, srcHists[i].r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());


    }


    //calculate squared error and match
    double sqErr;
    double tempErr;
    Mat sqErrMat_B, sqErrMat_G, sqErrMat_R;
    int matchSave[4] = { 0, };

    for (int i = 0; i < 4; i++) {
        sqErr = 999999999999999999.9999999;
        sqErrMat_B.setTo(0);
        sqErrMat_G.setTo(0);
        sqErrMat_R.setTo(0);
        for (int j = 0; j < 4; j++) {
            pow(trgHists[i].b_hist - srcHists[j].b_hist, 2, sqErrMat_B);
            pow(trgHists[i].g_hist - srcHists[j].g_hist, 2, sqErrMat_G);
            pow(trgHists[i].r_hist - srcHists[j].r_hist, 2, sqErrMat_R);
            tempErr = sum(sqErrMat_B + sqErrMat_G + sqErrMat_R)[0];
            
            if (tempErr < sqErr) {
                sqErr = tempErr;
                matchSave[i] = j;
            }
        }
        cout << "The minimum squared Error is " << sqErr << endl;
        cout << "Patch #" << i << " of the Image 1 matches to" << "Patch #" << matchSave[i] << "of the Image 2." << endl;
        cout << endl;
    }

    //결과 출력
    cout << "RESULT:" << endl;
    cout << "Img1 Point 0" << " <~~~~> " << "Img2 Point " << matchSave[0] << '\n';
    cout << "Img1 Point 1" << " <~~~~> " << "Img2 Point " << matchSave[1] << '\n';
    cout << "Img1 Point 2" << " <~~~~> " << "Img2 Point " << matchSave[2] << '\n';
    cout << "Img1 Point 3" << " <~~~~> " << "Img2 Point " << matchSave[3] << '\n';


        return 0;
}

void on_mouse(int event, int x, int y, int falgs, void* userdata) {
    struct Userdata* data = ((struct Userdata*)userdata);
    vector<Mat>* storeHere = (data->ROIs);
    Mat* img = (data->img);

    

    switch (event) {

    case EVENT_LBUTTONDOWN:
        break;
    case EVENT_LBUTTONUP:
        cout << "(" << x << "," << y << ")" << "is Clicked and saved as a patch!" << endl;
        if (img->cols < x + PATCH_SIDE_LEN) {
            cout << "x value reduced by PATCH_SIDE_LEN" << endl;
            x -= PATCH_SIDE_LEN;
        }
        if (img->rows < y + PATCH_SIDE_LEN) {
            cout << "y value reduced by PATCH_SIDE_LEN" << endl;
            y -= PATCH_SIDE_LEN;
        }
        (*storeHere).push_back((*img)(Rect(x, y, PATCH_SIDE_LEN, PATCH_SIDE_LEN)));
        
        break;

    }

    return;
}

#include<opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main() {

    Mat image = imread("C:\\Users\\admin\\livshits_a_b\\prj\\lab_3\\cross.png", IMREAD_GRAYSCALE);
    imshow("input image", image);
    imwrite("C:\\Users\\admin\\livshits_a_b\\prj\\lab_3\\cross_gray.png", image);
    
    array<int, 256> hist;
    hist.fill(0);
    for (int irows = 0; irows < image.rows; ++irows) {
        for (int icols = 0; icols < image.cols; ++icols) {
            ++hist[image.at<uchar>(Point(icols, irows))];
        }
    }

    int max_hist = 0;

    for (int ind = 0; ind < 256; ++ind) {
        if (hist[ind] > max_hist) {
            max_hist = hist[ind];
        }
    }
    
    Mat hist_im(Mat::zeros(256, 256, CV_8UC1));

    for (int ind = 0; ind < 256; ++ind) {
        line(hist_im, Point(ind, 255), Point(ind, 255 - hist[ind] * 255 / max_hist), Scalar(255));
    }
    imshow("Histogram", hist_im);

    waitKey(0);
    return 0;
}
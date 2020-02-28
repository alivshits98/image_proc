#include<opencv2/opencv.hpp>
using namespace cv;

int main() {
    //number of rows and columns in image
    int rows = 60,
        cols = 26 * 30;

    //image (cols x rows) size, filled with zeros
    Mat image(Mat::zeros(rows, cols, CV_8UC1));

    //filling image with "step" gradient (step = 10, from 5 to 255, each level has 30 pixels in width)

    int intensity = 5, //intensity of pixel
        step = 30; //step of changing of intensity
    for (int icol = 0; icol < cols; icol += step) {
        image(Rect(icol, 0, step, rows)) = intensity;
        intensity += 10;
    }

    //visualizing image
    imshow("Grey step-gradient image", image);

    //gamma-correction of image
    Mat image_corr;
    image.convertTo(image_corr, CV_64FC1, 1.0 / 255);
    pow(image_corr, 2.24, image_corr);
    image_corr.convertTo(image_corr, CV_8UC1, 256);

    //copying corrected image below the gradient image
    Mat roi = image_corr(Rect(0, rows / 2, cols, rows / 2));
    roi.copyTo(image(Rect(0, rows / 2, roi.cols, roi.rows)));

    //visualising result
    imshow("Upper: previous image, Lower: gamma-corrected image", image);

    waitKey(0);
    return 0;
}
#include<opencv2/opencv.hpp>
#include<vector>

using namespace cv;

int main() {

    Mat image = imread("C:\\Users\\admin\\livshits_a_b\\prj\\lab_2\\cross_100.png");

    imwrite("C:\\Users\\admin\\livshits_a_b\\prj\\lab_2\\cross_95.jpeg", image, { IMWRITE_JPEG_QUALITY, 95 });
    imwrite("C:\\Users\\admin\\livshits_a_b\\prj\\lab_2\\cross_65.jpeg", image, { IMWRITE_JPEG_QUALITY, 65 });

    Mat image_95 = imread("C:\\Users\\admin\\livshits_a_b\\prj\\lab_2\\cross_95.jpeg");
    Mat image_65 = imread("C:\\Users\\admin\\livshits_a_b\\prj\\lab_2\\cross_65.jpeg");

    std::vector<Mat> channels;
    split(image, channels);

    Mat output = Mat(Mat::zeros(image.rows * 3, image.cols * 4, CV_8UC3));

    //imshow("100% quality image", image);

    //imshow("95% quality image", image_95);

    //imshow("65% quality image", image_65);



    waitKey(0);
    return 0;
}
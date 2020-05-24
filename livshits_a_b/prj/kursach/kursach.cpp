#define _CRT_SECURE_NO_WARNINGS
#include<vector>
#include<iostream>
#include<sstream>
#include<random>
#include<opencv2/opencv.hpp>
#include<opencv2/xfeatures2d.hpp>
#include<cmath>

using namespace cv;
using namespace std;

typedef unsigned long long ull;

struct pt {
    double x, y;
    pt(double x = 0, double y = 0) :x(x), y(y) {
    }
    double dist2() {
        return x * x + y * y;
    }
    pt operator+(pt a) { return pt(x + a.x, y + a.y); }
    pt operator*(double a) { return pt(x * a, y * a); }
    pt operator/(double a) { return pt(x / a, y / a); }
};

struct my_line {

    double a, b, c;

    my_line() {
    }
    my_line(pt m, pt n) {
        a = n.y - m.y;
        b = m.x - n.x;
        c = -m.x * n.y + m.y * n.x;
    }
    my_line(double a, double b, double c) :a(a), b(b), c(c) {
    }
    double operator[](pt p) { return a * p.x + b * p.y + c; }
};

double cross(pt a, pt b) {
    return a.x * b.y - a.y * b.x;
}

pt intersect(pt a, pt b, my_line li) {
    pt l = a, r = b;
    for (int i = 0; i < 50; ++i) {
        pt mid1 = (l * 2 + r) / 3;
        pt mid2 = (l + r * 2) / 3;
        double res1 = li[mid1];
        double res2 = li[mid2];
        if (abs(res1) < abs(res2)) r = mid2;
        else l = mid1;
    }
    return l;
}

pt intersect1(my_line l1, my_line l2) {
    pt  n1(l1.a, l1.b),
        n2(l2.a, l2.b),
        k(l1.a, l1.c),
        l(l2.a, l2.c),
        p(l1.b, l1.c),
        q(l2.b, l2.c);
    return pt(cross(p, q) / cross(n1, n2), -cross(k, l) / cross(n1, n2));
}

//k - number of pixels out of border
//numeration of points is clockwise!!
vector<Point2f> widen_border(const vector<Point>& corners, double k) {
    //converting corners into pt
    vector<pt>corner_pts(corners.size());
    for (int ind = 0; ind < corners.size(); ++ind) {
        corner_pts[ind] = pt(corners[ind].x, corners[ind].y);
    }

    //find centre of figure
    //pt centre = intersect(my_line(corner_pts[0], corner_pts[2]), my_line(corner_pts[1], corner_pts[3]));
    pt centre = intersect(corner_pts[0], corner_pts[2], my_line(corner_pts[1], corner_pts[3]));

    //new corner points
    //vector<pair<int, int>> res;
    vector<Point2f> res;
    for (int ind = 0; ind < corner_pts.size(); ++ind) {
        res.push_back(Point2f(round((corner_pts[ind].x - centre.x) * (1 + k)), round((corner_pts[ind].y - centre.y) * (1 + k))));
    }
    return res;
}

//res_corners are: (0, 0), (600, 0), (600, 600), (0, 600) //(cols, rows)
Mat make_unperspective(const Mat& image, const vector<Point>& corners, const vector<Point2f>& res_corners, double k) {
    Mat img;

    image.copyTo(img);

    fillConvexPoly(img, corners, Scalar(0, 0, 0));

    vector<Point2f> new_corners;
    new_corners = widen_border(corners, k);

    //Mat H = findHomography(res_corners, new_corners);
    Mat H = findHomography(new_corners, res_corners);
    Mat dest_image;
    warpPerspective(img, dest_image, H, img.size(), INTER_LINEAR /*+ WARP_INVERSE_MAP*/);
    Mat res_image;
    dest_image.copyTo(res_image);
    return res_image;
}

ull dhash(Mat image, int hash_size = 8) {
    Mat changed;
    cvtColor(image, changed, COLOR_BGR2GRAY);
    resize(changed, changed, Size(hash_size + 1, hash_size));
    int rows = 8,
        cols = 8;
    ull diff_hash = 0;
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            diff_hash <<= 1;
            diff_hash |= 1 * (image.at<uchar>(col, row) < image.at<uchar>(col + 1, row));
        }
    }
    return diff_hash;
}

int count_bits(ull number) {
    int res = 0;
    while (number) {
        ++res;
        number &= number - 1;
    }
    return res;
}

int hamming_dist(ull img_hash_1, ull img_hash_2) {
    ull res = img_hash_1 ^ img_hash_2;
    return count_bits(res);
}

//vector<Mat>& generator(const vector<Mat>& etalon_data, int num_per_image) {
//    //random generator for chosing operation
//    //for dataset preparation
//    random_device rd;
//    mt19937 gen(rd());
//    uniform_int_distribution<> dis(1, 3);
//    int operation = 0;
//
//    int et_num = etalon_data.size();
//
//    for (int item = 0; item < et_num; ++item) {
//        for (int attempt = 0; attempt < num_per_image; ++attempt) {
//            //1 - rotated image
//            //2 - scaled image
//            //3 - rotated & scaled image
//            operation = dis(gen);
//            if (operation == 1) {
//                continue;
//            }
//            if (operation == 2) {
//                continue;
//            }
//            if (operation == 3) {
//                continue;
//            }
//        }
//    }
//}

//vector<pair<int, int>> vert_obj;
//vector<pair<int, int>> vert_wide;
//int count_pts = 0;
//Mat im;
//
//void mouse_callback(int  event, int  x, int  y, int  flag, void* param) {
//    static int curx, cury;
//    if (event == EVENT_LBUTTONDOWN) {
//        ++count_pts;
//        if (count_pts <= 4) {
//            vert_obj.push_back({ x, y });
//            if (count_pts == 4) {
//            }
//        }
//        else {
//            vert_wide.push_back({ x, y });
//        }
//        cout << "(" << x << ", " << y << ")" << endl;
//        imshow("example", pic);
//        
//    }
//}

int main() {
    //file with paths of Etalon dataset
    freopen("markdown.txt", "r", stdin);

    //vector<vector<pair<int, int>>> vertices;
    vector<vector<Point>> vertices;
    vector<Mat> images;

    int num_img;
    cin >> num_img;
    vertices.resize(num_img);
    images.resize(num_img);

    string path;
    for (int img = 0; img < num_img; ++img) {
        int x, y;
        cin >> path;
        images[img] = imread(path);
        for (int vert = 0; vert < 4; ++vert) {
            cin >> x >> y;
            //vertices[img].push_back({ x, y });
            vertices[img].push_back(Point(x, y));
        }
        polylines(images[img], vertices[img], true, Scalar(0, 0, 255), 2);
        imshow("Image", images[img]);
        waitKey(0);
    }
    imwrite("markdown_2.png", images[1]);

    vector<Point2f> res_vert(4);
    res_vert[0] = Point2f(0, 200);
    res_vert[1] = Point2f(0, 0);
    res_vert[2] = Point2f(200, 0);
    res_vert[3] = Point2f(200, 200);

    vector<Mat> unpersp_img(num_img);
    for (int img = 0; img < num_img; ++img) {
        unpersp_img[img] = make_unperspective(images[img], vertices[img], res_vert, 0.5);
        imshow("Unperspective image", unpersp_img[img]);
        waitKey(0);
    }


    ////number of images in Etalon dataset
    //int et_num;
    //cin >> et_num;

    //vector<Mat> etalon_data;

    //string path;
    //for (int item = 0; item < et_num; ++item) {
    //    cin >> path;
    //    etalon_data.push_back(imread(path));
    //}

    ////generating dataset
    //vector<Mat> data;
    ////for (int item = 0; item < et_num; ++item) {
    ////    //original image
    ////    data.push_back(etalon_data[item]);


    ////    //visualizing image

    ////    //rotated image


    ////    //scaled image

    ////    cout << 5 << endl;
    ////    //rotated and scale image


    ////}

    //cout << etalon_data[0].rows << " " << etalon_data[0].cols << endl;
    //int rows = etalon_data[0].rows,
    //    cols = etalon_data[0].cols;

    ////Mat output = Mat(rows, cols, CV_8UC3);
    ////etalon_data[0].copyTo(output(Rect(0, 0, cols, rows)));
    ////imshow("Etalon dataset image", output);
    //imshow("", etalon_data[0]);

    //waitKey(0);
    return 0;
}

//int res = 0;
//while (img_hash_1 != 0) {
//    res += ~((img_hash_1 & 1ULL) ^ (img_hash_2 & 1ULL));
//    img_hash_1 >>= 1;
//    img_hash_2 >>= 1;
//}

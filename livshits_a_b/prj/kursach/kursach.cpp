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
    vector<ull> percept_hashes(num_img);
    for (int img = 0; img < num_img; ++img) {
        unpersp_img[img] = make_unperspective(images[img], vertices[img], res_vert, 0.5);
        imshow("Unperspective image", unpersp_img[img]);
        percept_hashes[img] = dhash(unpersp_img[img]);
        waitKey(0);
    }

    for (int img1 = 0; img1 < num_img; ++img1) {
        for (int img2 = 1; img2 < num_img; ++img2) {
            if (img1 == img2) {
                continue;
            }
            int hamm_dist = hamming_dist(percept_hashes[img1], percept_hashes[img2]);
            if (hamm_dist == 0) {
                cout << "Background near images " << img1 << "and " << img2 << "is equal!" << endl;
                continue;
            }
            if (hamm_dist <= 10) {
                cout << "Background near images " << img1 << "and " << img2 << "is similar" << endl;
                continue;
            }
        }
    }

    return 0;
}

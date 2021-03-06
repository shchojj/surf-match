// CameraCalibration.cpp: 定义控制台应用程序的入口点。
//
#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <Devignetting.h>
#include <opencv2/xfeatures2d/nonfree.hpp>


using namespace std;
using namespace cv;
using namespace xfeatures2d;
void match()
{
	//载入源图像，显示并转换灰度图
	Mat srcImage1 = imread("D:\\WorkSpace\\Images\\FS0588_20180821\\f4.bmp", 0);
	Mat srcImage2 = imread("D:\\WorkSpace\\Images\\FS0588_20180821\\pattern.bmp", 0);




	int minHessian = 4000;
	Ptr<SurfFeatureDetector> detector = SurfFeatureDetector::create(minHessian);
	vector<KeyPoint> key_points_1, key_points_2;

	Mat dstImage1, dstImage2;
	detector->detectAndCompute(srcImage1, Mat(), key_points_1, dstImage1);
	detector->detectAndCompute(srcImage2, Mat(), key_points_2, dstImage2);//可以分成detect和compute

	Mat img_keypoints_1, img_keypoints_2;
	drawKeypoints(srcImage1, key_points_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	drawKeypoints(srcImage2, key_points_2, img_keypoints_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	/*Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("FlannBased");
	vector<DMatch>mach;

	matcher->match(dstImage1, dstImage2, mach);
	double Max_dist = 0;
	double Min_dist = 100;
	for (int i = 0; i < dstImage1.rows; i++)
	{
		double dist = mach[i].distance;
		if (dist < Min_dist)Min_dist = dist;
		if (dist > Max_dist)Max_dist = dist;
	}
	cout << "最短距离" << Min_dist << endl;
	cout << "最长距离" << Max_dist << endl;

	vector<DMatch>goodmaches;
	for (int i = 0; i < dstImage1.rows; i++)
	{
		if (mach[i].distance < 4 * Min_dist&&mach[i].distance >2 * Min_dist)
			goodmaches.push_back(mach[i]);
	}
	Mat img_maches;
	drawMatches(srcImage1, key_points_1, srcImage2, key_points_2, goodmaches, img_maches);

	for (int i = 0; i < goodmaches.size(); i++)
	{
		cout << "符合条件的匹配：" << goodmaches[i].queryIdx << "--" << goodmaches[i].trainIdx << endl;
	}*/

#pragma region 对齐
	// Match features.
	std::vector<DMatch> matches;
	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("FlannBased");
	matcher->match(dstImage1, dstImage2, matches);
	// Sort matches by score
	std::sort(matches.begin(), matches.end());
	// Remove not so good matches
	const int numGoodMatches = matches.size() * 0.8;
	matches.erase(matches.begin() + numGoodMatches, matches.end());
	// Draw top matches
	Mat imMatches;
	drawMatches(srcImage1, key_points_1, srcImage2, key_points_2, matches, imMatches);

	//imwrite("matches.jpg", imMatches);
	// Extract location of good matches
	std::vector<Point2f> points1, points2;
	for (size_t i = 0; i < matches.size(); i++)
	{
		points1.push_back(key_points_1[matches[i].queryIdx].pt);
		points2.push_back(key_points_2[matches[i].trainIdx].pt);
	}
	// Find homography
	Mat h = findHomography(points1, points2, RANSAC);
	// Use homography to warp image
	Mat im1Reg;
	warpPerspective(srcImage1, im1Reg, h, srcImage2.size());

	Mat thresholdMat1, thresholdMat2;
	threshold(im1Reg, thresholdMat1, 0, 255, THRESH_OTSU);
	threshold(srcImage2, thresholdMat2, 0, 255, THRESH_OTSU);
	Mat subMat;
	absdiff(thresholdMat2, thresholdMat1, subMat);
	imwrite("save.jpg", subMat);
#pragma endregion


	namedWindow("效果图1", 0);
	resizeWindow("效果图1", 640, 480);
	imshow("效果图1", img_keypoints_1);
	namedWindow("效果图2", 0);
	resizeWindow("效果图2", 640, 480);
	imshow("效果图2", img_keypoints_2);
	namedWindow("匹配效果", 0);
	resizeWindow("匹配效果", 640, 480);
	imshow("匹配效果", imMatches);
	namedWindow("对齐效果", 0);
	resizeWindow("对齐效果", 640, 480);
	imshow("对齐效果", im1Reg);

	namedWindow("差分图", 0);
	resizeWindow("差分图", 640, 480);
	imshow("差分图", subMat);
}
void main() 
{
	string imgPath = "flickr_3.jpg";

	Mat src = imread(imgPath, 0);
	//if (src.type() == CV_8UC1)
	//	cvtColor(src, src, CV_GRAY2BGR);
	Mat dst(src.size(), CV_8UC1, Scalar(0));

	namedWindow("src", 0);
	resizeWindow("src", 640, 480);
	imshow("src", src);

	DevignettingImg(src, dst);
	imwrite("devignetting.jpg", dst);
	cout << "结束了" << endl;

	namedWindow("devignetting", 0);
	resizeWindow("devignetting", 640, 480);
	imshow("devignetting", dst);
	waitKey();
	destroyAllWindows();


	
	

	waitKey(0);


	getchar();
}
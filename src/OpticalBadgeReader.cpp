#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat blueFilter(const Mat& src)
{
  //
  assert(src.type() == CV_8UC3);
  Mat hsv;
  cvtColor(src, hsv, COLOR_BGR2HSV);
  Scalar lower_blue = Scalar(110,50,50);
  Scalar upper_blue = Scalar(130,255,255);
  Mat blueOnly;
  inRange(hsv, lower_blue, upper_blue, blueOnly);
  return blueOnly;
}

int main(int argc, char* argv[])
{
  Mat image;

	/* use either image from command line or capture device */
  if (2 == argc) {
    image = imread(argv[1], 1);
  } else {
    VideoCapture capture(0);
    if(!capture.isOpened()) {
      cerr << "Could not create capture";
      return -1;
    }
    Mat frame;
    capture.read(frame);
    frame.copyTo(image);
    capture.release();
  }

  if (!image.data) {
	  cerr << "No image data\n";
	  return -1;
  }

  /* # find contours http://docs.opencv.org/doc/tutorials/imgproc/shapedescriptors/find_contours/find_contours.html
   * # create binary thresholded image
   * # detect edges
   * # find polygons
   * # detect 2 (blue) quadrilaterals
   * # use largest as a mask
   * # extract face */
  Mat blueOnly = blueFilter(image);

  // Find contours
  Mat edges;
  int thresh = 100;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  blur(blueOnly, blueOnly, Size(3,3));
  Canny(blueOnly, edges, thresh, thresh*2, 3);
  findContours(edges, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

  // Approximate contours to polygons to get bounding boxes
  vector<vector<Point> > contours_poly( contours.size() );
  vector<Rect> boundRect( contours.size() );
  for (unsigned int i = 0; i < contours.size(); i++ ) {
    approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
    boundRect[i] = boundingRect( Mat(contours_poly[i]) );
  }

  // Draw largest blue quadrilateral
  int largest_area = 0;
  int largest_contour = -1;
  for( unsigned int i = 0; i < contours.size(); i++ )
  {
    if (boundRect[i].area() < 5000)
      continue;
    if (boundRect[i].area() > largest_area)
     largest_contour = i;
  }
  if (largest_contour > 0) {
    rectangle(image, boundRect[largest_contour].tl(), boundRect[largest_contour].br(), Scalar(0,255,0), 2, 8, 0);
  }

  // Draw contours
  // http://docs.opencv.org/doc/tutorials/imgproc/shapedescriptors/bounding_rects_circles/bounding_rects_circles.html
  // http://opencvexamples.blogspot.com/2013/09/find-contour.html
  Mat drawing = Mat::zeros( edges.size(), CV_8UC3 );
  RNG rng(12345);
  for( unsigned int i = 0; i < contours.size(); i++ )
  {
    // use a random color
    Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
    //drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
    rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
  }

  namedWindow("Contours", CV_WINDOW_AUTOSIZE);
  imshow("Contours", drawing);

  namedWindow("Input Image", WINDOW_AUTOSIZE);
  imshow("Input Image", image);

  namedWindow("Blue Image", WINDOW_AUTOSIZE);
  imshow("Blue Image", blueOnly);


  waitKey(0);
  destroyAllWindows();
  return 0;
}

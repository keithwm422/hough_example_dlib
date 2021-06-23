// The contents of this file are in the public domain. See LICENSE_FOR_EXAMPLE_PROGRAMS.txt
// ORIGINAL FILE comments
/*

    This is an example illustrating the use of the Hough transform tool in the
    dlib C++ Library.


    In this example we are going to draw a line on an image and then use the
    Hough transform to detect the location of the line.  Moreover, we do this in
    a loop that changes the line's position slightly each iteration, which gives
    a pretty animation of the Hough transform in action.
*/
// Keith editing this for his example of random points and then drawing those lins in Hough space
#include <dlib/gui_widgets.h>
#include <dlib/image_transforms.h>
//#include <stdlib.h>
#include <iostream>
//#include <time.h>
#include <random>
#include <unistd.h>
#include <cmath>
using namespace std;
using namespace dlib;
#define SIZE_I 400
#define SIZE_N 400

void make_pixel_coordinate(double x_original, double y_original, long& x, long& y); // idea would be to input coordinates with origin as the center and then construct the image with the output of this function (to pixel image coordinate space)
void make_original_coordinate(double& x_original, double& y_original, long x_pixel, long y_pixel); // and the inverse
void calculate_line_properties(dpoint p1, dpoint p2, double& m, double& b); // once two of the points to construct line are made from an HT, then can find the slope and intercept of the line
void calculate_line_properties_pixel(dpoint p1, dpoint p2, double& m, double& b); // once two of the points to construct line are made from an HT, then can find the slope and intercept of the line- values in pixel space
double calculate_slope(double x1,double y1, double x2, double y2);
int main()
{
    // First let's make a 400x400 image.  This will form the input to the Hough transform.
    array2d<unsigned char> img(SIZE_I,SIZE_I);
    array2d<unsigned char> img2(400,400); // this one for data points maybe?
    // Now we make a hough_transform object.  The 300 here means that the Hough transform
    // will operate on a 300x300 subwindow of its input image.
    int ht_image_size=300;
    hough_transform ht(ht_image_size); // 300 is size of the square hough transform region of the input "image"
    cout << "starting" << endl;
    cout << "_size is : " << ht.size() << endl;
    image_window win, win2;
    //srand (time(NULL));
    //int random_num=std::rand();
    //cout << random_num << endl;
    double mean=0;
    double std_dev=0.5;
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(mean,std_dev);
    double m=0;
    double b = 0;
    double m_calc=0;
    double b_calc = 0;
    double m_pixel=0; // slope in pixel space of original line
    double b_pixel = 0;
    double first_x=0;
    double first_y=0;
    double last_x=0;
    double last_y=0;
    double x[SIZE_N]={0};
    int start_x=-1*(int)(SIZE_I)/2;
    int end_x=(int)(SIZE_I)/2;
    double y[SIZE_N]={0}; // storing the data points (there will be supposedly 216 sense wires)
    long pixel_x[SIZE_N]={0};
    long pixel_y[SIZE_N]={0};
    double hit_size=1.0;
    point p1;
    double ht_r=0; // these are output by the HT object in get_line_properties and in the coordinate system with center of the image as the origin
    double ht_phi=0; // in degrees
    double ht_x=0; // x coordinate of HT image point given in x coordinate of HT space
    double ht_y=0; // y coordinate of HT image point given in y coordinate of HT space
    double ht_m=0; // calculated original slope from HT image-value given in coordinates of original space
    double ht_b=0; // calculated intercept from HT image-value given in coordinates of original space
    double ht_m_pixel=0;
    double ht_b_pixel=0;
    cout << (long) p1.x() << endl;
    double angle1 = 0;
    double angle2 = 0;
    int i=0;
    unsigned int sleepy_time=3;
    dpoint center_p(SIZE_I/2,SIZE_I/2);
        // Generate a line segment that is rotating around inside the image.  The line is
        // generated based on the values in angle1 and angle2. So each iteration creates a
        // slightly different line.
    while(i<10){
        angle1 += pi/130;
        angle2 += pi/400;
        const point cent = center(get_rect(img));  
        // A point 90 pixels away from the center of the image but rotated by angle1.
        const point arc = rotate_point(cent, cent + point(90,0), angle1); 
        // Now make a line that goes though arc but rotate it by angle2.
        const point l = rotate_point(arc, arc + point(500,0), angle2);
        const point r = rotate_point(arc, arc - point(500,0), angle2);

        // Next, blank out the input image and then draw our line on it.
        assign_all_pixels(img, 0);
        //draw_line(img, l, r, 255);
        //draw_line(img, p1, p1+50, 200);
        dpoint hits(50,50);
        for(int i=0;i<SIZE_I;i++){
          x[i]=(double) (i+start_x);
          y[i]= m*x[i] + b + distribution(generator);
          make_pixel_coordinate(x[i],y[i],pixel_x[i],pixel_y[i]);
          dpoint hit_point(pixel_x[i],pixel_y[i]);
          draw_solid_circle(img,hit_point,hit_size,200);
          if(i==0){
            first_x=x[i];
            first_y=m*x[i]+b;
          }
        }
        last_x=x[SIZE_I-1];
        last_y=m*x[SIZE_I-1]+b;
        m_calc=calculate_slope(first_x,first_y,last_x,last_y);
        const point offset(50,50); // because ht image size was 300 and real image size was 400?
        array2d<int> himg;
        // pick the window inside img on which we will run the Hough transform.
        const rectangle box = translate_rect(get_rect(ht),offset);
        // Now let's compute the hough transform for a subwindow in the image.  In
        // particular, we run it on the 300x300 subwindow with an upper left corner at the
        // pixel point(50,50).  The output is stored in himg.
        ht(img, box, himg);
        cout << "ht size x size is: " << ht.size() << endl;
        dpoint hit_test(200,200);
        draw_solid_circle(img,hit_test,hit_size,200);

        // Now that we have the transformed image, the Hough image pixel with the largest
        // value should indicate where the line is.  So we find the coordinates of the
        // largest pixel:
        dpoint p = max_point(mat(himg));
        // And then ask the ht object for the line segment in the original image that
        // corresponds to this point in Hough transform space.
        std::pair<point,point> line = ht.get_line(p);
        ht.get_line_properties(p,ht_phi,ht_r);
        ht.get_line_properties_friendly(p,ht_phi,ht_r,ht_x,ht_y);
        // Finally, let's display all these things on the screen.  We copy the original
        // input image into a color image and then draw the detected line on top in red.
        array2d<rgb_pixel> temp;
        assign_image(temp, img);
        // Note that we must offset the output line to account for our offset subwindow.
        // We do this by just adding in the offset to the line endpoints. 
        draw_line(temp, line.first+offset, line.second+offset, rgb_pixel(255,0,0));
        draw_solid_circle(temp,line.first+offset,4.0*hit_size,50);
        draw_solid_circle(temp,line.second+offset,4.0*hit_size,50);
        cout << "line pairs are : " << line.first << " " << line.second << endl;
        draw_solid_circle(temp,center_p,2.0*hit_size,75);
        win.clear_overlay();
        win.set_image(temp);
        // Also show the subwindow we ran the Hough transform on as a green box.  You will
        // see that the detected line is exactly contained within this box and also
        // overlaps the original line.
        win.add_overlay(box, rgb_pixel(0,255,0));

        // We can also display the Hough transform itself using the jet color scheme.
        draw_solid_circle(himg,p,10*hit_size,150);  // and draw a circle around that point in the ht image
        win2.set_image(jet(himg));
        cout << "point p: " << p << endl;
        cout << "HT slope: " << p.x() << " and HT intercept: " << p.y() << endl;
        cout << "HT phi: " << ht_phi << " and HT r: " << ht_r << endl;
        cout << "HT x: " << ht_x << " and HT y: " << ht_y << endl;
        calculate_line_properties(line.first+offset,line.second+offset,ht_m,ht_b);
        calculate_line_properties_pixel(line.first+offset,line.second+offset,ht_m_pixel,ht_b_pixel);
        cout << "real slope: " << m << " and real intercept: " << b << endl;
        cout << "calc slope-HT: " << ht_m << " and calc intercept-HT: " << ht_b << endl;
        cout << "calc slope: " << m_calc << " and calc intercept: " << ht_b << endl;
        cout << "real slope-p: " << m_pixel << " and real intercept-p: " << b_pixel << endl;
        cout << "calc slope-p: " << ht_m << " and calc intercept-p: " << ht_b_pixel << endl;
        i++;
        b+=10.0;
        m+=-0.05;
        sleep(sleepy_time);
    }
}


void make_pixel_coordinate(double x_original, double y_original, long& x, long& y){
  x=lround((double)(SIZE_I)/2.0 + x_original);
  y=lround((double)(SIZE_I)/2.0 - y_original);

}
void make_original_coordinate(double& x_original, double& y_original, long x_pixel, long y_pixel){
  x_original=(double) (x_pixel)-(double)(SIZE_I)/2.0;
  y_original=((double)(SIZE_I)/2.0) - (double)(y_pixel);

}

void calculate_line_properties(dpoint p1, dpoint p2, double& m, double& b){
  long x1=p1.x(); // x coord in pixel space
  long y1=p1.y(); // y coord in pixel space
  long x2=p2.x();
  long y2=p2.y();
  double x1_o=0; // for transforming to original cooridnates
  double y1_o=0;
  double x2_o=0;
  double y2_o=0;
  make_original_coordinate(x1_o,y1_o,x1,y1);
  make_original_coordinate(x2_o,y2_o,x2,y2);
  m=calculate_slope(x1_o,y1_o,x2_o,y2_o);
  b=y2_o-m*x2_o;
}
void calculate_line_properties_pixel(dpoint p1, dpoint p2, double& m, double& b){
  long x1=p1.x(); // x coord in pixel space
  long y1=p1.y(); // y coord in pixel space
  long x2=p2.x();
  long y2=p2.y();
  m=(y2-y1)/(x2-x1);
  b=y2-m*x2;
}
double calculate_slope(double x1,double y1, double x2, double y2){
  return (y2-y1)/(x2-x1);
}

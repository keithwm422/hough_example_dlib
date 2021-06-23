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
// Keith editing this for his example of HT on a DCT display image of the track of muon
#include <dlib/gui_widgets.h>
#include <dlib/image_transforms.h>
#include <dlib/image_processing/generic_image.h>
#include <dlib/image_io.h>
//#include <stdlib.h>
#include <iostream>
//#include <time.h>
#include <random>
#include <unistd.h>
#include <cmath>
using namespace std;
//using namespace dlib;
#define SIZE_I 1000
#define SIZE_N 1000

void make_pixel_coordinate(double x_original, double y_original, long& x, long& y); // idea would be to input coordinates with origin as the center and then construct the image with the output of this function (to pixel image coordinate space)
void make_original_coordinate(double& x_original, double& y_original, long x_pixel, long y_pixel); // and the inverse
void calculate_line_properties(dlib::dpoint p1, dlib::dpoint p2, double& m, double& b); // once two of the points to construct line are made from an HT, then can find the slope and intercept of the line
void calculate_line_properties_pixel(dlib::dpoint p1, dlib::dpoint p2, double& m, double& b); // once two of the points to construct line are made from an HT, then can find the slope and intercept of the line- values in pixel space
double calculate_slope(double x1,double y1, double x2, double y2);
int main()
{
    // First let's make a 1000x1000 image.  This will load the image (and change the image size to fit the image exactly).
    //dlib::array2d<unsigned char> img_temp_o(SIZE_I,SIZE_I);
    dlib::array2d<unsigned char> * img_temp;
    img_temp = new dlib::array2d<unsigned char>(SIZE_I,SIZE_I);
    dlib::assign_all_pixels(*img_temp, 0);
    dlib::load_image(*img_temp,"/home/keith/images/keith_test_HT_uniform_color.png");
    //void * img_temp_data = dlib::image_data(*(img_temp));
//    load_image(img_temp,"/home/keith/images/ev14.png");
    //image_window win_test;
    //win_test.clear_overlay();
    /*win_test.set_image(img_temp);
    point ptemp_debug;
    while (win_test.get_next_double_click(ptemp_debug)){
      cout << "User double clicked on pixel:         " << ptemp_debug << endl;
      cout << "edge pixel value at this location is: " << (int)img_temp[ptemp_debug.y()][ptemp_debug.x()] << endl;
    }*/
    //assign_image(img_temp, img);
    //int num_image_rows=img_temp.nr();
    //int num_image_columns=img_temp.nc();
    int num_image_columns=img_temp->nc();
    int num_image_rows=img_temp->nr();
    // we can use these coordinates to create the image on a bigger, square, blank pixel space image. then
    int biggest=num_image_rows;

    if(num_image_rows<= num_image_columns) biggest=num_image_columns;

    int ht_image_size=0;

    if(biggest % 2 == 0) ht_image_size=biggest;

    else ht_image_size=biggest-1;
    int image_size_new=ht_image_size+10;
    dlib::array2d<unsigned char> img(image_size_new,image_size_new); // this will be for making a square box around the region. Since HT is a square, this needs to have the image centered on huge array of pixels so the square can encompass the whole image.
    dlib::assign_all_pixels(img, 0);
    cout << "image_temp num rows: " << num_image_rows << endl;
    cout << "image_temp num columns: " << num_image_columns << endl;
    cout << "image num rows: " << img.nr() << endl;
    cout << "image num columns: " << img.nc() << endl;
    cout << "biggest: " << biggest << endl;
    cout << "image_size: " << image_size_new << endl;
    cout << "ht_image_size: " << ht_image_size << endl;
    char const_gray_num = 0;
    // now for our just slightly bigger square image space (img), we can assign the values to the image that are blank (since a white background is default on the root canvas draw) by subtracting 255 from them. Since image space is 1byte pixels, this will be grayscale image
    int displaced=4;
    char to_iterate_over=0;
    for(int i1=0;i1<num_image_rows; i1++){
      for(int i2=0;i2<num_image_columns; i2++){
        //img_temp[i1][i2]+=const_gray_num;
        // *(*(img_temp+i1)+i2)+=const_gray_num;
        //cout << *(*(img_temp+i1)+i2) << endl;
        //cout << *(*(img_temp+i1)+i2) << endl;
        //to_iterate_over=(*(img_temp_data))[i1][i2];
        //cout << (*img_temp)[0][0] << endl;
        dlib::assign_pixel(img[i1+displaced][i2+displaced],(*img_temp)[i1][i2]);
        //dlib::assign_pixel(img[i1+displaced][i2+displaced],img_temp[i1][i2]);
        //dlib::assign_pixel(img[i1+displaced][i2+displaced],*(*(img_temp+i1)+i2));
      }
    } // assume this works, but could also display it on a third window if debugging
    dlib::image_window win_test2;
    win_test2.set_image(img);
    dlib::point ptemp_debug2;
    while (win_test2.get_next_double_click(ptemp_debug2)){
      cout << "User double clicked on pixel:         " << ptemp_debug2 << endl;
      cout << "edge pixel value at this location is: " << (int)img[ptemp_debug2.y()][ptemp_debug2.x()] << endl;
    }

    dlib::hough_transform ht(ht_image_size); // make the hough_transform the same size as the square img space above

    int offset_val= (image_size_new - ht_image_size)/2;
        //cout << "image num rows: " << img.nr() << endl;
        //cout << "image num columns: " << img.nc() << endl;
    // Now we make a hough_transform object.  The 300 here means that the Hough transform
    // will operate on a 300x300 subwindow of its input image.
    // find the modulus to make hough transform even size? (needs to be a square)
    cout << "starting" << endl;
    cout << "offset_val is: " << offset_val << endl;
    //cout << "_size is : " << ht.size() << endl;
    dlib::image_window win, win2;
    double m_calc=0;
    double b_calc = 0;
    double m_pixel=0; // slope in pixel space of original line
    double b_pixel = 0;
    double first_x=0;
    double first_y=0;
    double last_x=0;
    double last_y=0;
    long pixel_x[SIZE_N]={0};
    long pixel_y[SIZE_N]={0};
    double hit_size=1.0;
    dlib::point p1;
    double ht_r=0; // these are output by the HT object in get_line_properties and in the coordinate system with center of the image as the origin
    double ht_phi=0; // in degrees
    double ht_x=0; // x coordinate of HT image point given in x coordinate of HT space
    double ht_y=0; // y coordinate of HT image point given in y coordinate of HT space
    double ht_m=0; // calculated original slope from HT image-value given in coordinates of original space
    double ht_b=0; // calculated intercept from HT image-value given in coordinates of original space
    double ht_m_pixel=0;
    double ht_b_pixel=0;
    //cout << (long) p1.x() << endl;
    int i=0;
    unsigned int sleepy_time=10;
    dlib::dpoint center_p(SIZE_I/2,SIZE_I/2);
        // Generate a line segment that is rotating around inside the image.  The line is
        // generated based on the values in angle1 and angle2. So each iteration creates a
        // slightly different line.
    double threshold_fraction=0.1;
    const dlib::point cent = center(get_rect(img));
    dlib::dpoint hits(50,50); // a dumb example of declaring a point
    //m_calc=calculate_slope(first_x,first_y,last_x,last_y);
    const dlib::point offset(offset_val,offset_val); // because ht image size was 300 and real image size was 400?
    dlib::array2d<int> himg;
    dlib::assign_all_pixels(himg, 0);
    // pick the window inside img on which we will run the Hough transform.
    const dlib::rectangle box = translate_rect(get_rect(ht),offset);
    ht(img, box, himg);
    dlib::dpoint p = max_point(mat(himg)); // get the biggest valued pixel and use it to construct a threshold
    cout << "Best Hough point p: " << p << endl;
    int threshold_in=himg[p.y()][p.x()];
    int threshold_diff=(int) ((double) (threshold_in)*threshold_fraction);
    std::pair<dlib::point,dlib::point> line = ht.get_line(p);
    //std::pair<point,point> line2 = ht.get_line(best_ht_p);
    ht.get_line_properties(p,ht_phi,ht_r);
    ht.get_line_properties_friendly(p,ht_phi,ht_r,ht_x,ht_y);
    cout << "HT phi: " << ht_phi << " and HT r: " << ht_r << endl;
    cout << "HT x: " << ht_x << " and HT y: " << ht_y << endl;
    //    cout << "HT pixel x: " << p.x() << " and HT pixel y: " << p.y() << endl;
    //point ptemp;
    /*while (win2.get_next_double_click(ptemp)){
      cout << "User double clicked on pixel:         " << ptemp << endl;
      cout << "edge pixel value at this location is: " << (int)himg[ptemp.y()][ptemp.x()] << endl;
    }*/
    cout << "deleting loaded image temp array..." << endl;
    delete img_temp;
    while(i<10){
        // also make a window to display the original image
        //image_window my_window2(img2, "Original Image");

        // Next, blank out the input image and then draw our line on it.
        //load_image(img,"dct_example_mudata_track_ht.png");
        // invert or make negative

        //cout << "image size loaded: " <<  img.size() << endl;
        //draw_line(img, l, r, 255);
        //draw_line(img, p1, p1+50, 200);
        // Now let's compute the hough transform for a subwindow in the image.  In
        // particular, we run it on the 300x300 subwindow with an upper left corner at the
        // pixel point(50,50).  The output is stored in himg.
        //cout << "before ht image" << endl;
        //cout << "made ht image" << endl;
        //cout << "ht size x size is: " << ht.size() << endl;
        //dpoint hit_test(200,200);
        //draw_solid_circle(img,hit_test,hit_size,200);
        // Now that we have the transformed image, the Hough image pixel with the largest
        // value should indicate where the line is.  So we find the coordinates of the
        // largest pixel:

        // but also documentation explains the get_best_hough_point returns the point p such that 
        //cout << "himg max point" << endl;
        //point pt_of_interest(150,150);
        //cout << "pt of interest" << endl;
        //draw_solid_circle(img,pt_of_interest,4.0*hit_size,200);// draw the point to help get navigation
        //point best_ht_p = ht.get_best_hough_point(pt_of_interest, himg);
        //point best_ht_p = ht.get_best_hough_point(center_p, himg);
        //cout << "best_hp: " << best_ht_p << endl;
        // And then ask the ht object for the line segment in the original image that
        // corresponds to this point in Hough transform space.
        // get the strong hough points as a vector of points in hough space
        double angle_thresh = 0;
        double radius_thresh = 0;
        std::vector<dlib::point> hp_good_pts = ht.find_strong_hough_points(himg,threshold_in,angle_thresh,radius_thresh);
        // now for points in original image that vote for the best hough point
        std::vector<std::vector<dlib::point>>pixels_for_hp = ht.find_pixels_voting_for_lines(img,box,hp_good_pts,1,1);
        for(int k=0;k<pixels_for_hp.size();k++){
          for(int l=0; l < pixels_for_hp[k].size(); l++){
            cout << pixels_for_hp[k][l] << endl;
          }
        }
        cout << pixels_for_hp.size() << endl;
        int num_hp_good_pts = (int) hp_good_pts.size();
        cout << "found " << num_hp_good_pts << "hough points above threshold (" << threshold_in << ") : ";
        // Finally, let's display all these things on the screen.  We copy the original
        // input image into a color image and then draw the detected line on top in red.
        dlib::array2d<dlib::rgb_pixel> temp;
        dlib::assign_image(temp, img);
        // finding Hough features
        //int pixel_i=0;
        for (int pixel_i=0; pixel_i<num_hp_good_pts;pixel_i++){
          //cout << pt_i << " , ";
          dlib::point pt_i=hp_good_pts[pixel_i];
          cout << "point " << pt_i << " has value " << himg[pt_i.y()][pt_i.x()] << " , ";
          //draw_solid_circle(himg,pt_i,5.0*hit_size,rgb_pixel(255-pixel_i,pixel_i,255));
          std::pair<dlib::point,dlib::point> linetemp = ht.get_line(pt_i);
          dlib::draw_line(temp, linetemp.first+offset, linetemp.second+offset, dlib::rgb_pixel(255-pixel_i,pixel_i,255));
          //pixel_i+=10;
          //if(pixel_i>=200) pixel_i=0;
        }
        cout << endl;
        // Note that we must offset the output line to account for our offset subwindow.
        // We do this by just adding in the offset to the line endpoints. 
        dlib::draw_line(temp, line.first+offset, line.second+offset, dlib::rgb_pixel(255,0,0));
        //draw_line(temp, line2.first+offset, line2.second+offset, rgb_pixel(255,255,0));
        dlib::draw_solid_circle(temp,line.first+offset,4.0*hit_size,50);
        dlib::draw_solid_circle(temp,line.second+offset,4.0*hit_size,50);
        cout << "line pairs are : " << line.first << " " << line.second << endl;
        //draw_solid_circle(temp,center_p,2.0*hit_size,75);
        win.clear_overlay();
        win.set_image(temp);
        // Also show the subwindow we ran the Hough transform on as a green box.  You will
        // see that the detected line is exactly contained within this box and also
        // overlaps the original line.
        win.add_overlay(box, dlib::rgb_pixel(0,255,0));

        // We can also display the Hough transform itself using the jet color scheme.
        //draw_solid_circle(himg,p,10*hit_size,150);  // and draw a circle around that point in the ht image
        win2.set_image(himg);
        i++;
        threshold_in-=threshold_diff;
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

void calculate_line_properties(dlib::dpoint p1, dlib::dpoint p2, double& m, double& b){
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
void calculate_line_properties_pixel(dlib::dpoint p1, dlib::dpoint p2, double& m, double& b){
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

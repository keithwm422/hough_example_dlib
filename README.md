dlib example programs edited by keith that show how to use dlib image processing and pixel space to load images and do Hough Transform.

image_ex.cpp is the unaltered program of edge detection with loading an image file.

image_dct_ex.cpp is the example of just loading the image of the dct muon track example image file

hough_transform_line_properties is example code from hough_transform_ex.cpp in dlib examples, but now randomly samples hit points from a line of given slope and intercept with added gaussian noise. It then draws the hits and Hough Transform and finds the HT max point. This corresponds to the line in the original space that best fits the randomly sampled hit points. This code also reports that HT line's slope and intercept to verify the calculation. Includes example tranformations from a line space to an image pixel space.

hough_transform_dct_image is combined code from hough_transform_line_properties.cpp and image_dct_ex.cpp. It draws the original image and Hough Transform and finds the HT max point. This corresponds to the line in the original space that best fits the randomly sampled hit points. This code also reports that HT line's slope and intercept (hopefully for use with the track properties and global time).

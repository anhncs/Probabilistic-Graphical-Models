/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <iostream>
#include <dai/alldai.h>  // Include main libDAI header file
#include <CImg.h>        // This example needs CImg to be installed


using namespace dai;
using namespace std;
using namespace cimg_library;


/// Converts an image into a FactorGraph as described in section 1.1.3 of [\ref Moo08]
/** The grayscale pixel values of the image are transformed by a nonlinear transformation
    into the local fields for each binary Ising variable, and the pairwise interactions
    are uniform.
    \param img The input image (should be grayscale, with 0 corresponding to background
           and 255 corresponding to foreground)
    \param J The pairwise interaction strength (should be positive for smoothing)
    \param th_min Minimal value of local field strength (corresponding to background)
    \param th_max Maximal value of local field strength (corresponding to foreground)
    \param scale Typical difference in pixel values between fore- and background
    \param pbg Probability that a pixel belongs to the background (in percent)
    \param evidence The local evidence (represented as a grayscale image)
**/
template<class T>
FactorGraph img2fg( const CImg<T> &img, double J, double th_min, double th_max, double scale, double pbg, CImg<unsigned char> &evidence ) {
    vector<Var> vars;
    vector<Factor> factors;

#ifndef NEW_CIMG
    size_t dimx = img.width;   // Width of the image in pixels
    size_t dimy = img.height;  // Height of the image in pixels
#else
    size_t dimx = img.width();   // Width of the image in pixels
    size_t dimy = img.height();  // Height of the image in pixels
#endif
    size_t N = dimx * dimy;      // One variable for each pixel

    // Create variables
    cout << "  Image width:  " << dimx << endl;
    cout << "  Image height: " << dimy << endl;
    cout << "  Pairwise interaction strength:   " << J << endl;
    cout << "  Minimal local evidence strength: " << th_min << endl;
    cout << "  Maximal local evidence strength: " << th_max << endl;
    cout << "  Scale of pixel values:           " << scale << endl;
    cout << "  Percentage of background:        " << pbg << endl;
    cout << "  Creating " << N << " variables..." << endl;
    // Reserve memory for the variables
    vars.reserve( N );
    // Create a binary variable for each pixel
    for( size_t i = 0; i < N; i++ )
        vars.push_back( Var( i, 2 ) );

    // Build image histogram
    CImg<float> hist = img.get_channel( 0 ).get_histogram( 256, 0, 255 );
    size_t cum_hist = 0;

    // Find the critical level which corresponds with the seperation
    // between foreground and background, assuming that the percentage
    // of pixels in the image that belong to the background is pbg
    size_t level = 0;
    for( level = 0; level < 256; level++ ) {
        cum_hist += (size_t)hist(level);
        if( cum_hist > pbg * dimx * dimy / 100.0 )
            break;
    }

    // Create factors
    cout << "  Creating " << (3 * N - dimx - dimy) << " factors..." << endl;
    // Reserve memory for the factors
    factors.reserve( 3 * N - dimx - dimy );
    // th_avg is the local field strength that would correspond with pixel value level
    // th_width is the width of the local field strength range
    double th_avg = (th_min + th_max) / 2.0;
    double th_width = (th_max - th_min) / 2.0;
    // For each pixel
    for( size_t i = 0; i < dimx; i++ )
        for( size_t j = 0; j < dimy; j++ ) {
            // Add a pairwise interaction with the left neighboring pixel
            if( i >= 1 )
                factors.push_back( createFactorIsing( vars[i*dimy+j], vars[(i-1)*dimy+j], J ) );
            // Add a pairwise interaction with the upper neighboring pixel
            if( j >= 1 )
                factors.push_back( createFactorIsing( vars[i*dimy+j], vars[i*dimy+(j-1)], J ) );
            // Get the pixel value
            double x = img(i,j);
            // Apply the nonlinear transformation to get the local field strength
            double th = th_avg + th_width * tanh( (x - level) / scale );
            // Add a single-variable interaction with strength th
            factors.push_back( createFactorIsing( vars[i*dimy+j], th ) );

            // For visualization, we calculate a grayscale level corresponding to the local field strength
            unsigned char g = (unsigned char)((tanh(th) + 1.0) / 2.0 * 255.0);
            // and store it in the evidence image
            if( g > 127 ) {
                evidence(i,j,0) = 255;
                evidence(i,j,1) = 2 * (g - 127);
                evidence(i,j,2) = 2 * (g - 127);
            } else {
                evidence(i,j,0) = 0;
                evidence(i,j,1) = 0;
                evidence(i,j,2) = 2*g;
            }
        }

    // Create the factor graph out of the variables and factors
    cout << "Creating the factor graph..." << endl;
    return FactorGraph( factors.begin(), factors.end(), vars.begin(), vars.end(), factors.size(), vars.size() );
}


/// Perform an inference algorithm on a factorgraph, and visualize intermediate beliefs
/** \param fg The factorgraph (should have only binary variables)
 *  \param algOpts String specifying the inference algorithm and its options in the format "NAME[key1=val1,key2=val2,...,keyn=valn]"
 *  \param maxIter Maximum number of iterations to perform
 *  \param tol Convergence tolerance
 *  \param m Final magnetizations
 *  \param dimx Image width
 *  \param dimy Image height
 *  \param disp Handle for displaying the intermediate beliefs
 */
double doInference( FactorGraph& fg, string algOpts, size_t maxIter, double tol, vector<double> &m, size_t dimx, size_t dimy, CImgDisplay &disp ) {
    // Construct inference algorithm
    cout << "Inference algorithm: " << algOpts << endl;
    cout << "Constructing inference algorithm object..." << endl;
    InfAlg* ia = newInfAlgFromString( algOpts, fg );

    // Initialize inference algorithm
    cout << "Initializing inference algorithm..." << endl;
    ia->init();

    // Initialize vector for storing the magnetizations
    m = vector<double>( fg.nrVars(), 0.0 );

    // Construct an image that will hold the intermediate single-variable beliefs
    CImg<unsigned char> image( dimx, dimy, 1, 3 );

    // maxDiff stores the current convergence level
    double maxDiff = 1.0;
    
    // Iterate while maximum number of iterations has not been
    // reached and requested convergence level has not been reached
    cout << "Starting inference algorithm..." << endl;
    for( size_t iter = 0; iter < maxIter && maxDiff > tol; iter++ ) {
        // Set magnetizations to beliefs
        for( size_t i = 0; i < fg.nrVars(); i++ )
            m[i] = ia->beliefV(i)[1] - ia->beliefV(i)[0];

        // For each pixel, calculate a color coded magnetization
        // and store it in the image for visualization
        for( size_t i = 0; i < dimx; i++ )
            for( size_t j = 0; j < dimy; j++ ) {
                unsigned char g = (unsigned char)((m[i*dimy+j] + 1.0) / 2.0 * 255.0);
                if( g > 127 ) {
                    image(i,j,0) = 255;
                    image(i,j,1) = 2 * (g - 127);
                    image(i,j,2) = 2 * (g - 127);
                } else {
                    image(i,j,0) = 0;
                    image(i,j,1) = 0;
                    image(i,j,2) = 2*g;
                }
            }

        // Display the image with the current beliefs
#ifndef NEW_CIMG
        disp << image;
#else
        disp = image;
#endif

        // Perform the requested inference algorithm for only one step
        ia->setMaxIter( iter + 1 );
        maxDiff = ia->run();

        // Output progress
        cout << "  Iterations = " << iter << ", maxDiff = " << maxDiff << endl;
    }
    cout << "Finished inference algorithm" << endl;

    // Clean up inference algorithm
    delete ia;

    // Return reached convergence level
    return maxDiff;
}


/// Main program
/** This example shows how one can use approximate inference in factor graphs
 *  on a simple vision task: given two images, identify smooth regions where these
 *  two images differ more than some threshold. This can be used to seperate 
 *  foreground from background if one image contains the background and the other
 *  one the combination of background and foreground.
 */
int main(int argc,char **argv) {
    cout << "This program is part of libDAI - http://www.libdai.org/" << endl;
    cout << "(Use the option -h for getting help with the command line arguments.)" << endl;
    // Display program usage, when invoked from the command line with option '-h'
    cimg_usage( "This example shows how libDAI can be used for a simple image segmentation task" );
    // Get command line arguments
    const char* file_i1 = cimg_option( "-i1", "example_img_in1.jpg", "Input image 1" );
    const char* file_i2 = cimg_option( "-i2", "example_img_in2.jpg", "Input image 2" );
    const char* file_o1 = cimg_option( "-o1", "example_img_out1.jpg", "Output image (local evidence)" );
    const char* file_o2 = cimg_option( "-o2", "example_img_out2.jpg", "Output image (magnetizations)" );
    const double J = cimg_option( "-J", 2.4, "Pairwise interaction strength (i.e., smoothing strength)" );
    const double th_min = cimg_option( "-thmin", -3.0, "Local evidence strength of background" );
    const double th_max = cimg_option( "-thmax", 3.2, "Local evidence strength of foreground" );
    const double scale = cimg_option( "-scale", 40.0, "Typical difference in pixel values between fore- and background" );
    const double pbg = cimg_option( "-pbg", 90.0, "Percentage of background in image" );
    const char *infname = cimg_option( "-method", "BP[updates=SEQMAX,maxiter=1,tol=1e-9,logdomain=0]", "Inference method in format name[key1=val1,...,keyn=valn]" );
    const size_t maxiter = cimg_option( "-maxiter", 100, "Maximum number of iterations for inference method" );
    const double tol = cimg_option( "-tol", 1e-9, "Desired tolerance level for inference method" );
    const char *file_fg = cimg_option( "-fg", "", "Output factor graph" );

    // Read input images
    cout << endl;
    cout << "Reading input image 1 (" << file_i1 << ")..." << endl;
    CImg<unsigned char> image1 = CImg<>( file_i1 );
    cout << "Reading input image 2 (" << file_i2 << ")..." << endl;
    CImg<unsigned char> image2 = CImg<>( file_i2 );

    // Check image sizes
#ifndef NEW_CIMG
    if( (image1.width != image2.width) || (image1.height != image2.height) )
        cerr << "Error: input images should have same size." << endl;
    size_t dimx = image1.width;
    size_t dimy = image1.height;
#else
    if( (image1.width() != image2.width()) || (image1.height() != image2.height()) )
        cerr << "Error: input images should have same size." << endl;
    size_t dimx = image1.width();
    size_t dimy = image1.height();
#endif

    // Display input images
    cout << "Displaying input image 1..." << endl;
    CImgDisplay disp1( image1, "Input image 1", 0 );
    cout << "Displaying input image 2..." << endl;
    CImgDisplay disp2( image2, "Input image 2", 0 );

    // Construct absolute difference image
    cout << "Constructing difference image..." << endl;
    CImg<int> image3( image1 );
    image3 -= image2;
    image3.abs();
    // Normalize difference image
    image3.norm( 1 ); // 1 = L1, 2 = L2, -1 = Linf

    // Normalize the difference by the average value of the background image
    for( size_t i = 0; i < dimx; i++ ) {
        for( size_t j = 0; j < dimy; j++ ) {
            int avg = 0;
#ifndef NEW_CIMG
            for( int c = 0; c < image1.dimv(); c++ )
                avg += image1( i, j, c );
            avg /= image1.dimv();
#else
            for( int c = 0; c < image1.spectrum(); c++ )
                avg += image1( i, j, c );
            avg /= image1.spectrum();
#endif
            image3( i, j, 0 ) /= (1.0 + avg / 255.0);
        }
    }
    image3.normalize( 0, 255 );

    // Display difference image
    cout << "Displaying difference image..." << endl;
    CImgDisplay disp3( image3, "Relative difference of both inputs", 0 );

    // Convert difference image into a factor graph and store
    // the local evidence in image4 for visualization
    CImg<unsigned char> image4( dimx, dimy, 1, 3 );
    cout << "Converting difference image into factor graph..." << endl;
    FactorGraph fg = img2fg( image3, J, th_min, th_max, scale, pbg, image4 );

    // Display local evidence
    cout << "Displaying local evidence..." << endl;
    CImgDisplay disp4( image4, "Local evidence", 0 );
    cout << "Saving local evidence as JPEG in " << file_o1 << endl;
    image4.save_jpeg( file_o1, 100 );
    if( strlen( file_fg ) > 0 ) {
        cout << "Saving factor graph as " << file_fg << endl;
        fg.WriteToFile( file_fg );
    }

    // Solve the inference problem and visualize intermediate steps
    CImgDisplay disp5( dimx, dimy, "Beliefs during inference", 0 );
    vector<double> m; // Stores the final magnetizations
    cout << "Solving the inference problem...please be patient!" << endl;
    doInference( fg, infname, maxiter, tol, m, dimx, dimy, disp5 );

    // Visualize the final magnetizations
    for( size_t i = 0; i < dimx; i++ )
        for( size_t j = 0; j < dimy; j++ ) {
            unsigned char g = (unsigned char)((m[i*dimy+j] + 1.0) / 2.0 * 255.0);
            if( g > 127 ) {
                image4(i,j,0) = image2(i,j,0);
                image4(i,j,1) = image2(i,j,1);
                image4(i,j,2) = image2(i,j,2);
            } else
#ifndef NEW_CIMG
                for( size_t c = 0; c < (size_t)image4.dimv(); c++ )
                    image4(i,j,c) = 255;
#else
                for( size_t c = 0; c < (size_t)image4.spectrum(); c++ )
                    image4(i,j,c) = 255;
#endif
        }
    cout << "Displaying the final result of the segmentation problem..." << endl;
    CImgDisplay main_disp( image4, "Foreground/background segmentation result", 0 );

    cout << "Saving the final result of the segmentation problem as JPEG in " << file_o2 << endl;
    image4.save_jpeg( file_o2, 100 );

    cout << "Close the last image display in order to finish." << endl;
#ifndef NEW_CIMG
    while( !main_disp.is_closed )
        cimg::wait( 40 );
#else
    while( !main_disp.is_closed() )
        cimg::wait( 40 );
#endif

    return 0;
}

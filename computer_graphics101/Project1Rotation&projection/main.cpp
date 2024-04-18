#include "Triangle.hpp"

#include "rasterizer.hpp"

#include <eigen3/Eigen/Eigen>

#include <iostream>

#include <opencv2/opencv.hpp>

using namespace std;

constexpr double MY_PI = 3.1415926;



Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)

{

    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();



    Eigen::Matrix4f translate;

    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,

        -eye_pos[2], 0, 0, 0, 1;



    view = translate * view;



    return view;

}



Eigen::Matrix4f get_model_matrix(float rotation_angle)

{

    Eigen::Matrix4f model = Eigen::Matrix4f::Identity(); 



    // TODO: Implement this function

    // Create the model matrix for rotating the triangle around the Z axis.

    // Then return it.

    float rotation_radian_angle = rotation_angle*MY_PI/180;

    float cosa = cos(rotation_radian_angle);

    float sina = sin(rotation_radian_angle);

    // if rotate around different axis just alter the model matrix

    model(0,0) = cosa;

    model(0,1) = -sina;

    model(1,0) = sina;

    model(1,1) = cosa;

        //     cosa, -sina, Project0, Project0,

        // sina, cosa,  Project0, Project0,

        // Project0,    Project0,     1, Project0,

        // Project0,    Project0,     Project0, 1;

    return model;

}



Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,

                                      float zNear, float zFar)

{

    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();



    // TODO: Implement this function

    // Create the projection matrix for the given parameters.

    // Then return it.

    

    //squeeze fist and than projection

    //compute l r b t

    // cout<<"zfar"<<zFar;    //z all ways positive  and bigger means father

    // cout<<"zNear"<<zNear;

    float t = tan(eye_fov*MY_PI/2/180)*fabs(zNear);  //better use fabs for zNear

    float r = aspect_ratio*t;

    float n = zNear;

    float f = zFar;

    // cout<<"t"<<t;

    Matrix4f Mp;

    Mp<<

    -n, 0, 0, 0,

    0, -n, 0, 0,

    0, 0, -n-f, n*f,

    0, 0, 1, 0;



    Matrix4f Mo_translate;

  //becaus l=-r so we don't need to compute r+l/2  t+b/2

    // Mo_translate(2,3) = -(n+f)/2;  //this way need to use Matrix4f::Identity() before

    Mo_translate<<

    1, 0, 0, 0,

    0, 1, 0, 0,  //b=-t;

    0, 0, 1, (n+f)/2 ,

    0, 0, 0, 1;

    Matrix4f Mo_scale;

    Mo_scale<<

    1/r, 0, 0, 0,

    0, 1/t, 0, 0,

    0, 0, -2/(n-f), 0,

    0, 0, 0, 1;



    projection = (Mo_scale*Mo_translate)*Mp;

    return projection;

}



int main(int argc, const char** argv)

{

    float angle = 0;

    bool command_line = false;

    std::string filename = "output.png";



    if (argc >= 3) {     //

        command_line = true;

        angle = std::stof(argv[2]); // -r by default

        if (argc == 4) {

            filename = std::string(argv[3]);

        }

        else

            return 0;

    }



    rst::rasterizer r(700, 700);



    Eigen::Vector3f eye_pos = {0, 0, 5};



    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};



    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};



    auto pos_id = r.load_positions(pos);

    auto ind_id = r.load_indices(ind);



    int key = 0;

    int frame_count = 0;



    if (command_line) {

        r.clear(rst::Buffers::Color | rst::Buffers::Depth);



        r.set_model(get_model_matrix(angle));

        r.set_view(get_view_matrix(eye_pos));

        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));



        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());

        image.convertTo(image, CV_8UC3, 1.0f);



        cv::imwrite(filename, image);



        return 0;

    }



    while (key != 27) {

        r.clear(rst::Buffers::Color | rst::Buffers::Depth);



        r.set_model(get_model_matrix(angle));

        r.set_view(get_view_matrix(eye_pos));

        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));



        r.draw(pos_id, ind_id, rst::Primitive::Triangle);



        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());

        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imshow("image", image);

        key = cv::waitKey(10);



        std::cout << "frame count: " << frame_count++ << '\n';



        if (key == 'a') {

            angle += 10;

        }

        else if (key == 'd') {

            angle -= 10;

        }

    }



    return 0;

}


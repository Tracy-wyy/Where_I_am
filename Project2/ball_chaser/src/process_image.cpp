#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // Request a service and pass the velocities to it to drive the robot
    ROS_INFO_STREAM("Passing the velocities to the robot");

    // Request centered joint angles [1.57, 1.57]
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    // Call the safe_move service and pass the requested joint angles
    if (!client.call(srv))
        ROS_ERROR("Failed to call service ball_chaser");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    int left = img.step/3;
    int right = 2*img.step/3;
    bool iswhite = false;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    for (int i = 0; i < img.height * img.step; i+=3)
    {
        
        if((img.data[i]==white_pixel) && (img.data[i+1]==white_pixel) && (img.data[i+2]==white_pixel))
        {
            iswhite = true;
            // Then, identify if this pixel falls in the left, mid, or right side of the image
            int position = i % img.step;
            // Depending on the white ball position, call the drive_bot function and pass velocities to it
            if(position <= left)
            {
                drive_robot(0,0.3);
            }
            else if(position >= right)
            {
                drive_robot(0,-0.3);
            }
            else
            {
                // Go straight
                drive_robot(0.5, 0);
            }
            break;
        }
    }    
    // Request a stop when there's no white ball seen by the camera
    if(iswhite == false)
    {
        drive_robot(0,0);
    }
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}

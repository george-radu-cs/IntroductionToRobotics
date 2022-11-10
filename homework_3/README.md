# Homework 3 - 7-segment display & joystick

Control the position of the current segment on the 7-segment display using the joystick. Update the state(led active or inactive) of each segment by moving the joystick on the x direction.

Program that controls the states (led active or inactive) for each segment in a 7-segment diplay.

The program has 2 states: one for selecting the segment to update and one to choose the segment state.

The initial state is the one for selecting the segment for updating, starting from the dp led. To let the user know which segment is the current segment, the current segment is set to blink. Using a joystick the user can move to the other segments, the program allow the user one move at a time, after each one the joystick needs to come back in its init state (both axis on middle)
The program supports 8 directions (up, down, left, right by moving the joystick in a single direction and up-left, up-right, down-left, down-right by moving the joystick in both directions - on diagonal). The movements are bounded by the neighbors for each segment, meaning that trying to move from a segment to a non-existent neighbor will result in remaning at the same segment

While in the state of selecting a segment, by short pressing the joystick switch the user changes the system state to choosingSegmentState in which he can set the state of the current segment; The initial value of the segment will be the previous state saved, by moving the joystick on the x coordinate, to each end, the user can toggle between the state active or inactive of the segment. For saving the desired state the user can press the joystick switch (no additional requirements for pressing). After the press the system will save the current segment state and change back to the inital state of selecting a segment, and the current segment will remain the same

If a user wants to reset the configuration made to the display, he can make a long press on the joystick switch only while in the first state of selecting segment, which will trigger the reset of the system (turn all segments off and move the current segment back to the dp led).

## Pictures of the setup

![setup_image_1.jpg](./images/setup_image_1.jpg)
![setup_image_2.jpg](./images/setup_image_2.jpg)
![setup_image_3.jpg](./images/setup_image_3.jpg)

## Video showcasing the setup

[![hw3_video_showcase](./images/setup_image_1.jpg)](https://youtu.be/AcEbwVKgyy8)

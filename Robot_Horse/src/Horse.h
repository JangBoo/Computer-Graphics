//----------------------------------------------------------------------------

#define BASE_TORSO_HEIGHT 1.0
#define BASE_TORSO_WIDTH 3.0
#define BASE_TORSO_DEPTH 1.0

#define BASE_NECK_HEIGHT 1.5
#define BASE_NECK_WIDTH 0.6
#define BASE_NECK_DEPTH 0.6

#define BASE_HEAD_HEIGHT 1.0
#define BASE_HEAD_WIDTH 0.5
#define BASE_HEAD_DEPTH 0.5

#define BASE_UPPER_ARM_HEIGHT 1.0
#define BASE_LOWER_ARM_HEIGHT 1.0
#define BASE_UPPER_ARM_WIDTH  0.3
#define BASE_LOWER_ARM_WIDTH  0.3

#define BASE_UPPER_LEG_HEIGHT 1.0
#define BASE_LOWER_LEG_HEIGHT 1.0
#define BASE_UPPER_LEG_WIDTH  0.3
#define BASE_LOWER_LEG_WIDTH  0.3


double TORSO_HEIGHT = 1.0;
double TORSO_WIDTH = 3.0;
double TORSO_DEPTH = 1.0;

double NECK_HEIGHT = 1.5;
double NECK_WIDTH = 0.6;
double NECK_DEPTH = 0.6;

double HEAD_HEIGHT = 1.0;
double HEAD_WIDTH = 0.5;
double HEAD_DEPTH = 0.5;

double UPPER_ARM_HEIGHT = 1.0;
double LOWER_ARM_HEIGHT = 1.0;
double UPPER_ARM_WIDTH  = 0.3;
double LOWER_ARM_WIDTH  = 0.3;

double UPPER_LEG_HEIGHT = 1.0;
double LOWER_LEG_HEIGHT = 1.0;
double UPPER_LEG_WIDTH  = 0.3;
double LOWER_LEG_WIDTH  = 0.3;

double base_scale = 1.0f;
void updateBaseDimensions()
{
    TORSO_HEIGHT = BASE_TORSO_HEIGHT * base_scale;
    TORSO_WIDTH = BASE_TORSO_WIDTH * base_scale;
    TORSO_DEPTH = BASE_TORSO_DEPTH * base_scale;

    NECK_HEIGHT = BASE_NECK_HEIGHT * base_scale;
    NECK_WIDTH = BASE_NECK_WIDTH * base_scale;
    NECK_DEPTH = BASE_NECK_DEPTH * base_scale;

    UPPER_ARM_HEIGHT = BASE_UPPER_ARM_HEIGHT * base_scale;
    LOWER_ARM_HEIGHT = BASE_LOWER_ARM_HEIGHT * base_scale;
    UPPER_ARM_WIDTH = BASE_UPPER_ARM_WIDTH * base_scale;
    LOWER_ARM_WIDTH = BASE_LOWER_ARM_WIDTH * base_scale;

    UPPER_LEG_HEIGHT = BASE_UPPER_LEG_HEIGHT * base_scale;
    LOWER_LEG_HEIGHT = BASE_LOWER_LEG_HEIGHT * base_scale;
    UPPER_LEG_WIDTH = BASE_UPPER_LEG_WIDTH * base_scale;
    LOWER_LEG_WIDTH = BASE_LOWER_LEG_WIDTH * base_scale;

    HEAD_HEIGHT = BASE_HEAD_HEIGHT * base_scale;
    HEAD_WIDTH = BASE_HEAD_WIDTH * base_scale;
    HEAD_DEPTH = BASE_HEAD_DEPTH * base_scale;
}

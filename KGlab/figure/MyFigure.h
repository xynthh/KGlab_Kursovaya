#ifndef MY_FIGURE_
#define MY_FIGURE_
#define _USE_MATH_DEFINES

struct Vec2
{
    double x, y;
};

struct Vec3
{
    double x, y, z;
};

void drawPrism();
void drawSemicircle(Vec2 p1, Vec2 p2);

void drawFigure();

#endif // MY_FIGURE_
// clang-format off
#include "MyFigure.h"

#include <Windows.h>
#include <GL\GL.h>
#include <iostream>
#include <iomanip>
#include <random>
// clang-format on

static std::vector<Vec2> originalStar = {
    {.x = -3, .y = 1}, // 0
    {.x = -7, .y = 6}, // 1
    {.x = 0, .y = 2}, // 2
    {.x = 4, .y = 7}, // 3
    {.x = 1, .y = 0}, // 4
    {.x = 6, .y = -6}, // 5
    {.x = -1, .y = -9}, // 6
    {.x = -6, .y = -4}, // 7
    {.x = -3, .y = 1} // 8
};

constexpr double tex_min_x = -8.0; // Было -7.0
constexpr double tex_max_x = 6.0; // Было 6.0
constexpr double tex_min_y = -11.0; // Было -9.0
constexpr double tex_max_y = 7.0; // Было 7.0
constexpr double tex_range_x = tex_max_x - tex_min_x;
constexpr double tex_range_y = tex_max_y - tex_min_y;

void drawSemicircle(Vec2 p1, Vec2 p2)
{
    int seed = 0;
    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> r(0, 1);
    double alphaValue = 0.5;
    int segments = 90;
    double radius = sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2)) / 2.0;
    Vec2 center = {.x = (p1.x + p2.x) / 2.0, .y = (p1.y + p2.y) / 2.0};

    double angle1 = atan2(p1.y - center.y, p1.x - center.x);
    double angle2 = atan2(p2.y - center.y, p2.x - center.x);
    double startAngle = angle1;
    double endAngle = angle2;
    if (endAngle < startAngle)
    {
        endAngle += 2 * M_PI;
    }
    glBegin(GL_TRIANGLES);
    glNormal3d(0, 0, -1);

    for (int i = 0; i < segments; ++i)
    {
        double theta1 = startAngle + (endAngle - startAngle) * i / segments;
        double theta2 = startAngle + (endAngle - startAngle) * (i + 1) / segments;
        double x1 = center.x + radius * cos(theta1);
        double y1 = center.y + radius * sin(theta1);
        double x2 = center.x + radius * cos(theta2);
        double y2 = center.y + radius * sin(theta2);

        glColor3d(r(gen), r(gen), r(gen));
        glVertex3d(center.x, center.y, 0);
        glVertex3d(x1, y1, 0);
        glVertex3d(x2, y2, 0);
    }
    glEnd();
    glBegin(GL_TRIANGLES);
    glNormal3d(0, 0, 1);

    for (int i = 0; i < segments; ++i)
    {
        double theta1 = startAngle + (endAngle - startAngle) * i / segments;
        double theta2 = startAngle + (endAngle - startAngle) * (i + 1) / segments;
        double x1 = center.x + radius * cos(theta1);
        double y1 = center.y + radius * sin(theta1);
        double x2 = center.x + radius * cos(theta2);
        double y2 = center.y + radius * sin(theta2);
        glColor4d(1, 1, 1, alphaValue);
        glTexCoord2d((center.x - tex_min_x) / tex_range_x, (center.y - tex_min_y) / tex_range_y);
        glVertex3d(center.x, center.y, 2);
        glTexCoord2d((x1 - tex_min_x) / tex_range_x, (y1 - tex_min_y) / tex_range_y);
        glVertex3d(x1, y1, 2);
        glTexCoord2d((x2 - tex_min_x) / tex_range_x, (y2 - tex_min_y) / tex_range_y);
        glVertex3d(x2, y2, 2);
    }
    glEnd();
    glBegin(GL_QUADS);
    for (int i = 0; i < segments; ++i)
    {
        double theta1 = startAngle + (endAngle - startAngle) * i / segments;
        double theta2 = startAngle + (endAngle - startAngle) * (i + 1) / segments;
        double x1 = center.x + radius * cos(theta1);
        double y1 = center.y + radius * sin(theta1);
        double x2 = center.x + radius * cos(theta2);
        double y2 = center.y + radius * sin(theta2);
        double nx = cos(theta1);
        double ny = sin(theta1);
        double nz = 0;
        double len = sqrt(nx * nx + ny * ny + nz * nz);
        nx /= len;
        ny /= len;
        nz /= len;
        glNormal3d(nx, ny, nz);
        glColor3d(r(gen), r(gen), r(gen));
        glVertex3d(x1, y1, 0);
        glVertex3d(x2, y2, 0);
        glVertex3d(x2, y2, 2);
        glVertex3d(x1, y1, 2);
    }
    glEnd();
}

void printFigure()
{
    int seed = 0;
    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> r(0, 1);
    double alphaValue = 0.5;
    glBegin(GL_QUADS);
    for (int i = 0; i < static_cast<int>(originalStar.size()) - 3; i++)
    {
        double nx = originalStar[i + 1].y - originalStar[i].y;
        double ny = originalStar[i].x - originalStar[i + 1].x;
        double nz = 0;
        Vec2 midPoint = {.x = (originalStar[i].x + originalStar[i + 1].x) / 2.0,
                         .y = (originalStar[i].y + originalStar[i + 1].y) / 2.0};
        Vec2 center = {.x = 0.0, .y = 0.0};
        double dx = midPoint.x - center.x;
        double dy = midPoint.y - center.y;
        if (nx * dx + ny * dy < 0)
        {
            nx = -nx;
            ny = -ny;
        }
        double len = sqrt(nx * nx + ny * ny + nz * nz);
        nx /= len;
        ny /= len;
        nz /= len;
        glColor3d(r(gen), r(gen), r(gen));
        glNormal3d(nx, ny, nz);
        glVertex3d(originalStar[i].x, originalStar[i].y, 0);
        glVertex3d(originalStar[i].x, originalStar[i].y, 2);
        glVertex3d(originalStar[i + 1].x, originalStar[i + 1].y, 2);
        glVertex3d(originalStar[i + 1].x, originalStar[i + 1].y, 0);
    }
    double nx = originalStar[0].y - originalStar[7].y;
    double ny = originalStar[7].x - originalStar[0].x;
    double nz = 0;
    Vec2 midPoint = {.x = (originalStar[7].x + originalStar[0].x) / 2.0,
                     .y = (originalStar[7].y + originalStar[0].y) / 2.0};
    Vec2 center = {.x = 0.0, .y = 0.0};
    double dx = midPoint.x - center.x;
    double dy = midPoint.y - center.y;
    if (nx * dx + ny * dy < 0)
    {
        nx = -nx;
        ny = -ny;
    }
    double len = sqrt(nx * nx + ny * ny + nz * nz);
    nx /= len;
    ny /= len;
    nz /= len;
    glNormal3d(nx, ny, nz);
    glVertex3d(originalStar[7].x, originalStar[7].y, 0);
    glVertex3d(originalStar[7].x, originalStar[7].y, 2);
    glVertex3d(originalStar[0].x, originalStar[0].y, 2);
    glVertex3d(originalStar[0].x, originalStar[0].y, 0);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3d(0, 0, -1);
    glColor3d(r(gen), r(gen), r(gen));
    glVertex3d(originalStar[0].x, originalStar[0].y, 0);
    glVertex3d(originalStar[1].x, originalStar[1].y, 0);
    glVertex3d(originalStar[2].x, originalStar[2].y, 0);
    glVertex3d(originalStar[4].x, originalStar[4].y, 0);
    glEnd();

    glBegin(GL_TRIANGLES);
    glNormal3d(0, 0, -1);
    glColor3d(r(gen), r(gen), r(gen));
    glVertex3d(originalStar[2].x, originalStar[2].y, 0);
    glVertex3d(originalStar[3].x, originalStar[3].y, 0);
    glVertex3d(originalStar[4].x, originalStar[4].y, 0);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3d(0, 0, -1);
    glColor3d(r(gen), r(gen), r(gen));
    glVertex3d(originalStar[4].x, originalStar[4].y, 0);
    glVertex3d(originalStar[5].x, originalStar[5].y, 0);
    glVertex3d(originalStar[6].x, originalStar[6].y, 0);
    glVertex3d(originalStar[7].x, originalStar[7].y, 0);
    glEnd();

    glBegin(GL_TRIANGLES);
    glNormal3d(0, 0, -1);
    glColor3d(r(gen), r(gen), r(gen));
    glVertex3d(originalStar[0].x, originalStar[0].y, 0);
    glVertex3d(originalStar[4].x, originalStar[4].y, 0);
    glVertex3d(originalStar[7].x, originalStar[7].y, 0);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3d(0, 0, 1);
    glColor4d(1, 1, 1, alphaValue); // Белый цвет с альфа-прозрачностью

    glTexCoord2d((originalStar[0].x - tex_min_x) / tex_range_x, (originalStar[0].y - tex_min_y) / tex_range_y);
    glVertex3d(originalStar[0].x, originalStar[0].y, 2);

    glTexCoord2d((originalStar[1].x - tex_min_x) / tex_range_x, (originalStar[1].y - tex_min_y) / tex_range_y);
    glVertex3d(originalStar[1].x, originalStar[1].y, 2);

    glTexCoord2d((originalStar[2].x - tex_min_x) / tex_range_x, (originalStar[2].y - tex_min_y) / tex_range_y);
    glVertex3d(originalStar[2].x, originalStar[2].y, 2);

    glTexCoord2d((originalStar[4].x - tex_min_x) / tex_range_x, (originalStar[4].y - tex_min_y) / tex_range_y);
    glVertex3d(originalStar[4].x, originalStar[4].y, 2);
    glEnd();

    glBegin(GL_TRIANGLES);
    glNormal3d(0, 0, 1);
    glColor4d(1, 1, 1, alphaValue);

    glTexCoord2d((originalStar[2].x - tex_min_x) / tex_range_x, (originalStar[2].y - tex_min_y) / tex_range_y);
    glVertex3d(originalStar[2].x, originalStar[2].y, 2);

    glTexCoord2d((originalStar[3].x - tex_min_x) / tex_range_x, (originalStar[3].y - tex_min_y) / tex_range_y);
    glVertex3d(originalStar[3].x, originalStar[3].y, 2);

    glTexCoord2d((originalStar[4].x - tex_min_x) / tex_range_x, (originalStar[4].y - tex_min_y) / tex_range_y);
    glVertex3d(originalStar[4].x, originalStar[4].y, 2);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3d(0, 0, 1);
    glColor4d(1, 1, 1, alphaValue);

    glTexCoord2d((originalStar[4].x - tex_min_x) / tex_range_x, (originalStar[4].y - tex_min_y) / tex_range_y);
    glVertex3d(originalStar[4].x, originalStar[4].y, 2);

    glTexCoord2d((originalStar[5].x - tex_min_x) / tex_range_x, (originalStar[5].y - tex_min_y) / tex_range_y);
    glVertex3d(originalStar[5].x, originalStar[5].y, 2);

    glTexCoord2d((originalStar[6].x - tex_min_x) / tex_range_x, (originalStar[6].y - tex_min_y) / tex_range_y);
    glVertex3d(originalStar[6].x, originalStar[6].y, 2);

    glTexCoord2d((originalStar[7].x - tex_min_x) / tex_range_x, (originalStar[7].y - tex_min_y) / tex_range_y);
    glVertex3d(originalStar[7].x, originalStar[7].y, 2);
    glEnd();

    glBegin(GL_TRIANGLES);
    glNormal3d(0, 0, 1);
    glColor4d(1, 1, 1, alphaValue);

    glTexCoord2d((originalStar[0].x - tex_min_x) / tex_range_x, (originalStar[0].y - tex_min_y) / tex_range_y);
    glVertex3d(originalStar[0].x, originalStar[0].y, 2);

    glTexCoord2d((originalStar[4].x - tex_min_x) / tex_range_x, (originalStar[4].y - tex_min_y) / tex_range_y);
    glVertex3d(originalStar[4].x, originalStar[4].y, 2);

    glTexCoord2d((originalStar[7].x - tex_min_x) / tex_range_x, (originalStar[7].y - tex_min_y) / tex_range_y);
    glVertex3d(originalStar[7].x, originalStar[7].y, 2);
    glEnd();
}

void drawFigure()
{
    printFigure();
    drawSemicircle(originalStar[7], originalStar[6]);
}

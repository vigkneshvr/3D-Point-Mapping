#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <math.h>
#include "fssimplewindow.h"

using namespace std;

double GetColour(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{
    float A[] = { x1,y1,z1 }, B[] = { x2,y2,z2 }, C[] = { x3,y3,z3 };
    float AB[3], CB[3], n1[3], n2[] = { 0,0,1 };
    double mn1, angle, c;
    for (int i = 0;i < 3;i++)
    {
        AB[i] = B[i] - A[i];
        CB[i] = B[i] - C[i];
    }
    n1[0] = AB[1] * CB[2] - AB[2] * CB[1]; //cross producto to find normal vector to the plane
    n1[1] = AB[2] * CB[0] - AB[0] * CB[2];
    n1[2] = AB[0] * CB[1] - AB[1] * CB[0];

    mn1 = sqrt(double(n1[0] * n1[0] + n1[1] * n1[1] + n1[2] * n1[2])); //Finding magnitude
    angle = acos(n1[2] / mn1) * 180 / 3.14; // cos(theta)=n1.n2/|n1|.|n2|
    if (angle > 90)
        angle -= 90;
    c = 255 - (16 * angle / 9);
    return c;
}

void DrawVertex(double cx, double cy, double r)
{
    const double PI = 3.1415927;
    glColor3f(0, 0, 1);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < 64; ++i)
    {
        double a = (double)i * PI * 2.0 / 64.0;
        double s = sin(a);
        double c = cos(a);
        glVertex2d(cx + c * r, cy + s * r);
    }
    glEnd();
}


void DrawFace(float x[], float y[], float z[], int f[][3], int fnum)
{
    double colour;
    for (int i = 0;i < fnum;i++)
    {
        int v1 = f[i][0], v2 = f[i][1], v3 = f[i][2];

        DrawVertex(x[v1 - 1] * 100, y[v1 - 1] * 100, 3);

        DrawVertex(x[v2 - 1] * 100, y[v2 - 1] * 100, 3);

        DrawVertex(x[v3 - 1] * 100, y[v3 - 1] * 100, 3);

        colour = GetColour(x[v1 - 1], y[v1 - 1], z[v1 - 1], x[v2 - 1], y[v2 - 1], z[v2 - 1], x[v3 - 1], y[v3 - 1], z[v3 - 1]);

        glBegin(GL_TRIANGLES);
        glColor3ub(0, 0, colour);
        glVertex2f(x[v1 - 1] * 100, y[v1 - 1] * 100);
        glVertex2f(x[v2 - 1] * 100, y[v2 - 1] * 100);
        glVertex2f(x[v3 - 1] * 100, y[v3 - 1] * 100);
        glEnd();
    }
}

void RotateCalc(float x[], float y[], float z[], int f[][3], int fnum, int vnum, int thetaX, int thetaY)
{
    const double PI = 3.1415927;
    thetaX = thetaX * PI / 180;
    thetaY = thetaY * PI / 180;
    for (int i = 0;i < vnum;i++)
    {
        float a = x[i], b = y[i], c = z[i];
        x[i] = a * cos(thetaY) - c * sin(thetaY);   //Equations derived from rotational matrix
        y[i] = a * sin(thetaX) * sin(thetaY) + b * cos(thetaX) + c * sin(thetaX) * cos(thetaY);
        z[i] = a * cos(thetaX) * sin(thetaY) - b * sin(thetaX) + c * cos(thetaX) * cos(thetaY);
        /*if (z[i] != 0)
        {
            x[i] = x[i] / z[i];
            y[i] = y[i] / z[i];
        }*/
    }
    DrawFace(x, y, z, f, fnum);
}

int main()
{
    int vertex_num, face_num, line_num = 0, i = 0, v_num = 0;
    float x[10], y[10], z[10];
    int f[10][3];
    int mouseEvent, leftButton, middleButton, rightButton;
    int screenX, screenY, mx = -1, my = -1;
    int thetaX = 0, thetaY = 0;

    string line, temp;
    ifstream myfile;
    myfile.open("object.txt");
    if (!myfile.is_open())  //to check if file opened
    {
        cout << "Failed to open the file\n";
        return 0;
    }
    while (getline(myfile, line))    //storing the inputs from the file in variables
    {
        stringstream ss(line);
        if (0 == line_num)
        {
            getline(ss, temp, ',');
            vertex_num = stoi(temp);
            getline(ss, temp, ',');
            face_num = stoi(temp);
            line_num++;
        }
        else if (line_num > 0 && line_num < vertex_num + 1)
        {
            getline(ss, temp, ',');
            v_num = stoi(temp);
            getline(ss, temp, ',');
            x[v_num - 1] = stof(temp);
            getline(ss, temp, ',');
            y[v_num - 1] = stof(temp);
            getline(ss, temp, ',');
            z[v_num - 1] = stof(temp);
            line_num++;
        }
        else if (line_num > vertex_num && line_num < vertex_num + face_num)
        {
            getline(ss, temp, ',');
            f[i][0] = stoi(temp);
            getline(ss, temp, ',');
            f[i][1] = stoi(temp);
            getline(ss, temp, ',');
            f[i][2] = stoi(temp);
            i++;
        }
    }
    FsOpenWindow(16, 16, 800, 600, 1);
    glTranslated(400, 300, 0);  //Shifts the origin
    glRotated(180, 1, 0, 0);    //Rotates the y-axis
    for (;;)
    {
        FsPollDevice();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto key = FsInkey();
        if (FSKEY_ESC == key)
        {
            break;
        }
        mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, screenX, screenY);
        if (leftButton != 0 && mx == -1 && my == -1)
        {
            mx = screenX;
            my = screenY;
        }
        if (leftButton != 0 && mx != -1 && my != -1)
        {
            thetaY = (screenX - mx) * 2;
            thetaX = (screenY - my) * 2;
            mx = screenX;
            my = screenY;
        }
        if (leftButton == 0)
        {
            mx = -1;
            my = -1;
            thetaX = 0;
            thetaY = 0;
        }
        RotateCalc(x, y, z, f, face_num, v_num, thetaX, thetaY);
        FsSwapBuffers();
        FsSleep(25);
    }
    myfile.close();
    return 0;
}
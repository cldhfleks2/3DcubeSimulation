#include <iostream>
#include <vector>
#include <cmath>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

struct Point3D {
    float x, y, z;
};

// ť�긦 ��Ÿ���� ���� ��ǥ��
vector<Point3D> vertices = {
    { -1.0f, -1.0f, -1.0f },
    { 1.0f, -1.0f, -1.0f },
    { 1.0f, 1.0f, -1.0f },
    { -1.0f, 1.0f, -1.0f },
    { -1.0f, -1.0f, 1.0f },
    { 1.0f, -1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f },
    { -1.0f, 1.0f, 1.0f }
};

// ť�긦 ��Ÿ���� ��
vector<vector<int>> faces = {
    { 0, 1, 2, 3 }, // �ո�
    { 1, 5, 6, 2 }, // �����ʸ�
    { 4, 0, 3, 7 }, // ���ʸ�
    { 5, 4, 7, 6 }, // �޸�
    { 3, 2, 6, 7 }, // ���ʸ�
    { 0, 4, 5, 1 }  // �Ʒ��ʸ�
};

// ȸ�� ���
void rotate(vector<Point3D>& vertices, float angle, char axis) {
    float s = sin(angle);
    float c = cos(angle);
    for (auto& vertex : vertices) {
        float x = vertex.x;
        float y = vertex.y;
        float z = vertex.z;
        switch (axis) {
        case 'x':
            vertex.y = y * c - z * s;
            vertex.z = y * s + z * c;
            break;
        case 'y':
            vertex.x = x * c + z * s;
            vertex.z = -x * s + z * c;
            break;
        case 'z':
            vertex.x = x * c - y * s;
            vertex.y = x * s + y * c;
            break;
        }
    }
}

// ���� ��ȯ �Լ�
float toRadians(float degrees) {
    return degrees * M_PI / 180.0f;
}

// ���� �Լ�
int main() {
    // ť�� �ʱ�ȭ
    const int screenWidth = 800;
    const int screenHeight = 600;
    const float aspectRatio = (float)screenWidth / (float)screenHeight;
    const float near = 0.1f;
    const float far = 100.0f;
    const float fov = toRadians(60.0f);

    // �ܼ�â���� ����ϱ� ����, �뷫���� ȭ���� ����ϴ�.
    vector<char> screen(screenWidth * screenHeight, ' ');

    // ȸ���� �ʱ�ȭ
    float rotationX = 0.0;
    float rotationY = 0.0;
    float rotationZ = 0.0;

    // ȸ���� �ӵ�
    const float rotationSpeed = 0.02;

    // ���� ����
    while (true) {
        // ȸ�� ��� ���
        vector<Point3D> transformedVertices = vertices;
        rotate(transformedVertices, rotationX, 'x');
        rotate(transformedVertices, rotationY, 'y');
        rotate(transformedVertices, rotationZ, 'z');

        // ȭ�� �ʱ�ȭ
        for (int i = 0; i < screen.size(); i++) {
            screen[i] = ' ';
        }

        // ȭ�鿡 ť�� �׸���
        for (auto& face : faces) {
            // �������� ������ ������ �׸��� �ʽ��ϴ�.
            Point3D normal = {
                (transformedVertices[face[1]].y - transformedVertices[face[0]].y) * (transformedVertices[face[2]].z - transformedVertices[face[0]].z) - (transformedVertices[face[1]].z - transformedVertices[face[0]].z) * (transformedVertices[face[2]].y - transformedVertices[face[0]].y),
                (transformedVertices[face[1]].z - transformedVertices[face[0]].z) * (transformedVertices[face[2]].x - transformedVertices[face[0]].x) - (transformedVertices[face[1]].x - transformedVertices[face[0]].x) * (transformedVertices[face[2]].z - transformedVertices[face[0]].z),
                (transformedVertices[face[1]].x - transformedVertices[face[0]].x) * (transformedVertices[face[2]].y - transformedVertices[face[0]].y) - (transformedVertices[face[1]].y - transformedVertices[face[0]].y) * (transformedVertices[face[2]].x - transformedVertices[face[0]].x)
            };
            if (normal.z < 0) {
                continue;
            }

            // ����ȭ�� ���� ��ǥ
            vector<Point3D> normalizedVertices;
            for (auto& index : face) {
                Point3D vertex = transformedVertices[index];
                float x = vertex.x / (vertex.z * tan(fov / 2) * aspectRatio);
                float y = vertex.y / (vertex.z * tan(fov / 2));
                normalizedVertices.push_back({ x, y, vertex.z });
            }

            // ��ũ�� ��ǥ
            vector<Point3D> screenVertices;
            for (auto& vertex : normalizedVertices) {
                int x = (int)((vertex.x + 1.0f) * 0.5f * screenWidth);
                int y = (int)((-vertex.y + 1.0f) * 0.5f * screenHeight);
                screenVertices.push_back({ (float)x, (float)y, vertex.z });
            }

            // �� ����
            char color = 'X';

            // �� �׸���
            for (int i = 0; i < screenVertices.size(); i++) {
                Point3D p1 = screenVertices[i];
                Point3D p2 = screenVertices[(i + 1) % screenVertices.size()];
                int x0 = (int)p1.x;
                int y0 = (int)p1.y;
                int x1 = (int)p2.x;
                int y1 = (int)p2.y;

                if (x1 == x0) {
                    if (y1 < y0) {
                        swap(y0, y1);
                    }
                    for (int y = y0; y <= y1; y++) {
                        int index = y * screenWidth + x0;
                        screen[index] = color;
                    }
                }
                else {
                    float slope = (float)(y1 - y0) / (float)(x1 - x0);
                    if (fabs(slope) <= 1.0f) {
                        if (x1 < x0) {
                            swap(x0, x1);
                            swap(y0, y1);
                        }
                        float y = (float)y0;
                        for (int x = x0; x <= x1; x++) {
                            int index = (int)y * screenWidth + x;
                            screen[index] = color;
                            y += slope;
                        }
                    }
                    else {
                        if (y1 < y0) {
                            swap(x0, x1);
                            swap(y0, y1);
                        }
                        float x = (float)x0;
                        for (int y = y0; y <= y1; y++) {
                            int index = y * screenWidth + (int)x;
                            screen[index] = color;
                            x += 1.0f / slope;
                        }
                    }
                }
            }
        }

        // ȭ�� ���
        for (int y = 0; y < screenHeight; y++) {
            for (int x = 0; x < screenWidth; x++) {
                cout << screen[y * screenWidth + x];
            }
            cout << endl;
        }

        // ȸ���� ������Ʈ
        rotationY += rotationSpeed;
        rotationZ += rotationSpeed;
    }

    return 0;
}

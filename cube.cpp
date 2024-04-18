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

// 큐브를 나타내는 정점 좌표값
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

// 큐브를 나타내는 면
vector<vector<int>> faces = {
    { 0, 1, 2, 3 }, // 앞면
    { 1, 5, 6, 2 }, // 오른쪽면
    { 4, 0, 3, 7 }, // 왼쪽면
    { 5, 4, 7, 6 }, // 뒷면
    { 3, 2, 6, 7 }, // 위쪽면
    { 0, 4, 5, 1 }  // 아래쪽면
};

// 회전 행렬
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

// 각도 변환 함수
float toRadians(float degrees) {
    return degrees * M_PI / 180.0f;
}

// 메인 함수
int main() {
    // 큐브 초기화
    const int screenWidth = 800;
    const int screenHeight = 600;
    const float aspectRatio = (float)screenWidth / (float)screenHeight;
    const float near = 0.1f;
    const float far = 100.0f;
    const float fov = toRadians(60.0f);

    // 콘솔창에서 출력하기 위해, 대략적인 화면을 만듭니다.
    vector<char> screen(screenWidth * screenHeight, ' ');

    // 회전값 초기화
    float rotationX = 0.0;
    float rotationY = 0.0;
    float rotationZ = 0.0;

    // 회전각 속도
    const float rotationSpeed = 0.02;

    // 무한 루프
    while (true) {
        // 회전 행렬 계산
        vector<Point3D> transformedVertices = vertices;
        rotate(transformedVertices, rotationX, 'x');
        rotate(transformedVertices, rotationY, 'y');
        rotate(transformedVertices, rotationZ, 'z');

        // 화면 초기화
        for (int i = 0; i < screen.size(); i++) {
            screen[i] = ' ';
        }

        // 화면에 큐브 그리기
        for (auto& face : faces) {
            // 정면으로 보이지 않으면 그리지 않습니다.
            Point3D normal = {
                (transformedVertices[face[1]].y - transformedVertices[face[0]].y) * (transformedVertices[face[2]].z - transformedVertices[face[0]].z) - (transformedVertices[face[1]].z - transformedVertices[face[0]].z) * (transformedVertices[face[2]].y - transformedVertices[face[0]].y),
                (transformedVertices[face[1]].z - transformedVertices[face[0]].z) * (transformedVertices[face[2]].x - transformedVertices[face[0]].x) - (transformedVertices[face[1]].x - transformedVertices[face[0]].x) * (transformedVertices[face[2]].z - transformedVertices[face[0]].z),
                (transformedVertices[face[1]].x - transformedVertices[face[0]].x) * (transformedVertices[face[2]].y - transformedVertices[face[0]].y) - (transformedVertices[face[1]].y - transformedVertices[face[0]].y) * (transformedVertices[face[2]].x - transformedVertices[face[0]].x)
            };
            if (normal.z < 0) {
                continue;
            }

            // 정규화된 정점 좌표
            vector<Point3D> normalizedVertices;
            for (auto& index : face) {
                Point3D vertex = transformedVertices[index];
                float x = vertex.x / (vertex.z * tan(fov / 2) * aspectRatio);
                float y = vertex.y / (vertex.z * tan(fov / 2));
                normalizedVertices.push_back({ x, y, vertex.z });
            }

            // 스크린 좌표
            vector<Point3D> screenVertices;
            for (auto& vertex : normalizedVertices) {
                int x = (int)((vertex.x + 1.0f) * 0.5f * screenWidth);
                int y = (int)((-vertex.y + 1.0f) * 0.5f * screenHeight);
                screenVertices.push_back({ (float)x, (float)y, vertex.z });
            }

            // 면 색상
            char color = 'X';

            // 면 그리기
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

        // 화면 출력
        for (int y = 0; y < screenHeight; y++) {
            for (int x = 0; x < screenWidth; x++) {
                cout << screen[y * screenWidth + x];
            }
            cout << endl;
        }

        // 회전각 업데이트
        rotationY += rotationSpeed;
        rotationZ += rotationSpeed;
    }

    return 0;
}

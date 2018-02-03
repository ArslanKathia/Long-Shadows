#include <jni.h>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <map>
#include <set>
#include <cmath>
#include <iostream>
#include <assert.h>
#include <algorithm>
#include <android/log.h>

#define ALPHA_0 0

#define PI 3.14159265

using namespace std;

struct ShadowPath {
    vector<pair<int, int> > points;
    pair<int, int> startPointOne;
    pair<int, int> startPointTwo;
    pair<int, int> endPointOne;
    pair<int, int> endPointTwo;
};

vector<vector<pair<int, int> > > contours(int arr[], int width, int height) {
    int mat[height + 1][width + 1];
    int id[height + 1][width + 1];
    queue<pair<int, int> > q;

    vector<vector<pair<int, int> > > ans;
    ans.clear();

    vector<pair<int, int> > temp;

    int n = width * height;

    for (int i = 0; i < n; i++) {
        mat[i / width][i % width] = arr[i];
        id[i / width][i % width] = 0;
    }

    int cnt = 0, x, y, x1, y1;
    pair<int, int> tp;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (id[i][j] == 0 && mat[i][j] != ALPHA_0) {
                cnt++;
                temp.clear();

                while (!q.empty())
                    q.pop();

                q.push(make_pair(i, j));

                while (!q.empty()) {
                    tp = q.front();
                    q.pop();
                    x = tp.first;
                    y = tp.second;

                    if (x < 0 || y < 0 || x >= height || y >= width)
                        continue;

                    if (mat[x][y] == ALPHA_0)
                        continue;

                    if (id[x][y] != 0) {
                        assert(id[x][y] == cnt);
                        continue;
                    }

                    assert(id[x][y] == 0);
                    assert(mat[x][y] != ALPHA_0);

                    id[x][y] = cnt;

                    bool flag = false;

                    for (int i1 = -1; i1 < 2; i1++) {
                        for (int j1 = -1; j1 < 2; j1++) {
                            x1 = x + i1;
                            y1 = y + j1;

                            if (x1 < 0 || y1 < 0 || x1 >= height || y1 >= width) {
                                flag = true;
                                break;
                            }

                            if (mat[x1][y1] == ALPHA_0) {
                                flag = true;
                                break;
                            }
                        }

                        if (flag)
                            break;
                    }

                    if (flag)
                        temp.push_back(make_pair(y, x));

                    q.push(make_pair(x - 1, y - 1));
                    q.push(make_pair(x - 1, y));
                    q.push(make_pair(x - 1, y + 1));
                    q.push(make_pair(x, y - 1));
                    q.push(make_pair(x, y + 1));
                    q.push(make_pair(x + 1, y - 1));
                    q.push(make_pair(x + 1, y));
                    q.push(make_pair(x + 1, y + 1));
                }

                ans.push_back(temp);
            }
        }
    }

    return ans;
}

jobjectArray convertToObjectArray(JNIEnv *env, vector<ShadowPath> shadowPaths) {

    jclass shadowPathClass = (env)->FindClass("com/sdsmdg/harjot/longshadows/models/ShadowPath");
    jobjectArray shadowObjectArray = (env)->NewObjectArray(shadowPaths.size(), shadowPathClass,
                                                           NULL);
    jmethodID shadowClassConstructor = (env)->GetMethodID(shadowPathClass, "<init>",
                                                          "([Lcom/sdsmdg/harjot/longshadows/models/Point2D;"
                                                                  "Lcom/sdsmdg/harjot/longshadows/models/Point2D;"
                                                                  "Lcom/sdsmdg/harjot/longshadows/models/Point2D;"
                                                                  "Lcom/sdsmdg/harjot/longshadows/models/Point2D;"
                                                                  "Lcom/sdsmdg/harjot/longshadows/models/Point2D;)"
                                                                  "V");
    jobject shadowPathObject;

    for (int i = 0; i < shadowPaths.size(); i++) {

        vector<pair<int, int> > points = shadowPaths[i].points;

        jclass pointClass = (env)->FindClass("com/sdsmdg/harjot/longshadows/models/Point2D");
        jobjectArray pointArray = (env)->NewObjectArray(points.size(), pointClass, NULL);
        jmethodID pointClassConstructor = (env)->GetMethodID(pointClass, "<init>", "(II)V");
        jobject pointObject;

        for (int j = 0; j < points.size(); j++) {
            pointObject = (env)->NewObject(pointClass, pointClassConstructor, points[j].first,
                                           points[j].second);
            (env)->SetObjectArrayElement(pointArray, j, pointObject);
            (env)->DeleteLocalRef(pointObject);
        }

        jobject startPointOne = (env)->NewObject(pointClass, pointClassConstructor,
                                                 shadowPaths[i].startPointOne.first,
                                                 shadowPaths[i].startPointOne.second);
        jobject startPointTwo = (env)->NewObject(pointClass, pointClassConstructor,
                                                 shadowPaths[i].startPointTwo.first,
                                                 shadowPaths[i].startPointTwo.second);
        jobject endPointOne = (env)->NewObject(pointClass, pointClassConstructor,
                                               shadowPaths[i].endPointOne.first,
                                               shadowPaths[i].endPointOne.second);
        jobject endPointTwo = (env)->NewObject(pointClass, pointClassConstructor,
                                               shadowPaths[i].endPointTwo.first,
                                               shadowPaths[i].endPointTwo.second);

        shadowPathObject = (env)->NewObject(shadowPathClass,
                                            shadowClassConstructor, pointArray, startPointOne,
                                            startPointTwo, endPointOne, endPointTwo);

        (env)->SetObjectArrayElement(shadowObjectArray, i, shadowPathObject);
        (env)->DeleteLocalRef(shadowPathObject);
        (env)->DeleteLocalRef(pointArray);

    }

    return shadowObjectArray;

}

vector<pair<int, int> >
getPath(pair<int, int> src, pair<int, int> dest, set<pair<int, int> > Graph) {
    map<pair<int, int>, bool> visited;
    map<pair<int, int>, pair<int, int> > parent;
    visited.clear();
    parent.clear();

    queue<pair<int, int> > Q;
    while (!Q.empty())
        Q.pop();

    Q.push(src);

    visited[src] = true;
    parent[src] = src;

    pair<int, int> tp;
    pair<int, int> pt;
    int x, y;

    while (!Q.empty()) {
        tp = Q.front();
        Q.pop();

        for (int i = -1; i < 2; i++) {
            for (int j = -1; j < 2; j++) {
                x = tp.first + i;
                y = tp.second + j;
                pt = make_pair(x, y);
                if (Graph.find(pt) != Graph.end() && !visited[pt]) {
                    visited[pt] = true;
                    parent[pt] = tp;
                    Q.push(pt);
                }
            }
        }
    }

    vector<pair<int, int> > path;
    path.clear();

    if (visited[dest] == false) {
        assert(false);
        return path;
    }

    pair<int, int> now = dest;
    while (now != parent[now]) {
        path.push_back(now);
        now = parent[now];
    }
    path.push_back(now);

    return path;
}

long double crossProduct(pair<int, int> p1, pair<int, int> p2, pair<int, int> p3) {
    long double x1, y1, x2, y2;

    x1 = p2.first - p1.first;
    y1 = p2.second - p1.second;

    x2 = p3.first - p1.first;
    y2 = p3.second - p1.second;

    long double ans = x1 * y2 - x2 * y1;

    return ans;
}

long double getArea(vector<pair<int, int> > path, pair<int, int> ref) {
    vector<pair<long double, pair<int, int> > > polarOrder;
    polarOrder.clear();

    long double angle;
    long double H, B;

    for (int i = 0; i < path.size(); i++) {
        H = path[i].second - ref.second;
        B = path[i].first - ref.first;

        angle = atan(H / B);

        polarOrder.push_back(make_pair(angle, path[i]));
    }

    sort(polarOrder.begin(), polarOrder.end());

    path.clear();
    for (int i = 0; i < polarOrder.size(); i++)
        path.push_back(polarOrder[i].second);

    long double area = 0.0;
    for (int i = 1; i < path.size(); i++)
        area = area + crossProduct(ref, path[i - 1], path[i]);

    area = area / 2.0;
    area = abs(area);

    return area;
}

vector<pair<int, int> > getLargestComponent(set<pair<int, int> > Graph) {
    assert(Graph.size() > 0);

    vector<pair<int, int> > component;
    vector<pair<int, int> > now;
    component.clear();

    map<pair<int, int>, bool> visited;
    visited.clear();

    for (set<pair<int, int> >::iterator it = Graph.begin(); it != Graph.end(); it++) {
        if (visited[*it])
            continue;

        now.clear();
        now.push_back(*it);

        queue<pair<int, int> > Q;
        while (!Q.empty())
            Q.pop();

        Q.push(*it);

        visited[*it] = true;

        pair<int, int> tp;
        pair<int, int> pt;
        int x, y;

        while (!Q.empty()) {
            tp = Q.front();
            Q.pop();

            for (int i = -1; i < 2; i++) {
                for (int j = -1; j < 2; j++) {
                    x = tp.first + i;
                    y = tp.second + j;
                    pt = make_pair(x, y);
                    if (Graph.find(pt) != Graph.end() && !visited[pt]) {
                        visited[pt] = true;
                        Q.push(pt);
                        now.push_back(pt);
                    }
                }
            }
        }
        if (component.size() < now.size()) {
            component.clear();
            for (int i = 0; i < now.size(); i++)
                component.push_back(now[i]);
        }
    }

    return component;
}

vector<pair<int, int> > getPolarOrder(vector<pair<int, int> > path, pair<int, int> ref) {
    vector<pair<long double, pair<int, int> > > polarOrder;
    polarOrder.clear();

    long double angle;
    long double H, B;

    for (int i = 0; i < path.size(); i++) {
        H = path[i].second - ref.second;
        B = path[i].first - ref.first;

        angle = atan(H / B);

        polarOrder.push_back(make_pair(angle, path[i]));
    }

    vector<pair<long double, pair<int, int> > > temp;
    temp.clear();

    for (int i = 0; i < polarOrder.size(); i++) {
        if (temp.size() == 0)
            temp.push_back(polarOrder[i]);
        else if (temp[temp.size() - 1].first > polarOrder[i].first)
            temp.push_back(polarOrder[i]);
    }

    path.clear();
    for (int i = 0; i < temp.size(); i++)
        path.push_back(temp[i].second);

    return path;
}

vector<pair<int, int> > getOuterBoundary(pair<int, int> src, set<pair<int, int> > Graph) {
    map<pair<int, int>, bool> visited;
    visited.clear();

    vector<pair<int, int> > outerBoundary;
    outerBoundary.clear();

    queue<pair<int, int> > Q;
    while (!Q.empty())
        Q.pop();

    Q.push(src);
    visited[src] = true;

    outerBoundary.push_back(src);

    pair<int, int> tp;
    pair<int, int> pt;
    int x, y;

    while (!Q.empty()) {
        tp = Q.front();
        Q.pop();

        for (int i = -1; i < 2; i++) {
            for (int j = -1; j < 2; j++) {
                x = tp.first + i;
                y = tp.second + j;
                pt = make_pair(x, y);
                if (Graph.find(pt) != Graph.end() && !visited[pt]) {
                    visited[pt] = true;
                    Q.push(pt);
                    outerBoundary.push_back(pt);
                }
            }
        }
    }

    return outerBoundary;
}

vector<pair<int, int> > boundaryPath(vector<pair<int, int> > pts, pair<int, int> ref, int what) {
    set<pair<int, int> > boundary;
    boundary.clear();

    for (int i = 0; i < pts.size(); i++)
        boundary.insert(pts[i]);

    vector<pair<long double, pair<int, int> > > polarOrder;
    polarOrder.clear();

    long double angle;
    long double H, B;

    for (set<pair<int, int> >::iterator it = boundary.begin(); it != boundary.end(); it++) {
        H = (it->second) - ref.second;
        B = (it->first) - ref.first;
        angle = atan(H / B);
        polarOrder.push_back(make_pair(angle, *it));
    }

    sort(polarOrder.begin(), polarOrder.end());

    pair<int, int> tangent1, tangent2;
    tangent1 = polarOrder[0].second;
    tangent2 = polarOrder[polarOrder.size() - 1].second;

    vector<pair<int, int> > outerBoundary = getOuterBoundary(tangent1, boundary);

    boundary.clear();
    for (int i = 0; i < outerBoundary.size(); i++)
        boundary.insert(outerBoundary[i]);

    vector<pair<int, int> > path1 = getPath(tangent1, tangent2, boundary);

    vector<pair<int, int> > path2;
    path2.clear();

    for (int i = 0; i < path1.size(); i++)
        boundary.erase(path1[i]);

    for (set<pair<int, int> >::iterator it = boundary.begin(); it != boundary.end(); it++)
        path2.push_back(*it);

    long double area1 = getArea(path1, ref);
    long double area2 = getArea(path2, ref);

    vector<pair<int, int> > component = getLargestComponent(boundary);

    boundary.clear();
    for (int i = 0; i < component.size(); i++)
        boundary.insert(component[i]);

    polarOrder.clear();
    for (set<pair<int, int> >::iterator it = boundary.begin(); it != boundary.end(); it++) {
        H = (it->second) - ref.second;
        B = (it->first) - ref.first;
        angle = atan(H / B);
        polarOrder.push_back(make_pair(angle, *it));
    }

    sort(polarOrder.begin(), polarOrder.end());

    tangent1 = polarOrder[0].second;
    tangent2 = polarOrder[polarOrder.size() - 1].second;

    path2 = getPath(tangent1, tangent2, boundary);

    vector<pair<int, int> > path1PolarOrder;
    vector<pair<int, int> > path2PolarOrder;
    path1PolarOrder.clear();
    path2PolarOrder.clear();

    path1PolarOrder = getPolarOrder(path1, ref);
    path2PolarOrder = getPolarOrder(path2, ref);

    vector<pair<int, int> > fullPath;
    vector<pair<int, int> > fullPathPolarOrder;
    fullPath.clear();
    fullPathPolarOrder.clear();

    for (int i = 0; i < path1.size(); i++)
        fullPath.push_back(path1[i]);
    for (int i = path2.size() - 1; i >= 0; i--)
        fullPath.push_back(path2[i]);

    for (int i = 0; i < path1PolarOrder.size(); i++)
        fullPathPolarOrder.push_back(path1PolarOrder[i]);
    for (int i = path2PolarOrder.size() - 1; i >= 0; i--)
        fullPathPolarOrder.push_back(path2PolarOrder[i]);

    bool flag = false;
    if (area1 < area2)
        flag = true;

    if (what == 0) // Front Path
    {
        if (flag)
            return path1;
        return path2;
    }
    if (what == 1) // Front Polar Path
    {
        if (flag)
            return path1PolarOrder;
        return path2PolarOrder;
    }
    if (what == 2) // Back Path
    {
        if (!flag)
            return path1;
        return path2;
    }
    if (what == 3) // Back Polar Path
    {
        if (!flag)
            return path1PolarOrder;
        return path2PolarOrder;
    }
    if (what == 4) // Full Path
        return fullPath;
    if (what == 5) // Full Polar Path
        return fullPathPolarOrder;
}

ShadowPath
getFinalPathPointsFromContour(vector<pair<int, int> > points, int width, int height, int angle,
                              int shadowLength) {

    __android_log_print(ANDROID_LOG_DEBUG, "POINTS", "size_1 : %lu", points.size());

    pair<int, int> ref = make_pair(width / 2 - (2000 * cos(angle * PI / 180)),
                                   height / 2 - (2000 * sin(angle * PI / 180)));

    vector<pair<int, int> > boundary_front_polar = boundaryPath(points, ref, 1);
    vector<pair<int, int> > boundary_back_polar = boundaryPath(points, ref, 3);

    int boundary_front_polar_size = boundary_front_polar.size();
    int boundary_back_polar_size = boundary_back_polar.size();

    vector<pair<int, int> > pathPoints;

    for (int i = 0; i < boundary_front_polar.size(); i++) {
        pathPoints.push_back(boundary_front_polar[i]);
    }

    for (int i = 0; i < boundary_back_polar.size(); i++) {
        boundary_back_polar[i].first += shadowLength * cos(angle * PI / 180);
        boundary_back_polar[i].second += shadowLength * sin(angle * PI / 180);
    }

    for (int i = boundary_back_polar.size() - 1; i >= 0; i--) {
        pathPoints.push_back(boundary_back_polar[i]);
    }

    ShadowPath shadowPath;

    shadowPath.points = pathPoints;
    shadowPath.startPointOne = boundary_front_polar[0];
    shadowPath.startPointTwo = boundary_front_polar[boundary_front_polar_size - 1];
    shadowPath.endPointOne = boundary_back_polar[0];
    shadowPath.endPointTwo = boundary_back_polar[boundary_back_polar_size - 1];

    return shadowPath;

}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_sdsmdg_harjot_longshadows_shadowutils_LongShadowsGenerator_getContours(
        JNIEnv *env,
        jobject,
        jintArray arr,
        jint width,
        jint height,
        jint angle,
        jint shadowLength) {

    vector<vector<pair<int, int> > > ans;
    ans.clear();

    jint *c_array = (env)->GetIntArrayElements(arr, NULL);

    __android_log_print(ANDROID_LOG_DEBUG, "TIME_CPP", " 1 ");

    ans = contours(c_array, width, height);

    __android_log_print(ANDROID_LOG_DEBUG, "TIME_CPP", " 2 ");

    __android_log_print(ANDROID_LOG_DEBUG, "TIME_CPP", " 3 ");

    vector<ShadowPath> shadowPaths;

    for (int i = 0; i < ans.size(); i++) {

        __android_log_print(ANDROID_LOG_DEBUG, "TIME_CPP", " 3.%d.%d ", i + 1, 0);

        shadowPaths.push_back(
                getFinalPathPointsFromContour(ans[i], width, height, angle, shadowLength));

        __android_log_print(ANDROID_LOG_DEBUG, "TIME_CPP", " 3.%d.%d ", i + 1, 1);
    }

    __android_log_print(ANDROID_LOG_DEBUG, "TIME_CPP", " 4 ");

    return convertToObjectArray(env, shadowPaths);

}

// SPDX-License-Identifier: GPL-3.0-only
// (C) 2024 Vassilij Nadarajah, TU Berlin
// nadarajah@campus.tu-berlin.de

#include <cstdio>
#include <vector>

#include "simpleocv.h"
#include "src/yolov7.h"

using namespace yolo;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s [imagepath]\n", argv[0]);
        return -1;
    }

    const char* imagepath = argv[1];

    cv::Mat m = cv::imread(imagepath, 1);
    if (m.empty())
    {
        fprintf(stderr, "cv::imread %s failed\n", imagepath);
        return -1;
    }

    std::vector<Object> objects;
    YoloV7::detect(m, objects);

    YoloV7::draw_objects(m, objects);

    return 0;
}


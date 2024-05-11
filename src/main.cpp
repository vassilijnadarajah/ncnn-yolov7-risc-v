// SPDX-License-Identifier: GPL-3.0-only
// (C) 2024 Vassilij Nadarajah, TU Berlin
// nadarajah@campus.tu-berlin.de

#include <cstdio>
#include <vector>

#include "simpleocv.h"
#include "YoloV7.h"

using namespace Yolo;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s [imagepath]\n", argv[0]);
        return -1;
    }

    char* imagepath = argv[1];

    // Load image
    cv::Mat m = cv::imread(imagepath, 1);
    if (m.empty())
    {
        fprintf(stderr, "cv::imread %s failed\n", imagepath);
        return -1;
    }

    // Create YOLOv7 object for inference, anchors are from YOLOv7's autoanchor function
    std::vector<float> anchors = {12, 16, 19, 36, 40, 28, 36, 75, 76, 55, 72, 146, 142, 110, 192, 243, 459, 401};
    YoloV7 yolov7 = YoloV7(640, 80, 0.25, 0.5, anchors);

    // Perform inference
    std::vector<Object> objects;
    yolov7.detect(m, objects);

    // Create output image with bounding boxes
    yolov7.draw_objects(m, objects);

    // Optionally write detection results to file
    yolov7.write_objects(objects, imagepath);

    return 0;
}


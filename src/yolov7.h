// SPDX-License-Identifier: GPL-3.0-only
// (C) 2024 Vassilij Nadarajah, TU Berlin
// nadarajah@campus.tu-berlin.de

#ifndef YOLOV7_H
#define YOLOV7_H

#include "net.h"
#include "simpleocv.h"

#include <unistd.h>

#include <cfloat>
#include <cstdio>
#include <vector>

#define MAX_STRIDE 32

namespace yolo {

    struct Object {
        cv::Rect_<float> rect;
        int label;
        float prob;
    };

    class YoloV7 {
    public:
        static const char* path_to_param;
        static const char* path_to_bin;

        static inline float intersection_area(
                const Object &a,
                const Object &b
                );

        static void qsort_descent_inplace(
                std::vector<Object> &objects,
                int left,
                int right
                );

        static void qsort_descent_inplace(std::vector<Object> &objects);

        static void nms_sorted_bboxes(const std::vector<Object> &faceobjects,
                                      std::vector<int> &picked,
                                      float nms_threshold,
                                      bool agnostic = false);

        static inline float sigmoid(float x);

        static void generate_proposals(
                const ncnn::Mat &anchors,
                int stride,
                const ncnn::Mat &in_pad,
                const ncnn::Mat &feat_blob,
                float prob_threshold,
                std::vector<Object> &objects
                );

        static int detect(
                const cv::Mat &bgr,
                std::vector<Object> &objects
                );

        static void draw_objects(
                const cv::Mat &bgr,
                const std::vector<Object> &objects
                );
    };
}

#endif //YOLOV7_H

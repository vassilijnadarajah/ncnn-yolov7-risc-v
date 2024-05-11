// SPDX-License-Identifier: GPL-3.0-only
// (C) 2024 Vassilij Nadarajah, TU Berlin
// nadarajah@campus.tu-berlin.de

#ifndef NCNN_YOLO_YOLOV7_H
#define NCNN_YOLO_YOLOV7_H

#include "net.h"
#include "simpleocv.h"

#include <unistd.h>

#include <cfloat>
#include <cstdio>
#include <vector>

namespace Yolo {

    struct Object {
        cv::Rect_<float> rect;
        int label{};
        float prob{};
    };

    class YoloV7 {
    public:
        /// @brief Constructor
        /// @param target_size Target size of the input image, default is `640`
        /// @param num_classes Nnumber of classes, default is `80` based on COCO dataset
        /// @param prob_threshold Probability threshold for predictions, default is `0.25`
        /// @param nms_threshold Non-maximum suppression threshold, default is `0.5`
        /// @param anchors Vector of anchor boxes, can be obtained by YOLOv7's autoanchor function
        /// @param path_to_param Path to the ncnn model param file
        /// @param path_to_bin Path to the ncnn model bin file
        explicit YoloV7(int target_size = 640,
                        int num_classes = 80,
                        float prob_threshold = 0.25,
                        float nms_threshold = 0.5,
                        const std::vector<float> &anchors = {12, 16, 19, 36, 40, 28, 36, 75, 76, 55, 72, 146, 142, 110, 192, 243, 459, 401},
                        const char* path_to_param = "../resources/yolov7_tiny.torchscript.ncnn.param",
                        const char* path_to_bin = "../resources/yolov7_tiny.torchscript.ncnn.bin");

        /// @brief Performs inference on an image
        /// @param bgr Input image in BGR format
        /// @param objects Vector of predicted object detections
        void detect(const cv::Mat &bgr,
                   std::vector<Object> &objects);

        /// @brief Creates the output image with bounding boxes
        /// @param bgr Input image in BGR format
        /// @param objects Predictions from the inference
        void draw_objects(const cv::Mat &bgr, 
                          const std::vector<Object> &objects);

        /// @brief Writes the predictions to a `.txt` file
        /// \param objects Predicted object detections
        /// \param filename Name of the image file
        void write_objects(const std::vector<Object> &objects,
                           char* filename);

    private:
        int target_size;
        int num_classes;
        float prob_threshold;
        float nms_threshold;
        std::vector<float> anchors;
        const char* path_to_param;
        const char* path_to_bin;

        inline float intersection_area(const Object &a, 
                                       const Object &b);

        void qsort_descent_inplace(std::vector<Object> &objects, 
                                   int left, 
                                   int right);

        void qsort_descent_inplace(std::vector<Object> &objects);

        void nms_sorted_bboxes(const std::vector<Object> &faceobjects, 
                               std::vector<int> &picked,
                               bool agnostic = false);

        static inline float sigmoid(float x);

        void extract_proposals(ncnn::Extractor &ex, 
                               const char* output_name,
                               int anchor_idx,
                               int stride,
                               const ncnn::Mat &in_pad, 
                               std::vector<Object> &objects,
                               double *inference_time);

        void generate_proposals(const ncnn::Mat &anchors, 
                                int stride, 
                                const ncnn::Mat &in_pad, 
                                const ncnn::Mat &feat_blob,
                                std::vector<Object> &objects);
    };
}

#endif //NCNN_YOLO_YOLOV7_H

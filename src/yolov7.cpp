// SPDX-License-Identifier: GPL-3.0-only
// (C) 2024 Vassilij Nadarajah, TU Berlin
// nadarajah@campus.tu-berlin.de

#include <benchmark.h>
#include <libgen.h>
#include "YoloV7.h"
using namespace Yolo;

YoloV7::YoloV7(int target_size, int num_classes, float prob_threshold, float nms_threshold, const std::vector<float> &anchors, const char* path_to_param, const char* path_to_bin)
{
    this->path_to_param = path_to_param;
    this->path_to_bin = path_to_bin;
    this->target_size = target_size;
    this->num_classes = num_classes;
    this->prob_threshold = prob_threshold;
    this->nms_threshold = nms_threshold;
    this->anchors = anchors;
}

void YoloV7::detect(const cv::Mat& bgr, std::vector<Object>& objects)
{
    ncnn::Net model;

    model.opt.num_threads = 1;
    model.opt.use_vulkan_compute = false;
    // yolov7.opt.use_bf16_storage = true;

    if(model.load_param(this->path_to_param) || model.load_model(this->path_to_bin))
    {
        exit(-1);
    }

    int img_w = bgr.cols;
    int img_h = bgr.rows;

    const int max_stride = 64;

    // letterbox pad to multiple of max_stride
    int w = img_w;
    int h = img_h;
    float scale;
    if (w > h)
    {
        scale = (float)this->target_size / w;
        w = this->target_size;
        h = h * scale;
    }
    else
    {
        scale = (float)this->target_size / h;
        h = this->target_size;
        w = w * scale;
    }

    ncnn::Mat in = ncnn::Mat::from_pixels_resize(bgr.data, ncnn::Mat::PIXEL_BGR2RGB, img_w, img_h, w, h);

    // pad to target_size rectangle
    int wpad = (w + max_stride - 1) / max_stride * max_stride - w;
    int hpad = (h + max_stride - 1) / max_stride * max_stride - h;
    ncnn::Mat in_pad;
    ncnn::copy_make_border(in, in_pad, hpad / 2, hpad - hpad / 2, wpad / 2, wpad - wpad / 2, ncnn::BORDER_CONSTANT, 114.f);

    const float norm_vals[3] = {1 / 255.f, 1 / 255.f, 1 / 255.f};
    in_pad.substract_mean_normalize(nullptr, norm_vals);

    std::vector<Object> proposals;

    double inference_time = 0;
    double start = ncnn::get_current_time();

    ncnn::Extractor ex = model.create_extractor();
    ex.input("in0", in_pad);

    double end = ncnn::get_current_time();
    inference_time += end - start;

    // stride 8
    YoloV7::extract_proposals(ex, "out0", 0, 8, in_pad, proposals, &inference_time);

    // stride 16
    YoloV7::extract_proposals(ex, "out1", 6, 16, in_pad, proposals, &inference_time);

    // stride 32
    YoloV7::extract_proposals(ex, "out2", 12, 32, in_pad, proposals, &inference_time);

    // Print measured time
    fprintf(stderr, "Inference time = %.5f ms\n", inference_time);

    // sort all proposals by score from highest to lowest
    qsort_descent_inplace(proposals);

    // apply nms with nms_threshold
    std::vector<int> picked;
    nms_sorted_bboxes(proposals, picked);

    int count = picked.size();

    objects.resize(count);
    for (int i = 0; i < count; i++)
    {
        objects[i] = proposals[picked[i]];

        // adjust offset to original unpadded
        float x0 = (objects[i].rect.x - (wpad / 2)) / scale;
        float y0 = (objects[i].rect.y - (hpad / 2)) / scale;
        float x1 = (objects[i].rect.x + objects[i].rect.width - (wpad / 2)) / scale;
        float y1 = (objects[i].rect.y + objects[i].rect.height - (hpad / 2)) / scale;

        // clip
        x0 = std::max(std::min(x0, (float)(img_w - 1)), 0.f);
        y0 = std::max(std::min(y0, (float)(img_h - 1)), 0.f);
        x1 = std::max(std::min(x1, (float)(img_w - 1)), 0.f);
        y1 = std::max(std::min(y1, (float)(img_h - 1)), 0.f);

        objects[i].rect.x = x0;
        objects[i].rect.y = y0;
        objects[i].rect.width = x1 - x0;
        objects[i].rect.height = y1 - y0;
    }
}

void YoloV7::draw_objects(const cv::Mat& bgr, const std::vector<Object>& objects)
{
    // class names
    static const char* class_names[] = {
        "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light",
        "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow",
        "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
        "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard",
        "tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple",
        "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch",
        "potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone",
        "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear",
        "hair drier", "toothbrush"
    };

    // colors
    static const unsigned char colors[19][3] = {
        {54, 67, 244},
        {99, 30, 233},
        {176, 39, 156},
        {183, 58, 103},
        {181, 81, 63},
        {243, 150, 33},
        {244, 169, 3},
        {212, 188, 0},
        {136, 150, 0},
        {80, 175, 76},
        {74, 195, 139},
        {57, 220, 205},
        {59, 235, 255},
        {7, 193, 255},
        {0, 152, 255},
        {34, 87, 255},
        {72, 85, 121},
        {158, 158, 158},
        {139, 125, 96}
    };

    int color_index = 0;

    cv::Mat image = bgr.clone();

    for (size_t i = 0; i < objects.size(); i++)
    {
        const Object& obj = objects[i];

        const unsigned char* color = colors[color_index % 19];
        color_index++;

        cv::Scalar cc(color[0], color[1], color[2]);

        fprintf(stderr, "%d = %.5f at %.2f %.2f %.2f x %.2f\n", obj.label, obj.prob,
                obj.rect.x, obj.rect.y, obj.rect.width, obj.rect.height);

        cv::rectangle(image, obj.rect, cc, 2);

        char text[256];
        snprintf(text, 256, "%s %.1f%%", class_names[obj.label], obj.prob * 100);

        int baseLine = 0;
        cv::Size label_size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.4, 1, &baseLine);

        int x = obj.rect.x;
        int y = obj.rect.y - label_size.height - baseLine;
        if (y < 0)
            y = 0;
        if (x + label_size.width > image.cols)
            x = image.cols - label_size.width;

        cv::rectangle(image, cv::Rect(cv::Point(x, y), cv::Size(label_size.width, label_size.height + baseLine)),
                      cc, -1);

        cv::putText(image, text, cv::Point(x, y + label_size.height),
                    cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 255, 255));
    }

//    std::string name = "image " + YoloV7::file_counter;

    cv::imshow("image", image);
    cv::waitKey(0);
}

float YoloV7::intersection_area(const Object& a, const Object& b)
{
    cv::Rect_<float> inter = a.rect & b.rect;
    return inter.area();
}

void YoloV7::qsort_descent_inplace(std::vector<Object>& objects, int left, int right)
{
    int i = left;
    int j = right;
    float p = objects[(left + right) / 2].prob;

    while (i <= j)
    {
        while (objects[i].prob > p)
            i++;

        while (objects[j].prob < p)
            j--;

        if (i <= j)
        {
            // swap
            std::swap(objects[i], objects[j]);

            i++;
            j--;
        }
    }

#pragma omp parallel sections
    {
#pragma omp section
        {
            if (left < j) qsort_descent_inplace(objects, left, j);
        }
#pragma omp section
        {
            if (i < right) qsort_descent_inplace(objects, i, right);
        }
    }
}

void YoloV7::qsort_descent_inplace(std::vector<Object>& objects)
{
    if (objects.empty())
        return;

    qsort_descent_inplace(objects, 0, objects.size() - 1);
}

void YoloV7::nms_sorted_bboxes(const std::vector<Object>& faceobjects, std::vector<int>& picked,  bool agnostic)
{
    picked.clear();

    const int n = faceobjects.size();

    std::vector<float> areas(n);
    for (int i = 0; i < n; i++)
    {
        areas[i] = faceobjects[i].rect.area();
    }

    for (int i = 0; i < n; i++)
    {
        const Object& a = faceobjects[i];

        int keep = 1;
        for (int j = 0; j < (int)picked.size(); j++)
        {
            const Object& b = faceobjects[picked[j]];

            if (!agnostic && a.label != b.label)
                continue;

            // intersection over union
            float inter_area = intersection_area(a, b);
            float union_area = areas[i] + areas[picked[j]] - inter_area;
            // float IoU = inter_area / union_area
            if (inter_area / union_area > this->nms_threshold)
                keep = 0;
        }

        if (keep)
            picked.push_back(i);
    }
}

float YoloV7::sigmoid(float x)
{
    return static_cast<float>(1.f / (1.f + exp(-x)));
}

void YoloV7::generate_proposals(const ncnn::Mat& anchors, int stride, const ncnn::Mat& in_pad, const ncnn::Mat& feat_blob, std::vector<Object>& objects)
{
    const int num_grid_x = feat_blob.w;
    const int num_grid_y = feat_blob.h;

    const int num_anchors = anchors.w / 2;

    for (int q = 0; q < num_anchors; q++)
    {
        const int num_output = q * (this->num_classes + 5);
        const float anchor_w = anchors[q * 2];
        const float anchor_h = anchors[q * 2 + 1];

        for (int i = 0; i < num_grid_y; i++)
        {
            for (int j = 0; j < num_grid_x; j++)
            {
                // find class index with max class score
                int class_index = 0;
                float class_score = -FLT_MAX;
                for (int k = 0; k < this->num_classes; k++)
                {
                    float score = feat_blob.channel(num_output + 5 + k).row(i)[j];
                    if (score > class_score)
                    {
                        class_index = k;
                        class_score = score;
                    }
                }

                float box_score = feat_blob.channel(num_output + 4).row(i)[j];

                float confidence = sigmoid(box_score) * sigmoid(class_score);

                if (confidence >= this->prob_threshold)
                {
                    float dx = sigmoid(feat_blob.channel(num_output + 0).row(i)[j]);
                    float dy = sigmoid(feat_blob.channel(num_output + 1).row(i)[j]);
                    float dw = sigmoid(feat_blob.channel(num_output + 2).row(i)[j]);
                    float dh = sigmoid(feat_blob.channel(num_output + 3).row(i)[j]);

                    float pb_cx = (dx * 2.f - 0.5f + j) * stride;
                    float pb_cy = (dy * 2.f - 0.5f + i) * stride;

                    float pb_w = pow(dw * 2.f, 2) * anchor_w;
                    float pb_h = pow(dh * 2.f, 2) * anchor_h;

                    float x0 = pb_cx - pb_w * 0.5f;
                    float y0 = pb_cy - pb_h * 0.5f;
                    float x1 = pb_cx + pb_w * 0.5f;
                    float y1 = pb_cy + pb_h * 0.5f;

                    Object obj;
                    obj.rect.x = x0;
                    obj.rect.y = y0;
                    obj.rect.width = x1 - x0;
                    obj.rect.height = y1 - y0;
                    obj.label = class_index;
                    obj.prob = confidence;

                    objects.push_back(obj);
                }
            }
        }
    }
}

void YoloV7::extract_proposals(ncnn::Extractor& ex, const char* output_name, int anchor_idx, int stride, const ncnn::Mat& in_pad, std::vector<Object>& proposals, double* inference_time)
{
    double start = ncnn::get_current_time();
    ncnn::Mat out;
    ex.extract(output_name, out);
    double end = ncnn::get_current_time();
    *inference_time += end - start;

    ncnn::Mat anchors(6);
    anchors[0] = this->anchors[anchor_idx + 0];
    anchors[1] = this->anchors[anchor_idx + 1];
    anchors[2] = this->anchors[anchor_idx + 2];
    anchors[3] = this->anchors[anchor_idx + 3];
    anchors[4] = this->anchors[anchor_idx + 4];
    anchors[5] = this->anchors[anchor_idx + 5];

    std::vector<Object> objects;
    generate_proposals(anchors, stride, in_pad, out, objects);

    proposals.insert(proposals.end(), objects.begin(), objects.end());
}

void YoloV7::write_objects(const std::vector<Object>& objects, char* filename)
{
    filename = basename(filename);
    char* lastExt = strrchr (filename, '.');
    if (lastExt != NULL)
        *lastExt = '\0';

    char labelname[64];
    snprintf(labelname, sizeof(labelname), "%s.%s", filename, "txt");
    FILE* fp = fopen(labelname, "w");

    if (!fp)
    {
        fprintf(stderr, "fopen %s failed\n", filename);
        return;
    }

    for (const auto & obj : objects)
    {
        fprintf(fp, "%d %.5f %.5f %.5f %.5f %.5f\n", obj.label,
                obj.rect.x, obj.rect.y, obj.rect.width, obj.rect.height, obj.prob);
    }

    fclose(fp);

    fprintf(stderr, "Inference results saved in %s \n", labelname);
}
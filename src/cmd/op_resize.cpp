#include "cmd/op_resize.hpp"
#include "misc/map.hpp"

namespace op {
    bool ResizeOP::init(const map<string, string>& config) {
        get_value(config, "width", width_fno_, width_);
        get_value(config, "height", height_fno_, height_);
        get_value(config, "keep", keep_fno_, keep_);
        get_value(config, "bg", bg_fno_, bg_);

        return is_init();
    }

    bool ResizeOP::is_init() {
        return true;
    }

    cv::Mat ResizeOP::generate_background(const cv::Mat& img,
                                          int width, int height, int bg) {
        if (bg >= 0 && bg <= 255) {
            return cv::Mat(height, width, CV_8U, cv::Scalar(bg));
        }


        cv::Mat resized, background, binary;
        cv::resize(img, resized, cv::Size(width, height));
        cv::threshold(resized, binary, 0, 255, CV_THRESH_BINARY|CV_THRESH_OTSU);
        cv::erode(binary.clone(), binary, cv::Mat(), cv::Point(-1, -1), 1);

        int col_sum[width], col_avg[width], col_cnt[width];
        int row_sum[height], row_avg[height], row_cnt[height];
        std::fill_n(col_sum, width, 0);
        std::fill_n(col_cnt, width, 0);
        std::fill_n(row_sum, height, 0);
        std::fill_n(row_cnt, height, 0);

        int fg_max = 0, fg_sum = 0, fg_cnt = 0;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (binary.at<uchar>(y, x) >= 128) {
                    uchar color = resized.at<uchar>(y, x);
                    fg_sum += color;
                    fg_max = max(fg_max, (int) color);
                    col_sum[x] += color;
                    row_sum[y] += color;
                    fg_cnt++;
                    col_cnt[x]++;
                    row_cnt[y]++;
                }
            }
        }

        for (int x = 0; x < width; x++) {
            int sum = 0, cnt = 0;
            for (int i = max(0, x - 1); i < min(width, x + 1); i++) {
                sum += col_sum[i];
                cnt += col_cnt[i];
            }
            if (cnt > 0) {
                col_avg[x] = min(fg_max, (int) (sum / cnt));
            } else {
                col_avg[x] = fg_cnt > 0 ? fg_sum / fg_cnt : 0;
            }
        }

        for (int y = 0; y < height; y++) {
            int sum = 0, cnt = 0;
            for (int i = max(0, y - 1); i < min(height, y + 1); i++) {
                sum += row_sum[i];
                cnt += row_cnt[i];
            }
            if (cnt > 0) {
                row_avg[y] = min(fg_max, (int) (sum / cnt));
            } else {
                row_avg[y] = fg_cnt > 0 ? fg_sum / fg_cnt : 0;
            }
        }

        cv::Mat kernel(5, 5, CV_32F, cv::Scalar(0));
        int wsum = 0;
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                int w = 5 - abs(i-2) - abs(j-2);
                kernel.at<float>(i, j) = (float) w;
                wsum += w;
            }
        }
        kernel /= wsum;
        cv::filter2D(resized, background, resized.depth(), kernel);
        cv::GaussianBlur(background.clone(), background,
                         cv::Size(5, 5), 0, 0);

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int color = max(col_avg[x], row_avg[y]);
                if (color > background.at<uchar>(y, x)) {
                    background.at<uchar>(y, x) = (uchar) color;
                }
            }
        }

        return background;
    }

    cv::Mat ResizeOP::execute_current(const cv::Mat& img,
                                      const vector<string>& fields) {
        cv::Mat ret;
        if (!is_init()) {
            LOG(ERROR) << "ResizeOp is not initialized";
        } else {
            int width = get_field_value(fields, width_fno_, width_);
            int height = get_field_value(fields, height_fno_, height_);
            int keep = get_field_value(fields, keep_fno_, keep_);
            int bg = get_field_value(fields, bg_fno_, bg_);

            if (width <= 0 || height <= 0) {
                LOG(ERROR) << "Invalid widthxheight ("
                           << width << "x" << height
                           << "): " << get_key(fields);
            } else {
                if (keep == 0) {
                    cv::resize(img, ret, cv::Size(width, height));
                } else {
                    int tmp_width, tmp_height;
                    if (width * img.rows < height * img.cols) {
                        tmp_width = width;
                        tmp_height = max(1, img.rows * width / img.cols);
                    } else {
                        tmp_height = height;
                        tmp_width = max(1, img.cols * height / img.rows);
                    }
                    cv::Mat tmp;
                    cv::resize(img, tmp, cv::Size(tmp_width, tmp_height));

                    auto calc_bg = [tmp](std::function<bool (uchar)> checker,
                                         int bg_default) {
                        int bg = 0, cnt = 0;
                        for (int y = 0; y < tmp.rows; y++) {
                            for (int x = 0; x < tmp.cols; x++) {
                                uchar gray = tmp.at<uchar>(y, x);
                                if (checker(gray)) {
                                    cnt++;
                                    bg += gray;
                                }
                            }
                        }
                        bg = cnt > 0 ? bg / cnt : bg_default;
                        return bg;
                    };

                    if (bg == -1) {
                        bg = calc_bg([](uchar gray)  { return gray < 32; }, 0);
                    } else if (bg == 256) {
                        bg = calc_bg([](uchar gray) { return gray >= 224; }, 255);
                    }
                    ret = generate_background(img, width, height, bg);
                    tmp.copyTo(ret(cv::Rect(0, 0, tmp_width, tmp_height)));
                }
            }
        }

        return ret;
    }
}

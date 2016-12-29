#include "cmd/op_template.hpp"
#include "misc/map.hpp"

namespace op {
    boost::shared_ptr<cv::Mat> TemplateOP::open_template(const string& template_file) {
        boost::shared_ptr<cv::Mat> ret;
        if (template_file.empty()) {
            LOG(ERROR) << "Missing template file";
            return ret;
        }

        auto it = template_cache_.find(template_file);
        if (it == template_cache_.end()) {
            LOG(INFO) << "Opening template image: " << template_file;
            boost::shared_ptr<cv::Mat> img(new cv::Mat());
            *(img.get()) = cv::imread(template_file, CV_LOAD_IMAGE_GRAYSCALE);
            if (!img->data) {
                LOG(ERROR) << "Could not find or open template image "
                           << template_file;
                return ret;
            }

            LOG(INFO) << "template size: "
                      << img->cols << "x" << img->rows;

            template_cache_[template_file] = img;
        }

        return template_cache_[template_file];
    }

    bool TemplateOP::init(const map<string, string>& config) {
        template_fno_ = -1;

        CHECK(get_value(config, "template", template_fno_, template_))
            << "template is required";
        LOG(INFO) << "template_fno: " << template_fno_ << ", "
                  << "template: " << template_;

        return true;
    }

    bool TemplateOP::is_init() {
        return (template_fno_ > 0 || !template_.empty());
    }

    cv::Mat TemplateOP::execute_current(const cv::Mat& img,
                                        const vector<string>& fields) {
        if (!is_init()) {
            LOG(ERROR) << "Failed to apply template";
        } else if (img.channels() > 1) {
            LOG(ERROR) << "Not grayscale image: " << get_key(fields);
        } else {
            string template_file = get_field_value(fields, template_fno_, template_);

            cv::Mat ret = img.clone();
            boost::shared_ptr<cv::Mat> template_img
                = open_template(template_file);

            if (!template_img) {
                return ret;
            }

            // Generate binary image
            cv::Mat binary;
            cv::threshold(img, binary, 0, 255, CV_THRESH_BINARY|CV_THRESH_OTSU);

            // Calculate text bounding box
            vector<uchar> fg;
            vector<int> row_sums;
            int max_row_sum = 0;
            int width = img.cols, height = img.rows;
            int minx = width - 1, maxx = 0, miny = height - 1, maxy = 0;
            int col_sum[width], col_avg[width], col_cnt[width];
            int row_sum[height], row_avg[height], row_cnt[height];
            std::fill_n(col_sum, width, 0);
            std::fill_n(col_cnt, width, 0);
            std::fill_n(row_sum, height, 0);
            std::fill_n(row_cnt, height, 0);

            for (int y = 0; y < height; y++) {
                row_sums.push_back(width - cv::sum(binary.row(y))[0] / 255);

                for (int x = 0; x < width; x++) {
                    uchar color = binary.at<uchar>(y, x);
                    if (color < 128) {
                        minx = min(minx, x);
                        maxx = max(maxx, x);
                        miny = min(miny, y);
                        maxy = max(maxy, y);
                        fg.push_back(img.at<uchar>(y, x));
                        col_sum[x] += img.at<uchar>(y, x);
                        row_sum[y] += img.at<uchar>(y, x);
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
                    col_avg[x] = sum / cnt;
                } else {
                    col_avg[x] = 192;
                }
            }

            for (int y = 0; y < height; y++) {
                int sum = 0, cnt = 0;
                for (int i = max(0, y - 1); i < min(height, y + 1); i++) {
                    sum += row_sum[i];
                    cnt += row_cnt[i];
                }
                if (cnt > 0) {
                    row_avg[y] = sum / cnt;
                } else {
                    row_avg[y] = 192;
                }
            }

            vector<int> sums(row_sums);
            if (sums.size() > 0) {
                sort(sums.begin(), sums.end());
                max_row_sum = sums[(int)(sums.size() * 0.9)];
            }

            // Refine text bounding box
            for (int y = miny; y < height; y++) {
                if (row_sums[y] > max_row_sum * 0.5) {
                    if (row_sums[miny] < max_row_sum * 0.5) {
                        miny = y;
                    }
                    maxy = y;
                }
            }

            minx = max(0, minx - (maxx - minx) / 10);
            maxx = min(width, maxx + (maxx - minx) / 10);

            if (max_row_sum > 0) {
                int fg_width = maxx - minx + 1;
                int fg_height = maxy - miny + 1;

                int weights[5][5];
                for (int y = 0; y < 5; y++) {
                    for (int x = 0; x < 5; x++) {
                        weights[y][x] = 5 - abs(x - 2) - abs(y - 2);
                    }
                }

                for (int y = miny; y <= maxy; y++) {
                    for (int x = minx; x <= maxx; x++) {
                        int tempx = (x - minx) * template_img->cols / fg_width;
                        int tempy = (y - miny) * template_img->rows / fg_height;
                        if (template_img->at<uchar>(tempy, tempx) >= 128) {
                            int sum = 0, cnt = 0;
                            for (int ky  = y - 2; ky <= y + 2; ky++) {
                                if (ky >= 0 && ky < height) {
                                    for (int kx = x - 2; kx <= x + 2; kx++) {
                                        if (kx >= 0 && kx < width) {
                                            int w = weights[ky - y + 2][kx - x + 2];
                                            cnt += w;
                                            sum += w * img.at<uchar>(ky, kx);
                                        }
                                    }
                                }
                            }

                            int avg = sum / cnt;
                            int color = min(avg, min(col_avg[x], row_avg[y]));
                            double transparent = template_img->at<uchar>(tempy, tempx) / 255.0;
                            ret.at<uchar>(y, x) = (uchar)(255-(255-color)*transparent);
                        }
                    }
                }
            }

            return ret;
        }

        return img;
    }
}

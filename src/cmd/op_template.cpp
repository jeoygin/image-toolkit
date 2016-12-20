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
            for (int y = 0; y < height; y++) {
                int row_sum = cv::sum(binary.row(y))[0];
                row_sums.push_back(row_sum);

                for (int x = 0; x < width; x++) {
                    uchar color = binary.at<uchar>(y, x);
                    if (color >= 128) {
                        minx = min(minx, x);
                        maxx = max(maxx, x);
                        miny = min(miny, y);
                        maxy = max(maxy, y);
                        fg.push_back(img.at<uchar>(y, x));
                    }
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

            if (max_row_sum > 0) {
                int fg_width = maxx - minx + 1;
                int fg_height = maxy - miny + 1;

                int weights[7][7];
                for (int y = 0; y < 7; y++) {
                    for (int x = 0; x < 7; x++) {
                        weights[y][x] = 7 - abs(y - 3) + abs(y - 3);
                    }
                }

                sort(fg.begin(), fg.end());
                uchar fg_mid = fg[(int)(fg.size() * 0.7)];

                for (int y = 0; y < template_img->rows; y++) {
                    for (int x = 0; x < template_img->cols; x++) {
                        if (template_img->at<uchar>(y, x) >= 128) {
                            int fg_x = minx + x * fg_width / template_img->cols;
                            int fg_y = miny + y * fg_height / template_img->rows;
                            int sum = fg_mid + img.at<uchar>(fg_y, fg_x);
                            int cnt = 2;
                            for (int ky = fg_y - 3; ky <= fg_y + 3; ky++) {
                                if (ky >= miny && ky <= maxy) {
                                    for (int kx = fg_x - 3; kx <= fg_x + 3; kx++) {
                                        if (kx >= minx && kx <= maxx
                                            && img.at<uchar>(ky, kx) >= 128) {
                                            int w = weights[ky - fg_y + 3][kx - fg_x + 3];
                                            cnt += w;
                                            sum += w * img.at<uchar>(ky, kx);
                                        }
                                    }
                                }
                            }
                            if (cnt > 0) {
                                ret.at<uchar>(fg_y, fg_x) = (uchar)(sum / cnt);
                            }
                        }
                    }
                }
            }

            return ret;
        }

        return img;
    }
}

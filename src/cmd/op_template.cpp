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
        fg_from_fno_ = fg_to_fno_ = -1;
        fg_from_ = fg_to_ = -1;

        CHECK(get_string_value(config, "template", template_fno_, template_))
            << "template is required";
        LOG(INFO) << "template_fno: " << template_fno_ << ", "
                  << "template: " << template_;

        CHECK(get_int_value(config, "fg_from", fg_from_fno_, fg_from_))
            << "fg_from is required";
        LOG(INFO) << "fg_from_fno: " << fg_from_fno_ << ", "
                  << "fg_from: " << fg_from_;

        CHECK(get_int_value(config, "fg_to", fg_to_fno_, fg_to_))
            << "fg_to is required";
        LOG(INFO) << "fg_to_fno: " << fg_to_fno_ << ", "
                  << "fg_to: " << fg_to_;

        return true;
    }

    bool TemplateOP::is_init() {
        return (template_fno_ > 0 || !template_.empty())
            && (fg_from_fno_ > 0 || fg_from_ >= 0)
            && (fg_to_fno_ > 0 || fg_to_ >= 0);
    }

    cv::Mat TemplateOP::execute_current(const cv::Mat& img,
                                        const vector<string>& fields) {
        if (!is_init()) {
            LOG(ERROR) << "Failed to apply template";
        } else if (img.channels() > 1) {
            LOG(ERROR) << "Not grayscale image: " << get_key(fields);
        } else {
            string template_file = template_;
            int fg_from = fg_from_, fg_to = fg_to_;

            if (template_fno_ > 0 && template_fno_ <= fields.size()) {
                template_file = fields[template_fno_ - 1];
            }

            if (fg_from_fno_ > 0 && fg_from_fno_ <= fields.size()) {
                fg_from = std::stoi(fields[fg_from_fno_ - 1]);
            }

            if (fg_to_fno_ > 0 && fg_to_fno_ <= fields.size()) {
                fg_to = std::stoi(fields[fg_to_fno_ - 1]);
            }

            cv::Mat ret = img.clone();
            boost::shared_ptr<cv::Mat> template_img
                = open_template(template_file);

            if (!template_img) {
                return ret;
            }

            int width = img.cols, height = img.rows;
            int minx = width - 1, maxx = 0, miny = height - 1, maxy = 0;
            vector<uchar> fg;
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    uchar color = img.at<uchar>(y, x);
                    if (color >= fg_from && color <= fg_to) {
                        minx = min(minx, x);
                        maxx = max(maxx, x);
                        miny = min(miny, y);
                        maxy = max(maxy, y);
                        fg.push_back(color);
                    }
                }
            }


            if (fg.size() > 0) {
                int fg_width = maxx - minx + 1;
                int fg_height = maxy - miny + 1;

                sort(fg.begin(), fg.end());
                uchar fg_mid = fg[fg.size() * 75 / 100];


                for (int y = 0; y < template_img->rows; y++) {
                    for (int x = 0; x < template_img->cols; x++) {
                        if (template_img->at<uchar>(y, x) >= 128) {
                            int fg_x = minx + x * fg_width / template_img->cols;
                            int fg_y = miny + y * fg_height / template_img->rows;
                            ret.at<uchar>(fg_y, fg_x) = fg_mid;
                        }
                    }
                }
            }

            return ret;
        }

        return img;
    }
}

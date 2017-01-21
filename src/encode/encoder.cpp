#include "encode/encoder.hpp"
#include "encode/encoder_base64.hpp"
#include "encode/encoder_string.hpp"

namespace encode {
    boost::shared_ptr<Encoder> get_encoder(const std::string& name) {
        boost::shared_ptr<Encoder> encoder_ptr;
        if (name == "string" || name.empty()) {
            encoder_ptr.reset(new StringEncoder());
        } else if (name == "base64") {
            encoder_ptr.reset(new Base64Encoder());
        }

        return encoder_ptr;
    }
}

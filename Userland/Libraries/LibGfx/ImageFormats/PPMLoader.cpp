/*
 * Copyright (c) 2020, Hüseyin ASLITÜRK <asliturk@hotmail.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "PPMLoader.h"
#include "PortableImageLoaderCommon.h"

namespace Gfx {

ErrorOr<void> read_image_data(PPMLoadingContext& context)
{
    Vector<Gfx::Color> color_data;
    auto const context_size = context.width * context.height;
    color_data.resize(context_size);

    auto& stream = *context.stream;

    if (context.type == PPMLoadingContext::Type::ASCII) {
        for (u64 i = 0; i < context_size; ++i) {
            auto const red = TRY(read_number(stream));
            TRY(read_whitespace(context));

            auto const green = TRY(read_number(stream));
            TRY(read_whitespace(context));

            auto const blue = TRY(read_number(stream));
            TRY(read_whitespace(context));

            Color color { static_cast<u8>(red), static_cast<u8>(green), static_cast<u8>(blue) };
            if (context.format_details.max_val < 255)
                color = adjust_color(context.format_details.max_val, color);
            color_data[i] = color;
        }
    } else if (context.type == PPMLoadingContext::Type::RAWBITS) {
        for (u64 i = 0; i < context_size; ++i) {
            Array<u8, 3> pixel;
            Bytes buffer { pixel };

            TRY(stream.read_until_filled(buffer));

            color_data[i] = { pixel[0], pixel[1], pixel[2] };
        }
    }

    TRY(create_bitmap(context));

    set_pixels(context, color_data);

    context.state = PPMLoadingContext::State::Bitmap;
    return {};
}
}

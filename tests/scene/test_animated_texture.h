/**************************************************************************/
/*  test_animated_texture.h                                                      */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#ifndef TEST_ANIMATED_TEXTURE_H
#define TEST_ANIMATED_TEXTURE_H

#include "scene/resources/animated_texture.h"
#include "scene/resources/image_texture.h"
#include "core/core_bind.h"
#include "core/io/image.h"

#include "tests/test_macros.h"

namespace TestAnimatedTexture {
// Logic placed in L42-L62 is copied from other PR (https://github.com/godotengine/godot/pull/82086). After that PR is merged, no need in this logic
const int default_height = 2;
const int default_width = 2;
const int default_channels = 3;
const Image::Format default_format = Image::Format::FORMAT_RGB8;

static Ref<Image> create_test_image_base(int p_channels, Image::Format p_format) {
	Vector<uint8_t> data;
	data.resize(default_width * default_height * p_channels);

	// This loop fills the data with image pixel values (RGBA format).
	for (int y = 0; y < default_height; y++) {
		for (int x = 0; x < default_width; x++) {
			int offset = (y * default_width + x) * p_channels;
			for (int c = 0; c < p_channels; c++) {
				data.set(offset + c, 255);
			}
		}
	}

	return Image::create_from_data(default_width, default_height, false, p_format, data);
}

static Ref<Image> create_test_image() {
	return create_test_image_base(default_channels, default_format);
}

TEST_CASE("[AnimatedTexture] Initial configuration") {
    const Ref<AnimatedTexture> sut = memnew(AnimatedTexture);

    CHECK_EQ(sut->get_frames(), 1);
    CHECK_EQ(sut->get_current_frame(), 0);
    CHECK_FALSE(sut->get_one_shot());
    CHECK_FALSE(sut->get_pause());
    CHECK_EQ(sut->get_frame_duration(0), 1.0f);
    CHECK_EQ(sut->get_speed_scale(), 1.0f);
    CHECK_EQ(sut->get_width(), 1);
    CHECK_EQ(sut->get_height(), 1);
    CHECK_FALSE(sut->has_alpha());
}

TEST_CASE("[AnimatedTexture] Set frames") {
    const Ref<AnimatedTexture> sut = memnew(AnimatedTexture);

    sut->set_frames(50);

    REQUIRE_EQ(sut->get_frames(), 50);
}

TEST_CASE("[AnimatedTexture] Can't set 0 frames") {
    const Ref<AnimatedTexture> sut = memnew(AnimatedTexture);

    ERR_PRINT_OFF;
    sut->set_frames(0);
    ERR_PRINT_ON;

    REQUIRE_EQ(sut->get_frames(), 1);
}

TEST_CASE("[AnimatedTexture] Can't set more than max frames") {
    const Ref<AnimatedTexture> sut = memnew(AnimatedTexture);

    ERR_PRINT_OFF;
    sut->set_frames(AnimatedTexture::MAX_FRAMES + 1);
    ERR_PRINT_ON;

    REQUIRE_EQ(sut->get_frames(), 1);
}

TEST_CASE("[AnimatedTexture] Set current frame") {
    const Ref<AnimatedTexture> sut = memnew(AnimatedTexture);

    sut->set_frames(50);
    sut->set_current_frame(20);

    REQUIRE_EQ(sut->get_current_frame(), 20);
}

TEST_CASE("[AnimatedTexture] Set pause") {
    const Ref<AnimatedTexture> sut = memnew(AnimatedTexture);

    sut->set_pause(true);
    REQUIRE(sut->get_pause());
    sut->set_pause(false);

    REQUIRE_FALSE(sut->get_pause());
}

TEST_CASE("[AnimatedTexture] Set one shot") {
    const Ref<AnimatedTexture> sut = memnew(AnimatedTexture);

    sut->set_one_shot(true);
    REQUIRE(sut->get_one_shot());

    sut->set_one_shot(false);
    REQUIRE_FALSE(sut->get_one_shot());
}

TEST_CASE("[AnimatedTexture] Set frame texture") {
    const Ref<Image> image = create_test_image();
    const Ref<ImageTexture> image_texture = memnew(ImageTexture);
    image_texture->set_image(image);

    const Ref<AnimatedTexture> sut = memnew(AnimatedTexture);

    sut->set_frame_texture(0, image_texture);

    REQUIRE_EQ(image_texture, sut->get_frame_texture(0));

    sut->set_current_frame(0);
    REQUIRE_EQ(default_width, sut->get_width());
    REQUIRE_EQ(default_height, sut->get_height());
    REQUIRE_EQ(Size2(default_width, default_height), sut->get_size());
}

TEST_CASE("[AnimatedTexture] Get image") {
    const Ref<Image> image = create_test_image();
    const Ref<ImageTexture> image_texture = ImageTexture::create_from_image(image);
    // image_texture->set_image(image);

    const Ref<AnimatedTexture> sut = memnew(AnimatedTexture);

    sut->set_frame_texture(0, image_texture);

    REQUIRE_EQ(image->get_data(), sut->get_image()->get_data());
}

} // namespace TestAnimatedTexture

#endif // TEST_ANIMATED_TEXTURE_H

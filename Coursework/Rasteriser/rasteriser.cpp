#include <iostream>
#include <vector>
#include <lodepng.h>

struct Vec2
{
	float x, y;
};

float edgeFunction(const Vec2& a, const Vec2& b, const Vec2& c)
{
	return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

int main()
{
	std::string outputFilename = "output.png";

	const int width = 1920, height = 1080;
	const int nChannels = 4;

	// Set up an image buffer
	std::vector<uint8_t> imageBuffer(height*width*nChannels);

	Vec2 v0 = { 500.0f, 200.0f };
	Vec2 v1 = { 1400.0f, 300.0f };
	Vec2 v2 = { 900.0f, 800.0f };

 

	// Cyan background
    for(int y = 0; y < height; ++y) 
		for (int x = 0; x < width; ++x) {
			int pixelIdx = x + y * width;
			imageBuffer[pixelIdx * nChannels + 0] = 0; 
			imageBuffer[pixelIdx * nChannels + 1] = 255;
			imageBuffer[pixelIdx * nChannels + 2] = 255; 
			imageBuffer[pixelIdx * nChannels + 3] = 255; 
		}

    for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			Vec2 p = { static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f };
			float w0 = edgeFunction(v1, v2, p);
			float w1 = edgeFunction(v2, v0, p);
			float w2 = edgeFunction(v0, v1, p);
			float area = edgeFunction(v0, v1, v2);
			if (
				(w0 >= 0 && w1 >= 0 && w2 >= 0 && area > 0) ||
				(w0 <= 0 && w1 <= 0 && w2 <= 0 && area < 0)
			) {
				int pixelIdx = x + y * width;
				imageBuffer[pixelIdx * nChannels + 0] = 255; 
				imageBuffer[pixelIdx * nChannels + 1] = 0; 
				imageBuffer[pixelIdx * nChannels + 2] = 0; 
				imageBuffer[pixelIdx * nChannels + 3] = 255; 
			}
		}
	}

    // Save the image
    int errorCode;
        errorCode = lodepng::encode(outputFilename, imageBuffer, width, height);
        if (errorCode) { // check the error code, in case an error occurred.
            std::cout << "lodepng error encoding image: " << lodepng_error_text(errorCode) << std::endl;
            return errorCode;
        }

    return 0;
}

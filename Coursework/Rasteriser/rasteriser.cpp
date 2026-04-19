#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <lodepng.h>

struct Vec2
{
	float x, y;
};

struct Vertex
{ 
	float x, y, z;
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

	
	std::vector<uint8_t> imageBuffer(height*width*nChannels);
	std::vector<float> depthBuffer(width * height, std::numeric_limits<float>::infinity());

	Vertex v0 = { 500.0f, 200.0f, 0.2f };
	Vertex v1 = { 1400.0f, 300.0f, 0.5f };
	Vertex v2 = { 900.0f, 800.0f, 0.8f };

	Vec2 p0 = { v0.x, v0.y };
	Vec2 p1 = { v1.x, v1.y };
	Vec2 p2 = { v2.x, v2.y };

 

	// Cyan background
    for(int y = 0; y < height; ++y) 
		for (int x = 0; x < width; ++x) {
			int pixelIdx = x + y * width;
			imageBuffer[pixelIdx * nChannels + 0] = 0; 
			imageBuffer[pixelIdx * nChannels + 1] = 255;
			imageBuffer[pixelIdx * nChannels + 2] = 255; 
			imageBuffer[pixelIdx * nChannels + 3] = 255; 
		}

	int minX = std::min({ (int)p0.x, (int)p1.x, (int)p2.x });
	int maxX = std::max({ (int)p0.x, (int)p1.x, (int)p2.x });
	int minY = std::min({ (int)p0.y, (int)p1.y, (int)p2.y });
	int maxY = std::max({ (int)p0.y, (int)p1.y, (int)p2.y });

	minX = std::max(minX, 0);
	maxX = std::min(maxX, width - 1);
	minY = std::max(minY, 0);
	maxY = std::min(maxY, height - 1);

	float area = edgeFunction(p0, p1, p2);

    for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			Vec2 p = {x + 0.5f, y + 0.5f};
			float w0 = edgeFunction(p1, p2, p);
			float w1 = edgeFunction(p2, p0, p);
			float w2 = edgeFunction(p0, p1, p);
			
			if (
				(w0 >= 0 && w1 >= 0 && w2 >= 0 && area > 0) ||
				(w0 <= 0 && w1 <= 0 && w2 <= 0 && area < 0)
			) {

				w0 /= area;
				w1 /= area;
				w2 /= area;

				float depth = w0 * v0.z + w1 * v1.z + w2 * v2.z;
				int pixelIdx = x + y * width;

				if (depth < depthBuffer[pixelIdx]) {
					depthBuffer[pixelIdx] = depth;
				}

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

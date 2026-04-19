#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <lodepng.h>

struct Vec2
{
	float x, y;
};

struct Vec3
{ 
	float x, y, z;
};

Vec2 project(const Vec3& v, int width, int height)
{
	float f = 500.0f; // Focal length

	float x = (v.x / v.z) * f + width / 2.0f;
	float y = (v.y / v.z) * f + height / 2.0f;

	return { x, y };
}

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

	// Cyan background
    for(int y = 0; y < height; ++y) 
		for (int x = 0; x < width; ++x) {
			int pixelIdx = x + y * width;
			imageBuffer[pixelIdx * nChannels + 0] = 0; 
			imageBuffer[pixelIdx * nChannels + 1] = 255;
			imageBuffer[pixelIdx * nChannels + 2] = 255; 
			imageBuffer[pixelIdx * nChannels + 3] = 255; 
		}

	std::vector<Vec3> vertices = {
		{ -1, -1, 3}, {1, -1, 3}, {1, 1, 3}, {-1, 1, 3},
		{  -1, -1, 5}, {1, -1, 5}, {1, 1, 5}, {-1, 1, 5}
	};

	std::vector<std::vector<int>> triangles = {
		{0, 1, 2}, {0, 2, 3},
		{4, 5, 6}, {4, 6, 7},
		{0, 1, 5}, {0, 5, 4},
		{2, 3, 7}, {2, 7, 6},
		{1, 2, 6}, {1, 6, 5},
		{0, 3, 7}, {0, 7, 4}

	};

	for (auto& tri : triangles) 
	{

		Vec3 v0 = vertices[tri[0]];
		Vec3 v1 = vertices[tri[1]];
		Vec3 v2 = vertices[tri[2]];

		Vec2 p0 = project(v0, width, height);
		Vec2 p1 = project(v1, width, height);
		Vec2 p2 = project(v2, width, height);

		int minX = std::max(0, (int)std::min({ p0.x, p1.x, p2.x }));
		int maxX = std::min(width - 1, (int)std::max({ p0.x, p1.x, p2.x }));
		int minY = std::max(0, (int)std::min({ p0.y, p1.y, p2.y }));
		int maxY = std::min(height - 1, (int)std::max({ p0.y, p1.y, p2.y }));

		float area = edgeFunction(p0, p1, p2);

		for (int y = minY; y <= maxY; ++y)
			for (int x = minX; x <= maxX; ++x) {

				Vec2 p = { x + 0.5f, y + 0.5f };

				float w0 = edgeFunction(p1, p2, p);
				float w1 = edgeFunction(p2, p0, p);
				float w2 = edgeFunction(p0, p1, p);

				if (
					(w0 >= 0 && w1 >= 0 && w2 >= 0 && area > 0) ||
					(w0 <= 0 && w1 <= 0 && w2 <= 0 && area < 0)
					){
					w0 /= area;
					w1 /= area;
					w2 /= area;

					float depth = w0 * v0.z + w1 * v1.z + w2 * v2.z;

					int pixelIdx = x + y * width;

					if (depth < depthBuffer[pixelIdx]) {
						depthBuffer[pixelIdx] = depth;
						imageBuffer[pixelIdx * nChannels + 0] = 255; 
						imageBuffer[pixelIdx * nChannels + 1] = 0;
						imageBuffer[pixelIdx * nChannels + 2] = 0; 
						imageBuffer[pixelIdx * nChannels + 3] = 255; 
					}
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

//Finished!

// CS 61C Fall 2015 Project 4

// include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#endif

// include OpenMP
#if !defined(_MSC_VER)
#include <pthread.h>
#endif
#include <omp.h>

#include "calcDepthOptimized.h"
#include "calcDepthNaive.h"

/* DO NOT CHANGE ANYTHING ABOVE THIS LINE. */
#include <math.h>
// Implements the displacement function
float displacementSick(int dx, int dy) {
	// float t[4];
	// t[0] = dx * dx + dy * dy;
	// dx++;
	// t[1] = dx * dx + dy * dy;
	// dx++;
	// t[2] = dx * dx + dy * dy;
	// dx++;
	// t[3] = dx * dx + dy * dy;

	// // printf("%f %f %f %f\n", t[0], t[1], t[2], t[3]);
	// __m128 a = _mm_loadu_ps(t);
	// __m128 b = _mm_sqrt_ps(a);
	// _mm_storeu_ps(d, b);

	// // printf("%f %f %f %f\n", d[0], d[1], d[2], d[3]);

	float squaredDisplacement = dx * dx + dy * dy;
	return squaredDisplacement;
}

void calcDepthOptimized(float *depth, float *left, float *right, int imageWidth, int imageHeight, int featureWidth, int featureHeight, int maximumDisplacement)
{

	int blockStop = 2 * featureWidth / 4 * 4;
	int heightDif = imageHeight - featureHeight;
	int widthDif = imageWidth - featureWidth;


	/* The two outer for loops iterate through each pixel */
	#pragma omp parallel for
	for (int y = 0; y < imageHeight; y++)
	{
		float minimumSquaredDifference;
		float minimumDisplacement;
		float newDisplacement;
		float squaredDifference;
		__m128 sumv;
		__m128 lt;
		__m128 rt;
		__m128 diff;
		__m128 mul;
		int leftX;
		int leftY;
		int rightX;
		int rightY;
		float difference;
		float b[4];
		int ymult = y * imageWidth;
		int x;
		int dy;
		int dx;
		int boxY;
		int boxX;
		int j;



		for (x = 0; x < imageWidth; x++)
		{
			/* Set the depth to 0 if looking at edge of the image where a feature box cannot fit. */
			if ((y < featureHeight) || (y >= heightDif) || (x < featureWidth) || (x >= widthDif))
			{
				depth[ymult + x] = 0;
				continue;
			}

			minimumSquaredDifference = -1;
			minimumDisplacement = 0;

			/* Iterate through all feature boxes that fit inside the maximum displacement box.
			   centered around the current pixel. */
			for (dy = -maximumDisplacement; dy <= maximumDisplacement; dy++)
			{
				for (dx = -maximumDisplacement; dx <= maximumDisplacement; dx++)
				{
					/* Skip feature boxes that dont fit in the displacement box. */
					if (y + dy - featureHeight < 0 || y + dy + featureHeight >= imageHeight || x + dx - featureWidth < 0 || x + dx + featureWidth >= imageWidth)
					{
						continue;
					}
					squaredDifference = 0;
					leftX = x - featureWidth;
					rightX = x + dx - featureWidth;
					sumv = _mm_setzero_ps( );

					/* Sum the squared difference within a box of +/- featureHeight and +/- featureWidth. */
					for (boxY = -featureHeight; boxY <= featureHeight; boxY++)
					{
						leftY = (y + boxY) * imageWidth;
						rightY = (y + dy + boxY) * imageWidth;
						// if (featureWidth < 4) {
						// 	difference = left[leftY + leftX] - right[rightY + rightX];
						// 	squaredDifference += difference * difference;
						// 	difference = left[leftY + leftX + 1] - right[rightY + rightX + 1];
						// 	squaredDifference += difference * difference;
						// 	difference = left[leftY + leftX + 2] - right[rightY + rightX + 2];
						// 	squaredDifference += difference * difference;
						// 	difference = left[leftY + leftX + 3] - right[rightY + rightX + 3];
						// 	squaredDifference += difference * difference;
						// } else {
						// 	for (int boxX = 0; boxX < blockStop; boxX += 4) {
						//     	lt = _mm_loadu_ps((left + leftY + leftX + boxX));
						//     	rt = _mm_loadu_ps((right + rightY + rightX + boxX));
						//     	diff = _mm_sub_ps(lt, rt);
						//     	mul = _mm_mul_ps(diff, diff);
						//     	sumv = _mm_add_ps(sumv, mul);
						//     }
						// }
						// if (featureWidth % 2 != 0) {
						// 	leftTail[0] = left[leftY + leftX + blockStop];
						// 	rightTail[0] = right[rightY + rightX + blockStop];
						// 	leftTail[1] = left[leftY + leftX + blockStop + 1];
						// 	rightTail[1] = right[rightY + rightX + blockStop + 1];
						// 	leftTail[2] = left[leftY + leftX + blockStop + 2];
						// 	rightTail[2] = right[rightY + rightX + blockStop + 2];
						// 	leftTail[3] = 0;
						// 	rightTail[3] = 0;
						// } else {
						// 	leftTail[0] = left[leftY + leftX + blockStop];
						// 	rightTail[0] = right[rightY + rightX + blockStop];
						// 	leftTail[1] = 0;
						// 	rightTail[1] = 0;
						// 	leftTail[2] = 0;
						// 	rightTail[2] = 0;
						// 	leftTail[3] = 0;
						// 	rightTail[3] = 0;
						// }

				    	// lt = _mm_loadu_ps(leftTail);
				    	// rt = _mm_loadu_ps(rightTail);
				    	// diff = _mm_sub_ps(lt, rt);
				    	// mul = _mm_mul_ps(diff, diff);
				    	// sumv = _mm_add_ps(sumv, mul);

						for (boxX = 0; boxX < blockStop; boxX += 4) {
					    	lt = _mm_loadu_ps((left + leftY + leftX + boxX));
					    	rt = _mm_loadu_ps((right + rightY + rightX + boxX));
					    	diff = _mm_sub_ps(lt, rt);
					    	mul = _mm_mul_ps(diff, diff);
					    	sumv = _mm_add_ps(sumv, mul);
					    }
					 	// tail case
						difference = left[leftY + leftX + blockStop] - right[rightY + rightX + blockStop];
						squaredDifference += difference * difference;
						if (featureWidth % 2 != 0) {
							difference = left[leftY + leftX + blockStop + 1] - right[rightY + rightX + blockStop + 1];
							squaredDifference += difference * difference;

							difference = left[leftY + leftX + blockStop + 2] - right[rightY + rightX + blockStop + 2];
							squaredDifference += difference * difference;
					    }
					}
					_mm_storeu_ps( b, sumv );

					for (j = 0; j < 4; j++) {
						squaredDifference += b[j];
					}
					/*
					Check if you need to update minimum square difference.
					This is when either it has not been set yet, the current
					squared displacement is equal to the min and but the new
					displacement is less, or the current squared difference
					is less than the min square difference.
					*/
					// if ((dx + maximumDisplacement) % 4 == 0) {
					// 	displacementSick(d, dx, dy);
					// }

					// newDisplacement = d[(dx + maximumDisplacement) % 4];
					// float t = displacementNaive(dx, dy);
					// if (newDisplacement != t) {
					// 	printf("%d %d %d %d %d %d %f %f\n", dx, dy, d[0], d[1], d[2], d[3], newDisplacement, t);
					// }
					newDisplacement = displacementSick(dx, dy);
					if ((minimumSquaredDifference == -1) || ((minimumSquaredDifference == squaredDifference) && (minimumDisplacement > newDisplacement)) || (minimumSquaredDifference > squaredDifference))
					{
						minimumSquaredDifference = squaredDifference;
						minimumDisplacement = newDisplacement;
					}
				}
			}

			/*
			Set the value in the depth map.
			If max displacement is equal to 0, the depth value is just 0.
			*/
			if (minimumSquaredDifference != -1)
			{
				if (maximumDisplacement == 0)
				{
					depth[ymult + x] = 0;
				}
				else
				{
					depth[ymult + x] = sqrt(minimumDisplacement);
				}
			}
			else
			{
				depth[ymult + x] = 0;
			}
		}
	}
}

# 

# COLOR_HISTOGRAM_MATCHING

Corner points matching methods via color histogram difference and SIFT.

- Use opencv 4.5.5 to compile and execute.
- * How to run the programs:
Each program is going to show windows for image1 and image2. Mouse click to choose 4 patches. After selecting press any keyboard keys to proceed.
- ** SIFT.cpp is going to show the result by connecting patches with lines.
For example,

![https://user-images.githubusercontent.com/40450463/160964268-d1252dc6-01c5-415c-ad5b-e0bc96c34dda.png](https://user-images.githubusercontent.com/40450463/160964268-d1252dc6-01c5-415c-ad5b-e0bc96c34dda.png)

- *** colorHist.cpp is going to show the result - the squared error of histograms and matching result - by command line print-out.
For example,

![https://user-images.githubusercontent.com/40450463/160965696-6db6e971-0d86-40eb-95f8-8b43b1c5e77e.png](https://user-images.githubusercontent.com/40450463/160965696-6db6e971-0d86-40eb-95f8-8b43b1c5e77e.png)

- **** Change PATCH_SIDE_LEN to change the size of the patches.
- ***** You can designate the picture you are going to use by changing FILE_NAME_1 and FILE_NAME_2

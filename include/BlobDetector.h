class BlobDetector {
	public:
    void run(cv::Mat &img){
		

		// Setup SimpleBlobDetector parameters.
		SimpleBlobDetector::Params params;

		// Change thresholds
		params.minThreshold = 0;
		params.maxThreshold = 255;

		// Filter by Area.
		//params.filterByArea = true;
		//params.minArea = 1500;

		// Filter by Circularity
		//params.filterByCircularity = true;
		//params.minCircularity = 0.1;

		// Filter by Convexity
		//params.filterByConvexity = true;
		//params.minConvexity = 0.87;

		// Filter by Inertia
		//params.filterByInertia = true;
		//params.minInertiaRatio = 0.01;

		Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);

	

		std::vector<KeyPoint> keypoints;
		// Detect blobs
		detector->detect(img, keypoints);
		drawKeypoints( img, keypoints, img, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
		
    }
};
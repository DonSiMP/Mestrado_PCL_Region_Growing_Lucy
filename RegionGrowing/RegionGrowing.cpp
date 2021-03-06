// RegionGrowing.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"



void extractRegions(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud, std::vector <pcl::PointIndices> clusters) {
	pcl::PCDWriter writer;

	pcl::search::Search <pcl::PointXYZ>::Ptr tree = boost::shared_ptr<pcl::search::Search<pcl::PointXYZ> >(new pcl::search::KdTree<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_plane(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::EuclideanClusterExtraction<pcl::PointXYZ> ec;
	ec.setClusterTolerance(0.01); // 1cm
	ec.setMinClusterSize(100);
	ec.setMaxClusterSize(25000);
	ec.setSearchMethod(tree);
	ec.setInputCloud(cloud);
	ec.extract(clusters);

	int j = 0;
	for (std::vector<pcl::PointIndices>::const_iterator it = clusters.begin(); it != clusters.end(); ++it)
	{
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_cluster(new pcl::PointCloud<pcl::PointXYZ>);
		for (std::vector<int>::const_iterator pit = it->indices.begin(); pit != it->indices.end(); ++pit)
			cloud_cluster->points.push_back(cloud->points[*pit]); //*
		cloud_cluster->width = cloud_cluster->points.size();
		cloud_cluster->height = 1;
		cloud_cluster->is_dense = true;

		std::cout << "PointCloud representing the Cluster: " << cloud_cluster->points.size() << " data points." << std::endl;
		std::stringstream ss;
		ss << "cloud_cluster_" << j << ".pcd";
		writer.write<pcl::PointXYZ>(ss.str(), *cloud_cluster, false); //*
		j++;
	}
}


int main(int argc, char** argv) {
	
	pcl::search::Search <pcl::PointXYZRGB>::Ptr tree = boost::shared_ptr<pcl::search::Search<pcl::PointXYZRGB> >(new pcl::search::KdTree<pcl::PointXYZRGB>);

	pcl::PointCloud <pcl::PointXYZRGB>::Ptr cloud_rgb(new pcl::PointCloud <pcl::PointXYZRGB>);
	//  if ( pcl::io::loadPCDFile <pcl::PointXYZRGB> ("../../bmrt_small_crp.pcd", *cloud_rgb) == -1 )
	
	
	if (pcl::io::loadPCDFile <pcl::PointXYZRGB>("lucy.pcd", *cloud_rgb) == -1)
	{
		std::cout << "Cloud reading failed." << std::endl;
		return (-1);
	}

	pcl::IndicesPtr indices(new std::vector <int>);
	pcl::PassThrough<pcl::PointXYZRGB> pass;
	pass.setInputCloud(cloud_rgb);
	pass.setFilterFieldName("z");
	pass.setFilterLimits(0.0, 2.0);
	pass.filter(*indices);



	pcl::RegionGrowingRGB<pcl::PointXYZRGB> reg;
	reg.setInputCloud(cloud_rgb);

	reg.setIndices(indices);

	reg.setSearchMethod(tree);
	reg.setDistanceThreshold(5);
	reg.setPointColorThreshold(10);
	reg.setRegionColorThreshold(1);
	reg.setMinClusterSize(100);

	std::vector <pcl::PointIndices> clusters;
	reg.extract(clusters);

	// pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);
	// pcl::copyPointCloud(*cloud_rgb, *cloud);

	// extractRegions(cloud,clusters);

	pcl::PointCloud <pcl::PointXYZRGB>::Ptr colored_cloud = reg.getColoredCloud();

	pcl::PCDWriter writer;
	writer.write<pcl::PointXYZRGB>("segment_result.pcd", *colored_cloud, false);

	
	/*pcl::visualization::CloudViewer viewer("Cluster viewer");
	viewer.showCloud(colored_cloud);
	while (!viewer.wasStopped())
	{
		boost::this_thread::sleep(boost::posix_time::microseconds(100));
	}*/

	return (0);
}
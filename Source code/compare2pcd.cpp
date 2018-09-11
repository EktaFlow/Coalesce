#include <pcl/point_cloud.h>
#include <pcl/octree/octree_pointcloud_changedetector.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/features/normal_3d.h>
#include <pcl/surface/gp3.h>
#include <pcl/io/vtk_io.h>

#include <iostream>
#include <vector>
#include <ctime>

int
main (int argc, char** argv)
{
  srand ((unsigned int) time (NULL));

  // Octree resolution - side length of octree voxels
  float resolution = 32.0f;

  // Instantiate octree-based point cloud change detection class
  pcl::octree::OctreePointCloudChangeDetector<pcl::PointXYZ> octree (resolution);

  pcl::PointCloud<pcl::PointXYZ>::Ptr cloudA (new pcl::PointCloud<pcl::PointXYZ> );

  //Vishesh adding .pcd file to cloud A
  if (pcl::io::loadPCDFile<pcl::PointXYZ> (argv[1], *cloudA) == -1) //* load the file
  {
    PCL_ERROR ("Couldn't read file  \n");
    return (-1);
  }
  //vishesh Comment starts
  /*
  // Generate pointcloud data for cloudA

  cloudA->width = 128;
  cloudA->height = 1;
  cloudA->points.resize (cloudA->width * cloudA->height);

  for (size_t i = 0; i < cloudA->points.size (); ++i)
  {
    cloudA->points[i].x = 64.0f * rand () / (RAND_MAX + 1.0f);
    cloudA->points[i].y = 64.0f * rand () / (RAND_MAX + 1.0f);
    cloudA->points[i].z = 64.0f * rand () / (RAND_MAX + 1.0f);
  }
  */
  //Vishesh Comment ends
  // Add points from cloudA to octree
  octree.setInputCloud (cloudA);
  octree.addPointsFromInputCloud ();

  // Switch octree buffers: This resets octree but keeps previous tree structure in memory.
  octree.switchBuffers ();

  pcl::PointCloud<pcl::PointXYZ>::Ptr cloudB (new pcl::PointCloud<pcl::PointXYZ> );
  //Vishesh: Adding data to point cloud B
  if (pcl::io::loadPCDFile<pcl::PointXYZ> (argv[2], *cloudB) == -1) //* load the file
  {
    PCL_ERROR ("Couldn't read file  \n");
    return (-1);
  }
  //Vishesh Comment starts
  /*
  // Generate pointcloud data for cloudB
  cloudB->width = 128;
  cloudB->height = 1;
  cloudB->points.resize (cloudB->width * cloudB->height);

  for (size_t i = 0; i < cloudB->points.size (); ++i)
  {
    cloudB->points[i].x = 64.0f * rand () / (RAND_MAX + 1.0f);
    cloudB->points[i].y = 64.0f * rand () / (RAND_MAX + 1.0f);
    cloudB->points[i].z = 64.0f * rand () / (RAND_MAX + 1.0f);
  }
  */
  //Vishesh Comment ends
  // Add points from cloudB to octree
  octree.setInputCloud (cloudB);
  octree.addPointsFromInputCloud ();

  std::vector<int> newPointIdxVector;

  // Get vector of point indices from octree voxels which did not exist in previous buffer
  octree.getPointIndicesFromNewVoxels (newPointIdxVector);
  //pcl::PointCloud<pcl::PointXYZ>::Ptr missing_cloud (new pcl::PointCloud<pcl::PointXYZ> );
  /* Create a point cloud to store the different point values */
  pcl::PointCloud<pcl::PointXYZ> missing_cloud;
  missing_cloud.width = newPointIdxVector.size();
  missing_cloud.height = 1;
  missing_cloud.points.resize(missing_cloud.width*missing_cloud.height);
  // missing_cloud->width = newPointIdxVector.size();
  // missing_cloud->height = 1;
  // missing_cloud->points.resize(missing_cloud->width*missing_cloud->height);
  // Output points, givves us the points that are differnet in cloud B when compared to cloud A
  //std::cout << newPointIdxVector.size()<< std::endl;
  std::cout << "Inserting data into the final point cloud" << std::endl;
  /* Adding data to new pcd file */
  for (size_t i = 0; i < newPointIdxVector.size (); ++i){
    missing_cloud.points[i].x = cloudB->points[newPointIdxVector[i]].x;
    missing_cloud.points[i].y = cloudB->points[newPointIdxVector[i]].y;
    missing_cloud.points[i].z = cloudB->points[newPointIdxVector[i]].z;
    // std::cout << i << "# Index:" << newPointIdxVector[i]
    //           << "  Point:" << cloudB->points[newPointIdxVector[i]].x << " "
    //           << cloudB->points[newPointIdxVector[i]].y << " "
    //           << cloudB->points[newPointIdxVector[i]].z << std::endl;
  }
  /* Vishesh - Save the point cloud to the output file */
  pcl::io::savePCDFileASCII("output\\Difference.pcd",missing_cloud);
  std::cerr << "Saved " << missing_cloud.points.size() << " data points to newCloud.pcd" << std::endl;
  /* Vishesh - end */
  /* Vishesh - Triangulation of missing_cloud */
  // pcl::PointCloud<pcl::PointXYZ>::Ptr final_cloud (new pcl::PointCloud<pcl::PointXYZ> );
  // *final_cloud = missing_cloud;
  // // Normal estimation*
  // pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal> n;
  // pcl::PointCloud<pcl::Normal>::Ptr normals (new pcl::PointCloud<pcl::Normal>);
  // pcl::search::KdTree<pcl::PointXYZ>::Ptr tree (new pcl::search::KdTree<pcl::PointXYZ>);
  // tree->setInputCloud (final_cloud);
  // n.setInputCloud (final_cloud);
  // n.setSearchMethod (tree);
  // n.setKSearch (20);
  // n.compute (*normals);
  // //* normals should not contain the point normals + surface curvatures
  //
  // // Concatenate the XYZ and normal fields*
  // pcl::PointCloud<pcl::PointNormal>::Ptr cloud_with_normals (new pcl::PointCloud<pcl::PointNormal>);
  // pcl::concatenateFields (*final_cloud, *normals, *cloud_with_normals);
  // //* cloud_with_normals = cloud + normals
  //
  // // Create search tree*
  // pcl::search::KdTree<pcl::PointNormal>::Ptr tree2 (new pcl::search::KdTree<pcl::PointNormal>);
  // tree2->setInputCloud (cloud_with_normals);
  //
  // // Initialize objects
  // pcl::GreedyProjectionTriangulation<pcl::PointNormal> gp3;
  // pcl::PolygonMesh triangles;
  //
  // // Set the maximum distance between connected points (maximum edge length)
  // gp3.setSearchRadius (0.035);
  //
  // // Set typical values for the parameters
  // gp3.setMu (2.5);
  // gp3.setMaximumNearestNeighbors (100);
  // gp3.setMaximumSurfaceAngle(M_PI/4); // 45 degrees
  // gp3.setMinimumAngle(M_PI/18); // 10 degrees
  // gp3.setMaximumAngle(2*M_PI/3); // 120 degrees
  // gp3.setNormalConsistency(false);
  //
  // // Get result
  // gp3.setInputCloud (cloud_with_normals);
  // gp3.setSearchMethod (tree2);
  // gp3.reconstruct (triangles);
  //
  // // Additional vertex information
  // std::vector<int> parts = gp3.getPartIDs();
  // std::vector<int> states = gp3.getPointStates();
  // pcl::io::saveVTKFile ("D:\\EKTA\\Coalesce\\files\\missing_cloud.vtk", triangles);
  /* Vishesh - Trian ends */

  return 0;
}

#include <iostream>
#include <pcl/common/io.h>
#include <pcl/io/pcd_io.h>
#include <fstream>
#include <string>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <vector>
using namespace std;


int main (int argc, char** argv)
{
  if (argc != 3)
  {
    std::cerr << "Syntax for converting to pcd is: " << argv[0] << " <file_in.pcd> <file_out.pcd> "<< std::endl;
    return (-1);
  }

  ifstream file(argv[1]);
  if (!file.is_open()) {
	  cout << "Error opening file" << endl;
	  return 0;
  }
  string line;
  int count=0;
  while (!file.eof()) {
	  getline(file, line);
	  if (line[0] == '-' || isdigit(line[0])) {
		  count++;
	  }
  }
  file.clear();
  file.seekg(0, ios::beg);

  pcl::PointCloud<pcl::PointXYZ> cloud;
  cloud.width = count;
  cloud.height = 1;
  cloud.points.resize(cloud.width*cloud.height);

  int i = 0;
  while (!file.eof()) {
	  getline(file, line);
	  if (line[0] == '-' || isdigit(line[0])) {
		  vector<string> tokens;
		  boost::algorithm::split(tokens, line, boost::algorithm::is_any_of(" "));
		  cloud.points[i].x = stof(tokens[0]);
		  cloud.points[i].y = stof(tokens[1]);
		  cloud.points[i].z = stof(tokens[2]);
		  i++;
	  }
  }

  pcl::io::savePCDFileASCII(argv[2], cloud);
  return 0;

}
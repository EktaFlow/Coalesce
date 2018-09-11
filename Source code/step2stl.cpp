
// STEP Read Methods
#include "STEPControl_Reader.hxx"
#include <TopoDS_Shape.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <STEPControl_Writer.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <StlAPI_Writer.hxx>
#include <BRepMesh_IncrementalMesh.hxx>

int step2stl(char *in, char *out) {
  // Read from STEP
	//Creat a reader
  STEPControl_Reader reader;
  //Read the input STEP file using the created reader
  IFSelect_ReturnStatus stat = reader.ReadFile(in);
  Standard_Integer NbRoots = reader.NbRootsForTransfer();
  Standard_Integer NbTrans = reader.TransferRoots();
  TopoDS_Shape Original_Solid = reader.OneShape();

  // Before writing to STL we need to form a mesh
  StlAPI_Writer stlWriter = StlAPI_Writer();
  // Convert the TopoDS_Shape Object to Triangular Mesh
  BRepMesh_IncrementalMesh Mesh(Original_Solid, 0.001);
  Mesh.Perform();

  //Write to STL
  stlWriter.Write( Original_Solid, out);
  //cout << "last check " << endl;
  return 1;
}


Standard_Integer main (int argc, char *argv[]) {
	step2stl(argv[1], argv[2]);
	return 0;
}

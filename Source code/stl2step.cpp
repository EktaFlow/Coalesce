
// STEP Read Methods
#include "STEPControl_Reader.hxx"
#include <TopoDS_Shape.hxx>
// STL Read & Write Methods
#include <StlAPI_Writer.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <StlAPI_Reader.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Connect.hxx>
#include <Standard_NoSuchObject.hxx>

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <gp_Pnt.hxx>
#include <OSD_Path.hxx>
#include <RWStl.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <STEPControl_Writer.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>


Standard_Boolean StlAPI_Reader::Read(TopoDS_Shape&  theShape,
	const Standard_CString theFileName)
{
	Handle(Poly_Triangulation) aMesh = RWStl::ReadFile(theFileName);
	if (aMesh.IsNull())
	{
		return Standard_False;
	}

	TopoDS_Vertex aTriVertexes[3];
	TopoDS_Face aFace;
	TopoDS_Wire aWire;
	BRepBuilderAPI_Sewing aSewingTool;
	aSewingTool.Init(1.0e-30, Standard_True);

	TopoDS_Compound aComp;
	BRep_Builder BuildTool;
	BuildTool.MakeCompound(aComp);

	const TColgp_Array1OfPnt& aNodes = aMesh->Nodes();
	const Poly_Array1OfTriangle& aTriangles = aMesh->Triangles();
	for (Standard_Integer aTriIdx = aTriangles.Lower();
		aTriIdx <= aTriangles.Upper();
		++aTriIdx)
	{
		const Poly_Triangle& aTriangle = aTriangles(aTriIdx);

		Standard_Integer anId[3];
		aTriangle.Get(anId[0], anId[1], anId[2]);

		const gp_Pnt& aPnt1 = aNodes(anId[0]);
		const gp_Pnt& aPnt2 = aNodes(anId[1]);
		const gp_Pnt& aPnt3 = aNodes(anId[2]);
		if ((!(aPnt1.IsEqual(aPnt2, 0.0))) && (!(aPnt1.IsEqual(aPnt3, 0.0))))
		{
			aTriVertexes[0] = BRepBuilderAPI_MakeVertex(aPnt1);
			aTriVertexes[1] = BRepBuilderAPI_MakeVertex(aPnt2);
			aTriVertexes[2] = BRepBuilderAPI_MakeVertex(aPnt3);

			aWire = BRepBuilderAPI_MakePolygon(aTriVertexes[0], aTriVertexes[1], aTriVertexes[2], Standard_True);
			if (!aWire.IsNull())
			{
				aFace = BRepBuilderAPI_MakeFace(aWire);
				if (!aFace.IsNull())
				{
					BuildTool.Add(aComp, aFace);
				}
			}
		}
	}

	aSewingTool.Load(aComp);
	aSewingTool.Perform();
	theShape = aSewingTool.SewedShape();
	if (theShape.IsNull())
	{
		theShape = aComp;
	}
	return Standard_True;
}





Standard_Integer main (int argc, char *argv[]) {
	const Standard_CString filename = argv[1];
	StlAPI_Reader areader;
	TopoDS_Shape s;
	areader.Read(s, filename);
	STEPControl_Writer writer;
	writer.Transfer(s, STEPControl_ManifoldSolidBrep);
	writer.Write(argv[2]);
	return 0;
}

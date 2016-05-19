//
// rayUI.h
//
// The header file for the UI part
//

#ifndef __rayUI_h__
#define __rayUI_h__

// who the hell cares if my identifiers are longer than 255 characters:
#pragma warning(disable : 4786)

#include <string>

using std::string;

class RayTracer;

class TraceUI {
public:
	TraceUI() : m_nDepth(0), m_nSize(512), m_displayDebuggingInfo(false),
                    m_shadows(true), m_smoothshade(true), raytracer(0),
                    m_nFilterWidth(1), m_Blocksize(8), m_Threshold(0), m_Threads(8),
		    m_aaSampleWidth(3), m_aaThreshold(100), m_KdDepth(15), m_targetleafsize(10),
		    m_antialias(false), m_kdtree(true), m_usingCubeMap(false),
		    m_backfacecull(true), m_gotCubeMap(false)  
                    {}

	virtual int	run() = 0;

	// Send an alert to the user in some manner
	virtual void alert(const string& msg) = 0;

	// setters
	virtual void setRayTracer( RayTracer* r ) { raytracer = r; }
	void	setDepth         ( int i) { m_nDepth        = i;}
	void	setBlocksize     ( int i) { m_Blocksize     = i;}
	void	setThreshold     ( int i) { m_Threshold     = i;}
	void	setSize          ( int i) { m_nSize         = i;}
	void	setThread        ( int i) { m_Threads       = i;}
	void	setSampleWidth   ( int i) { m_aaSampleWidth = i;}
	void	setAaThreshold   ( int i) { m_aaThreshold   = i;}
	void	setKdDepth       ( int i) { m_KdDepth       = i;}
	void	setTargetLeafSize( int i) { m_targetleafsize= i;}
	void	setFilterWidth   ( int i) { m_nFilterWidth  = i;}
	
	void	setAntialias(bool b) { m_antialias = b; }
	void	setKdtree(bool b) { m_kdtree = b; }
	void	setCubeMap(bool b) { m_gotCubeMap = b; }
	void	setSmoothShade(bool b) { m_smoothshade = b; }
	void	setShadow(bool b) { m_shadows = b; }
	void	setBackfacecull(bool b) { m_backfacecull = b; }
	void	useCubeMap(bool b) { m_usingCubeMap = b; }

	// accessors:
	int	getDepth() const { return m_nDepth; }
	int	getBlocksize() const { return m_Blocksize;}
	int	getThreshold() const { return m_Threshold;}
	int	getSize() const { return m_nSize; }
	int	getThread() const { return m_Threads;}
	int	getSampleWidth() const { return m_aaSampleWidth;}
	int	getAaThreshold() const { return m_aaThreshold;}
	int	getKdDepth() const { return m_KdDepth;}
	int	getTargetLeafSize() const { return m_targetleafsize;}
	int	getFilterWidth() const { return m_nFilterWidth; }
	
	bool	Antialias() const { return m_antialias; }
	bool	Kdtree() const { return m_kdtree; }
	bool	UsingCubeMap() const { return m_usingCubeMap; }
	bool	smShadSw() const { return m_smoothshade; }
	bool	shadowSw() const { return m_shadows; }
	bool	Backfacecull() const { return m_backfacecull; }

	static bool m_debug;

protected:
	RayTracer*	raytracer;

	int	m_nDepth;	// Max depth of recursion
	int	m_Blocksize;
	int	m_Threshold;
	int	m_nSize;	// Size of the traced image
	int	m_Threads;
	int	m_aaSampleWidth;
	int	m_aaThreshold;
	int	m_KdDepth;
	int	m_targetleafsize;
	int	m_nFilterWidth;  // width of cubemap filter

	// Determines whether or not to show debugging information
	// for individual rays.  Disabled by default for efficiency
	// reasons.
	bool	m_antialias;  
	bool	m_kdtree; 
	bool	m_usingCubeMap;
	bool	m_smoothshade; 
	bool	m_shadows;
	bool	m_backfacecull;
	bool	m_gotCubeMap;  
	bool	m_displayDebuggingInfo;
};

#endif

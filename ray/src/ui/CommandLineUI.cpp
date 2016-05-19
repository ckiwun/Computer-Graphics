#include <iostream>
#include <time.h>
#include <stdarg.h>
#include <thread>

#include <assert.h>

#include "CommandLineUI.h"
#include "../fileio/bitmap.h"

#include "../RayTracer.h"

using namespace std;

// The command line UI simply parses out all the arguments off
// the command line and stores them locally.
CommandLineUI::CommandLineUI( int argc, char* const* argv )
	: TraceUI()
{
	int i;

	progName=argv[0];

	while( (i = getopt( argc, argv, "r:b:t:w:n:a:k:c:s:h:f:" )) != EOF)
	{
		switch( i )
		{
			case 'r':
				m_nDepth = atoi( optarg );
				break;

			case 'b':
				m_Blocksize = atoi( optarg );
				break;

			case 't':
				m_Threshold = atoi( optarg );
				break;

			case 'w':
				m_nSize = atoi( optarg );
				break;

			case 'n':
				m_Threads = atoi( optarg );
				break;

			case 'a':
				m_antialias = atoi( optarg );
				break;

			case 'k':
				m_kdtree = atoi( optarg );
				break;

			case 'c':
				m_usingCubeMap = atoi( optarg );
				break;

			case 's':
				m_smoothshade = atoi( optarg );
				break;

			case 'h':
				m_shadows = atoi( optarg );
				break;

			case 'f':
				m_backfacecull = atoi( optarg );
				break;


			default:
			// Oops; unknown argument
			std::cerr << "Invalid argument: '" << i << "'." << std::endl;
			usage();
			exit(1);
		}
	}

	if( optind >= argc-1 )
	{
		std::cerr << "no input and/or output name." << std::endl;
		exit(1);
	}

	rayName = argv[optind];
	imgName = argv[optind+1];
}

int CommandLineUI::run()
{
	assert( raytracer != 0 );
	raytracer->loadScene( rayName );

	if( raytracer->sceneLoaded() )
	{
		int width = m_nSize;
		int height = (int)(width / raytracer->aspectRatio() + 0.5);
		raytracer->traceSetup( width, height );

		clock_t start, end;
		start = clock();
		
		int thread_num = (int)getThread()-1;
		std::thread tt[thread_num];
		int blocksize = getBlocksize();
		for(int y = 0; y<height/blocksize;y++){
			for(int x = 0; x<width/blocksize;x++){
				int thread_id = x%(thread_num+1);
				if(x==width/blocksize-1)
					raytracer->blocktracePixel(x,y);
				else if(thread_id<thread_num)
					tt[thread_id] = std::thread(&RayTracer::blocktracePixel, raytracer, x, y);
				else
					raytracer->blocktracePixel(x,y);
				if(x%(thread_num+1)==thread_num||x==(width/blocksize-1))
				for(int i=0;i<thread_id;i++){
					tt[i].join();
				}
			}
		}
		if(width%blocksize!=0||height%blocksize!=0){
			for(int y = 0;y<height;y++)
			for(int x = 0;x<width;x++)
				if(x>=int(width/blocksize)*blocksize||y>=int(height/blocksize)*blocksize)
					raytracer->tracePixel(x,y);
		}
		if(Antialias()){
			for(int y = 0;y<height;y++){
				for(int x=0;x<width;x++){
					bool needaa = false;
					if(x<width-1&&y<height-1&&x>0&&y>0){
						double mag_mxmy = (raytracer->buffer[3*(x-1+(y-1)*width)]+raytracer->buffer[3*(x-1+(y-1)*width)+1]+raytracer->buffer[3*(x-1+(y-1)*width)+2])/255.0;
						double mag_pxmy = (raytracer->buffer[3*(x+1+(y-1)*width)]+raytracer->buffer[3*(x+1+(y-1)*width)+1]+raytracer->buffer[3*(x+1+(y-1)*width)+2])/255.0;
						double mag_mxpy = (raytracer->buffer[3*(x-1+(y+1)*width)]+raytracer->buffer[3*(x-1+(y+1)*width)+1]+raytracer->buffer[3*(x-1+(y+1)*width)+2])/255.0;
						double mag_pxpy = (raytracer->buffer[3*(x+1+(y+1)*width)]+raytracer->buffer[3*(x+1+(y+1)*width)+1]+raytracer->buffer[3*(x+1+(y+1)*width)+2])/255.0;
						double threshold = m_aaThreshold*0.001;
						if(fabs(mag_mxmy-mag_mxpy)>threshold) needaa = true;
						else if(fabs(mag_mxmy-mag_pxmy)>threshold) needaa = true;
						else if(fabs(mag_mxmy-mag_pxpy)>threshold) needaa = true;
						else if(fabs(mag_pxmy-mag_mxpy)>threshold) needaa = true;
						else if(fabs(mag_pxmy-mag_pxpy)>threshold) needaa = true;
						else if(fabs(mag_mxpy-mag_pxpy)>threshold) needaa = true;
					}
					int thread_id = x%(thread_num+1);
					if(x==width-1)
						raytracer->AntiAlias(x,y,needaa);
					else if(thread_id<thread_num)
						tt[thread_id] = std::thread(&RayTracer::AntiAlias, raytracer, x, y ,needaa);
					else
						raytracer->AntiAlias(x,y,needaa);
					if(x%(thread_num+1)==7||x==width-1)
					for(int i=0;i<thread_num;i++){
						tt[i].join();
					}
				}
			}
			std::memcpy(raytracer->buffer,raytracer->aabuffer,raytracer->buffer_width*raytracer->buffer_height*3);
		}

		end=clock();

		// save image
		unsigned char* buf;

		raytracer->getBuffer(buf, width, height);

		if (buf)
			writeBMP(imgName, width, height, buf);

		double t=(double)(end-start)/CLOCKS_PER_SEC;
//		int totalRays = TraceUI::resetCount();
//		std::cout << "total time = " << t << " seconds, rays traced = " << totalRays << std::endl;
		std::cout << "total time = " << t << " seconds" << std::endl;
        return 0;
	}
	else
	{
		std::cerr << "Unable to load ray file '" << rayName << "'" << std::endl;
		return( 1 );
	}
}

void CommandLineUI::alert( const string& msg )
{
	std::cerr << msg << std::endl;
}

void CommandLineUI::usage()
{
	std::cerr << "usage: " << progName << " [options] [input.ray output.bmp]" << std::endl;
	std::cerr << "  -r <#>      set recursion level (default " << m_nDepth << ")" << std::endl; 
	std::cerr << "  -b <#>      set block size (default " << m_Blocksize << ")" << std::endl; 
	std::cerr << "  -t <#>      set threshold (not support now) (default " << m_Threshold << ")" << std::endl; 
	std::cerr << "  -w <#>      set output image width (default " << m_nSize << ")" << std::endl;
	std::cerr << "  -n <#>      set threads num (default " << m_Threads << ")" << std::endl; 
	std::cerr << std::boolalpha << "  -a <0/1>(false/true)  enable antialias (default " << m_antialias << ")" << std::endl; 
	std::cerr << std::boolalpha << "  -k <0/1>(false/true)  enable kdtree (default " << m_kdtree << ")" << std::endl; 
	std::cerr << std::boolalpha << "  -c <0/1>(false/true)  enable antialias (default " << m_usingCubeMap << ")" << std::endl; 
	std::cerr << std::boolalpha << "  -s <0/1>(false/true)  enable antialias (default " << m_smoothshade << ")" << std::endl; 
	std::cerr << std::boolalpha << "  -h <0/1>(false/true)  enable antialias (default " << m_shadows << ")" << std::endl; 
	std::cerr << std::boolalpha << "  -f <0/1>(false/true)  enable antialias (default " << m_backfacecull << ")" << std::endl; 
}

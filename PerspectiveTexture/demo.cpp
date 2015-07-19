/*
Sources:
Mikael Kalms, "Perspective Texturemapping using linear interpolation of 1/Z, U/Z and V/Z", http://www.lysator.liu.se/~mikaelk/doc/perspectivetexture/.
ousttrue, "[2D][Triangle][C++]", http://d.hatena.ne.jp/ousttrue/20090218/1234974683, http://ousttrue.github.io/.
Paul Nettle, "High Speed Software Rendering", http://www.flipcode.com/archives/High_Speed_Software_Rendering.shtml.
"Netpbm format", https://en.wikipedia.org/wiki/Netpbm_format.
*/

#include <vector>
#include <fstream>
#include <iostream>
#include <string>	// getline() 

static char *screen;

//#include "tpoly.cpp"
#include "ptpoly1.cpp"
//#include "ptpoly2.cpp"


enum	{TEX_X = 256};			// Texture resolution
enum	{TEX_Y = 256};			//

// ----------------------------------------------------------------------------
// Draws a checkerboard texture into textureBuffer.
// ----------------------------------------------------------------------------

void	drawTexture(char *textureBuffer)
{
	// Frequency: the lower the number, the higher the frequency

	const int	freq = 4;	// 256/16
	const int	fAnd = 1 << freq;

	for (int y = 0; y < TEX_Y; y++)
	{
		int	yIndex = y * TEX_X;

		for (int x = 0; x < TEX_X; x++)
		{
			textureBuffer[yIndex+x] = (y&fAnd) == (x&fAnd) ? 255:1;
		}
	}
}

//////////////////////////////////////////
// 
void loadTexture(char *texture)
{
  std::ifstream io("check16x16.pgm", std::ios::binary);
  std::string line;

  for(int i=0; i<3; ){
#if	__WATCOMC__
	std::streambuf *pbuf;
	pbuf = io.rdbuf();	// 
	line.clear();
    do {
		char ch = pbuf->sgetc();
		if (ch!='\n') line += ch;
    } while ( pbuf->snextc() != '\n' );	

	if(line[0]!='#'){
      std::cout << line.c_str() << std::endl;
      ++i;
    }
#else
    std::getline(io, line);
    if(line[0]!='#'){
      std::cout << line << std::endl;
      ++i;
    }
#endif	
  }

  io.read(texture, 256 * 256);

  std::cout << "load texture" << std::endl;
}

int main(int argc, char **argv)
{
  // screen 320 x 320
  std::vector<char> Screen(320 * 320, 0x00);
  screen=&Screen[0];

  // texture 256 x 256
  std::vector<char> Texture(256 * 256, 0x00);

  if (argc>1)
  {  
    // make a 256x256 bytes texture data 
    drawTexture(&Texture[0]);
    // write down 
    std::ofstream tf("check16x16.pgm", std::ios::binary);
    tf << "P5\n256 256\n255\n";
    tf.write(&Texture[0], Texture.size());  
  }
  else
  {
    loadTexture(&Texture[0]);  
  }

  TPolytri poly={
    20, 20, 30,
    20, 280, 30,
    280, 280, 30,
    0, 0,
    0, 256,
    256, 256,
    &Texture[0]
  };
  TPolytri poly2={
    20, 20, 30,
    280, 20, 30,
    280, 280, 30,
    0, 0,
    256, 0,
    256, 256,
    &Texture[0]
  };  
//  drawtpolysubtri(&poly);
//  drawtpolyperspsubtri(&poly);
  drawtpolyperspsubtri(&poly2);  
//  drawtpolyperspdivsubtri(&poly);  

  std::ofstream io("tmp.pgm", std::ios::binary);
  io << "P5\n320 320\n255\n";
  io.write(&Screen[0], Screen.size());
}

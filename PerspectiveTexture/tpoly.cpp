// Texturemapper with subpixels and subtexels, uses floats all the way
//  through

struct TPolytri
{
	float x1, y1, z1;
	float x2, y2, z2;
	float x3, y3, z3;
	float u1, v1;
	float u2, v2;
	float u3, v3;
	char *texture;
};

static float dudx, dvdx, dudy, dvdy;
static float xa, xb, ua, va;
static float dxdya, dxdyb, dudya, dvdya;
static char *texture;

static void drawtpolysubtriseg(int y1, int y2);

void drawtpolysubtri(TPolytri *poly)
{
	float x1, y1, x2, y2, x3, y3;
	float u1, v1, u2, v2, u3, v3;
	float dxdy1, dxdy2, dxdy3;
	float tempf;
	float denom;
	float dy;
	int y1i, y2i, y3i;
	int side;

	// Shift XY coordinate system (+0.5, +0.5) to match the subpixeling
	//  technique

	x1 = poly->x1 + 0.5;
	y1 = poly->y1 + 0.5;
	x2 = poly->x2 + 0.5;
	y2 = poly->y2 + 0.5;
	x3 = poly->x3 + 0.5;
	y3 = poly->y3 + 0.5;

	u1 = poly->u1;
	v1 = poly->v1;
	u2 = poly->u2;
	v2 = poly->v2;
	u3 = poly->u3;
	v3 = poly->v3;

	texture = poly->texture;

	// Sort the vertices in ascending Y order

#define swapfloat(x, y) tempf = x; x = y; y = tempf;
	if (y1 > y2)
	{
		swapfloat(x1, x2);
		swapfloat(y1, y2);
		swapfloat(u1, u2);
		swapfloat(v1, v2);
	}
	if (y1 > y3)
	{
		swapfloat(x1, x3);
		swapfloat(y1, y3);
		swapfloat(u1, u3);
		swapfloat(v1, v3);
	}
	if (y2 > y3)
	{
		swapfloat(x2, x3);
		swapfloat(y2, y3);
		swapfloat(u2, u3);
		swapfloat(v2, v3);
	}
#undef swapfloat

	y1i = y1;
	y2i = y2;
	y3i = y3;

	// Skip poly if it's too thin to cover any pixels at all

	if ((y1i == y2i && y1i == y3i)
	    || ((int) x1 == (int) x2 && (int) x1 == (int) x3))
		return;

	// Calculate horizontal and vertical increments for UV axes (these
	//  calcs are certainly not optimal, although they're stable
	//  (handles any dy being 0)

	denom = ((x3 - x1) * (y2 - y1) - (x2 - x1) * (y3 - y1));

	if (!denom)		// Skip poly if it's an infinitely thin line
		return;	

	denom = 1 / denom;	// Reciprocal for speeding up
	dudx = ((u3 - u1) * (y2 - y1) - (u2 - u1) * (y3 - y1)) * denom;
	dvdx = ((v3 - v1) * (y2 - y1) - (v2 - v1) * (y3 - y1)) * denom;
	dudy = ((u2 - u1) * (x3 - x1) - (u3 - u1) * (x2 - x1)) * denom;
	dvdy = ((v2 - v1) * (x3 - x1) - (v3 - v1) * (x2 - x1)) * denom;

	// Calculate X-slopes along the edges

	if (y2 > y1)
		dxdy1 = (x2 - x1) / (y2 - y1);
	if (y3 > y1)
		dxdy2 = (x3 - x1) / (y3 - y1);
	if (y3 > y2)
		dxdy3 = (x3 - x2) / (y3 - y2);

	// Determine which side of the poly the longer edge is on

	side = dxdy2 > dxdy1;

	if (y1 == y2)
		side = x1 > x2;
	if (y2 == y3)
		side = x3 > x2;

	if (!side)	// Longer edge is on the left side
	{
		// Calculate slopes along left edge

		dxdya = dxdy2;
		dudya = dxdya * dudx + dudy;
		dvdya = dxdya * dvdx + dvdy;

		// Perform subpixel pre-stepping along left edge

		dy = 1 - (y1 - y1i);
		xa = x1 + dy * dxdya;
		ua = u1 + dy * dudya;
		va = v1 + dy * dvdya;

		if (y1i < y2i)	// Draw upper segment if possibly visible
		{
			// Set right edge X-slope and perform subpixel pre-
			//  stepping

			dxdyb = dxdy1;
			xb = x1 + dy * dxdyb;

			drawtpolysubtriseg(y1i, y2i);
		}
		if (y2i < y3i)	// Draw lower segment if possibly visible
		{
			// Set right edge X-slope and perform subpixel pre-
			//  stepping

			dxdyb = dxdy3;
			xb = x2 + (1 - (y2 - y2i)) * dxdyb;

			drawtpolysubtriseg(y2i, y3i);
		}
	}
	else	// Longer edge is on the right side
	{
		// Set right edge X-slope and perform subpixel pre-stepping

		dxdyb = dxdy2;
		dy = 1 - (y1 - y1i);
		xb = x1 + dy * dxdyb;

		if (y1i < y2i)	// Draw upper segment if possibly visible
		{
			// Set slopes along left edge and perform subpixel
			//  pre-stepping

			dxdya = dxdy1;
			dudya = dxdya * dudx + dudy;
			dvdya = dxdya * dvdx + dvdy;
			xa = x1 + dy * dxdya;
			ua = u1 + dy * dudya;
			va = v1 + dy * dvdya;

			drawtpolysubtriseg(y1i, y2i);
		}
		if (y2i < y3i)	// Draw lower segment if possibly visible
		{
			// Set slopes along left edge and perform subpixel
			//  pre-stepping

			dxdya = dxdy3;
			dudya = dxdya * dudx + dudy;
			dvdya = dxdya * dvdx + dvdy;
			dy = 1 - (y2 - y2i);
			xa = x2 + dy * dxdya;
			ua = u2 + dy * dudya;
			va = v2 + dy * dvdya;

			drawtpolysubtriseg(y2i, y3i);
		}
	}
}

static void drawtpolysubtriseg(int y1, int y2)
{
	char *scr;
	int x1, x2;
	float u, v, dx;

	while (y1 < y2)		// Loop through all lines in the segment
	{
		x1 = xa;
		x2 = xb;

		// Perform subtexel pre-stepping on UV

		dx = 1 - (xa - x1);
		u = ua + dx * dudx;
		v = va + dx * dvdx;

		scr = &screen[y1 * 320 + x1];

		while (x1++ < x2)	// Draw horizontal line
		{
			// Copy pixel from texture to screen

			*(scr++) = texture[((((int) v) & 0xff) << 8) + (((int) u) & 0xff)];

			// Step UV horizontally

			u += dudx;
			v += dvdx;
		}

		// Step along both edges

		xa += dxdya;
		xb += dxdyb;
		ua += dudya;
		va += dvdya;

		y1++;
	}
}

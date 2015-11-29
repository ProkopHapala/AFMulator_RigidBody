
#ifndef _QUATERNION_
#define _QUATERNION_

#include <math.h>
#define TRACKBALLSIZE (0.8f)

float project_beam_to_sphere( float r, float x, float y ){
// project a beam with plane coordinates x and y onto a sphere of radius r
// if the camera is far away from the centre of the sphere use a hyberbolic sheet instead of the sphere

	float d, t, z;

	// distance of a beam origin from the origin of the sphere
	d = sqrt( x*x + y*y );

	// if d < r/sqrt(2), then we are inside a sphere
	// otherwise we use hyperbola instead
	if ( d < r * 0.70710678118654752440 ) {
		// inside a sphere
	
		// set z to the length of the beam from its origin to its intersection with a sphere
		z = sqrt(r*r - d*d);
	} else {
		// on a hyperbola
		t = r * 0.70710678118654752440; // == 1/sqrt(2)
		z = t*t / d;
	}

	return z;
}


// read also:
// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToAngle/

//template <class TYPE, class VEC, class MAT, class QUAT> 
//template <class TYPE, class VEC> 
template <class TYPE> 
class Quat4TYPE {
	using VEC  = Vec3TYPE<TYPE>; 
	using MAT  = Mat3TYPE<TYPE>; 
	using QUAT = Quat4TYPE<TYPE>; 
	public:
	union{
		struct{ TYPE x,y,z,w; };
		TYPE array[4];
	};

	inline void set   ( const  QUAT& q                     ){ x=q.x; y=q.y; z=q.z; w=q.w; };
	inline void set   ( TYPE fx, TYPE fy, TYPE fz, TYPE fw ){ x=fx;  y=fy;  z=fz;  w=fw;  };
	inline void setOne( ){ x=y=z=0; w=1; };

// ====== basic aritmetic

// ============== Basic Math

    inline void mul        ( TYPE f                               ){ x*=f;            y*=f;           z*=f;           w*=f;            };
    inline void add        ( const QUAT& v                        ){ x+=v.x;          y+=v.y;         z+=v.z;         w+=v.w;          };
    inline void sub        ( const QUAT& v                        ){ x-=v.x;          y-=v.y;         z-=v.z;         w-=v.w;          };
	inline void set_add    ( const QUAT& a, const QUAT& b         ){ x =a.x+b.x;      y =a.y+b.y;     z =a.z+b.z;     w =a.w+b.w;      };
	inline void set_sub    ( const QUAT& a, const QUAT& b         ){ x =a.x-b.x;      y =a.y-b.y;     z =a.z-b.z;     w =a.w-b.w;      };
	inline void add_mul    ( const QUAT& a, TYPE f                ){ x+=a.x*f;        y+=a.y*f;       z+=a.z*f;       w+=a.w*f;        };
	inline void set_add_mul( const QUAT& a, const QUAT& b, TYPE f ){ x =a.x + f*b.x;  y =a.y + f*b.y; z =a.z + f*b.z; w =a.w + f*b.w;  };

    inline TYPE dot  ( QUAT q   ) {  return       w*q.w + x*q.x + y*q.y + z*q.z;   }
    inline TYPE norm2(          ) {  return       w*  w + x*  x + y*  y + z*  z;   }
	inline TYPE norm (          ) {  return sqrt( w*  w + x*  x + y*  y + z*  z ); }
    inline TYPE normalize() {
		TYPE norm  = sqrt( x*x + y*y + z*z + w*w );
		TYPE inorm = 1.0d/norm;
		x *= inorm;    y *= inorm;    z *= inorm;   w *= inorm;
		return norm;
    }

// ====== Trackball

	void trackballQ( float p1x, float p1y, float p2x, float p2y ){
	// project the points onto a virtual trackball and return a quaternion q corresponding to
	// the appropriate rotation of the trackball
	// actually the trackball is deformed, nearby its centre it is taken as a ball,
	// whereas far from the centre it is taken as a hyperbolic sheet
	// the centre of the ball is {0,0,0}; arguments are assumed to lie in the range (-1.0, .., 1.0)

		Vec3f axis; // axis of rotation
		float phi;  // angle of rotation
		Vec3f p1, p2, d;
		float t;
	
		// NOTE: quaternion {0,0,0,1} corresponds to the rotation matrix which is equal to the identity
	
		// if a new position of the mouse is identical to the old one, return the identity
		if( p1x == p2x && p1y == p2y ){
			setOne();
			return;
		}
	
		// calcutate the intersection of the 'mouse pointer' and the deformed sphere
		// to p1 set coordinates of the intersection corresponding to the old mouse position
		// to p2 set coordinates of the intersection corresponding to the new mouse position
		// prior to this setting calculate z-coordinates of the intersections with help of tb_project_to_sphere
		p1.set( p1x, p1y, project_beam_to_sphere( TRACKBALLSIZE, p1x, p1y ) );
		p2.set( p2x, p2y, project_beam_to_sphere( TRACKBALLSIZE, p2x, p2y ) );
	
		// axis of rotation is calculated by a cross product of p1 and p2
		axis.set_cross( p2, p1 );
	
		// prepare for the rotation-angle calculation
		d.set_sub( p1, p2 );
		t = d.norm() / ( 2.0 * TRACKBALLSIZE );

		// avoid problems with out-of-control values
		if( t > 1.0 )  t =  1.0;
		if( t < -1.0 ) t = -1.0;
	
		// angle of rotation
		phi = 2.0 * asin( t );
	
		// calculate the quaternion
		fromAngleAxis( phi, axis );

	}	
	

// ====== Quaternion multiplication

	inline void setQmul( const QUAT& a, const QUAT& b) {
        x =  a.x * b.w + a.y * b.z - a.z * b.y + a.w * b.x;
        y = -a.x * b.z + a.y * b.w + a.z * b.x + a.w * b.y;
        z =  a.x * b.y - a.y * b.x + a.z * b.w + a.w * b.z;
        w = -a.x * b.x - a.y * b.y - a.z * b.z + a.w * b.w;
    };

    inline void qmul( const QUAT& a) {
        TYPE aw = a.w, ax = a.x, ay = a.y, az = a.z;
        TYPE x_ =  x * aw + y * az - z * ay + w * ax;
        TYPE y_ = -x * az + y * aw + z * ax + w * ay;
        TYPE z_ =  x * ay - y * ax + z * aw + w * az;
                w = -x * ax - y * ay - z * az + w * aw;
        x = x_; y = y_; z = z_;
    };

    inline void invertUnitary() { x=-x; y=-y; z=-z; }

    inline void invert() {
		TYPE norm = sqrt( x*x + y*y + z*z + w*w );
		if ( norm > 0.0 ) {
			TYPE invNorm = 1.0d / norm;
			x *= -invNorm; y *= -invNorm;z *= -invNorm;	w *=  invNorm;
		}
    };


// ======= Conversion : Angle & Axis

	void fromAngleAxis( TYPE angle, const VEC& axis ){  

		TYPE ir   = double( 1 )/axis.norm();
		VEC  hat  = axis * ir;
		TYPE a    = 0.5d * angle;
		TYPE sa   = sin(a);
		w =         cos(a);
		x = sa * hat.x;
		y = sa * hat.y;
		z = sa * hat.z;
		
//		printf( "ha: %f %f %f %f %f \n", angle, x, a, sa, hat.x );
	};

	void fromAngleAxisNoGonio( TYPE scal_prod, const VEC& axis ){
	// we assume -phi instead of phi!!!, minus effectively implies sa -> -sa
	
		TYPE cosphi, sinphi, sa, phi, sgn_sinphi;
		TYPE ir   = 1.0 / axis.norm();
		VEC  hat  = axis * ir;

		cosphi = scal_prod;				
		sgn_sinphi = 1.0; // ?
		
		if( cosphi >  1 - 1e-6 ){
			sa = 0; w = 1;
		} else if( cosphi < -1 + 1e-6 ){
			sa = -1; w = 0;
		} else {
			sa = + sqrt( (1 - cosphi) / 2.0 );
			w  = - sqrt( (1 + cosphi) / 2.0 ) * sgn_sinphi;
//			sa = -sa; w = -w;
		}
		
		x = sa * hat.x;
		y = sa * hat.y;
		z = sa * hat.z;

	}


// ====== Differential rotation

	inline void dRot_exact ( TYPE dt, const VEC& omega ) {
		TYPE hx   = omega.x;
		TYPE hy   = omega.y;
		TYPE hz   = omega.z;
		TYPE r2   = hx*hx + hy*hy + hz*hz;
		if(r2>0){
			TYPE norm = sqrt( r2 );
			TYPE a    = dt * norm * 0.5d;
			TYPE sa   = sin( a )/norm;  // we normalize it here to save multiplications
			TYPE ca   = cos( a );
			hx*=sa; hy*=sa; hz*=sa;            // hat * sin(a)
			TYPE x_ = x, y_ = y, z_ = z, w_ = w;
			x =  hx*w_ + hy*z_ - hz*y_ + ca*x_;
			y = -hx*z_ + hy*w_ + hz*x_ + ca*y_;
			z =  hx*y_ - hy*x_ + hz*w_ + ca*z_;
			w = -hx*x_ - hy*y_ - hz*z_ + ca*w_;
		}
	};


	inline void dRot_taylor2 ( TYPE dt, VEC& omega ) {
		TYPE hx   = omega.x;
		TYPE hy   = omega.y;
		TYPE hz   = omega.z;
		TYPE r2   = hx*hx + hy*hy + hz*hz;
		TYPE b2   = dt*dt*r2;
		const TYPE c2 = 1.0d/8;    // 4  *2       
		const TYPE c3 = 1.0d/48;   // 8  *2*3     
		const TYPE c4 = 1.0d/384;  // 16 *2*3*4   
		const TYPE c5 = 1.0d/3840; // 32 *2*3*4*5 
		TYPE sa   = dt * ( 0.5d - b2*( c3 - c5*b2 ) ); 
		TYPE ca   =      ( 1    - b2*( c2 - c4*b2 ) );
		hx*=sa; hy*=sa; hz*=sa;  // hat * sin(a)
		TYPE x_ = x, y_ = y, z_ = z, w_ = w;
		x =  hx*w_ + hy*z_ - hz*y_ + ca*x_;
		y = -hx*z_ + hy*w_ + hz*x_ + ca*y_;
		z =  hx*y_ - hy*x_ + hz*w_ + ca*z_;
		w = -hx*x_ - hy*y_ - hz*z_ + ca*w_;
	};


	inline void toMatrix( MAT& result) const {
		    TYPE r2 = w*w + x*x + y*y + z*z;
		    //TYPE s  = (r2 > 0) ? 2d / r2 : 0;
			TYPE s  = 2 / r2;
		    // compute xs/ys/zs first to save 6 multiplications, since xs/ys/zs
		    // will be used 2-4 times each.
		    TYPE xs = x * s;  TYPE ys = y * s;  TYPE zs = z * s;
		    TYPE xx = x * xs; TYPE xy = x * ys; TYPE xz = x * zs;
		    TYPE xw = w * xs; TYPE yy = y * ys; TYPE yz = y * zs;
		    TYPE yw = w * ys; TYPE zz = z * zs; TYPE zw = w * zs;
		    // using s=2/norm (instead of 1/norm) saves 9 multiplications by 2 here
		    result.xx = 1 - (yy + zz);
		    result.xy =     (xy - zw);
		    result.xz =     (xz + yw);
		    result.yx =     (xy + zw);
		    result.yy = 1 - (xx + zz);
		    result.yz =     (yz - xw);
		    result.zx =     (xz - yw);
		    result.zy =     (yz + xw);
		    result.zz = 1 - (xx + yy);
	};


	inline void toMatrix_unitary( MAT& result)  const  {
		TYPE xx = x * x;
		TYPE xy = x * y;
		TYPE xz = x * z;
		TYPE xw = x * w;
		TYPE yy = y * y;
		TYPE yz = y * z;
		TYPE yw = y * w;
		TYPE zz = z * z;
		TYPE zw = z * w;
		result.xx = 1 - 2 * ( yy + zz );
		result.xy =     2 * ( xy - zw );
		result.xz =     2 * ( xz + yw );
		result.yx =     2 * ( xy + zw );
		result.yy = 1 - 2 * ( xx + zz );
		result.yz =     2 * ( yz - xw );
		result.zx =     2 * ( xz - yw );
		result.zy =     2 * ( yz + xw );
		result.zz = 1 - 2 * ( xx + yy );
	};

	void build_rotmatrix( float m[4][4] ){
	// from a quaternion construct the corresponding rotation matrix

		TYPE xx = x * x;
		TYPE xy = x * y;
		TYPE xz = x * z;
		TYPE xw = x * w;
		TYPE yy = y * y;
		TYPE yz = y * z;
		TYPE yw = y * w;
		TYPE zz = z * z;
		TYPE zw = z * w;
		
		m[0][0] = 1 - 2 * ( yy + zz );
		m[0][1] =     2 * ( xy - zw );
		m[0][2] =     2 * ( xz + yw );
		m[0][3] = 0.0;
				
		m[1][0] =     2 * ( xy + zw );
		m[1][1] = 1 - 2 * ( xx + zz );
		m[1][2] =     2 * ( yz - xw );
		m[1][3] = 0.0;
				
		m[2][0] =     2 * ( xz - yw );
		m[2][1] =     2 * ( yz + xw );
		m[2][2] = 1 - 2 * ( xx + yy );
		m[2][3] = 0.0;
		    
		m[3][0] = 0.0;
		m[3][1] = 0.0;
		m[3][2] = 0.0;
		m[3][3] = 1.0;

	}


	inline void toMatrix_unitary2( MAT& result)  const  {
		TYPE x2 = 2*x;
		TYPE y2 = 2*y;
		TYPE z2 = 2*z;
		TYPE xx = x2 * x;
		TYPE xy = x2 * y;
		TYPE xz = x2 * z;
		TYPE xw = x2 * w;
		TYPE yy = y2 * y;
		TYPE yz = y2 * z;
		TYPE yw = y2 * w;
		TYPE zz = z2 * z;
		TYPE zw = z2 * w;
		result.xx = 1 - ( yy + zz );
		result.xy =     ( xy - zw );
		result.xz =     ( xz + yw );
		result.yx =     ( xy + zw );
		result.yy = 1 - ( xx + zz );
		result.yz =     ( yz - xw );
		result.zx =     ( xz - yw );
		result.zy =     ( yz + xw );
		result.zz = 1 - ( xx + yy );
	};

	// this will compute force on quaternion from force on some point in coordinate system of the quaternion
	inline void addForceFromPoint( const VEC& p, const VEC& fp, QUAT& fq ) const {
		// dE/dx = dE/dpx * dpx/dx
		// dE/dx = fx
		TYPE px_x =    p.b*y +             p.c*z;
		TYPE py_x =    p.a*y - 2*p.b*x +   p.c*w;
		TYPE pz_x =    p.a*z -   p.b*w - 2*p.c*x;
		fq.x += 2*( fp.x * px_x  +  fp.y * py_x  + fp.z * pz_x );

		TYPE px_y = -2*p.a*y +   p.b*x -   p.c*w;
		TYPE py_y =    p.a*x +             p.c*z;
		TYPE pz_y =    p.a*w +   p.b*z - 2*p.c*y;
		fq.y += 2*( fp.x * px_y  +  fp.y * py_y  + fp.z * pz_y );

		TYPE px_z = -2*p.a*z +   p.b*w +   p.c*x;
		TYPE py_z =   -p.a*w - 2*p.b*z +   p.c*y;
		TYPE pz_z =    p.a*x +   p.b*y;
		fq.z += 2*( fp.x * px_z  +  fp.y * py_z  + fp.z * pz_z );

		TYPE px_w =    p.b*z -   p.c*y;
		TYPE py_w =   -p.a*z +   p.c*x;
		TYPE pz_w =    p.a*y -   p.b*x;
		fq.w += 2*( fp.x * px_w  +  fp.y * py_w  + fp.z * pz_w );
		
	}


	inline void fromMatrix( const VEC& a, const VEC& b, const VEC& c ) { fromMatrix( a.x,  a.y,  a.z,  b.x,  b.y,  b.z,  c.x,  c.y,  c.z  );  }
	inline void fromMatrix( const MAT& M                             ) { fromMatrix( M.ax, M.ay, M.az, M.bx, M.by, M.bz, M.cx, M.cy, M.cz );  }
	inline void fromMatrix( TYPE m00, TYPE m01, TYPE m02,    TYPE m10, TYPE m11, TYPE m12,        TYPE m20, TYPE m21, TYPE m22) {
        // Use the Graphics Gems code, from 
        // ftp://ftp.cis.upenn.edu/pub/graphics/shoemake/quatut.ps.Z
        TYPE t = m00 + m11 + m22;
        // we protect the division by s by ensuring that s>=1
        if (t >= 0) { // by w
            TYPE s = sqrt(t + 1);
            w = 0.5 * s;
            s = 0.5 / s;                 
            x = (m21 - m12) * s;
            y = (m02 - m20) * s;
            z = (m10 - m01) * s;
        } else if ((m00 > m11) && (m00 > m22)) { // by x
            TYPE s = sqrt(1 + m00 - m11 - m22); 
            x = s * 0.5; 
            s = 0.5 / s;
            y = (m10 + m01) * s;
            z = (m02 + m20) * s;
            w = (m21 - m12) * s;
        } else if (m11 > m22) { // by y
            TYPE s = sqrt(1 + m11 - m00 - m22); 
            y = s * 0.5; 
            s = 0.5 / s;
            x = (m10 + m01) * s;
            z = (m21 + m12) * s;
            w = (m02 - m20) * s;
        } else { // by z
            TYPE s = sqrt(1 + m22 - m00 - m11); 
            z = s * 0.5; 
            s = 0.5 / s;
            x = (m02 + m20) * s;
            y = (m21 + m12) * s;
            w = (m10 - m01) * s;
        }
	}


};

/*
class Quat4i : public Quat4TYPE< int,    Vec3i, Mat3i, Quat4i >{  };
class Quat4f : public Quat4TYPE< float,  Vec3f, Mat3f, Quat4f >{  };
class QUAT : public Quat4TYPE< TYPE, VEC, MAT, QUAT >{  };
*/

using Quat4i = Quat4TYPE< int>;
using Quat4f = Quat4TYPE< float>;
using Quat4d = Quat4TYPE< double >;

#endif


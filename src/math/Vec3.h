
#ifndef _VEC_3_
#define _VEC_3_

//template <class TYPE,class VEC> 
template <class TYPE> 
class Vec3TYPE{
	using VEC = Vec3TYPE<TYPE>;
	public:
	union{
		struct{ TYPE x,y,z; };
		struct{ TYPE a,b,c; };
		TYPE array[3];
	};

	// ===== methods
	inline void set( TYPE f                    ) { x=f;   y=f;   z=f;   };
    	inline void set( TYPE fx, TYPE fy, TYPE fz ) { x=fx;  y=fy;  z=fz;  };
    	inline void set( const VEC& v              ) { x=v.x; y=v.y; z=v.z; };
	inline void set( TYPE* arr                 ) { x=arr[0]; y=arr[1]; z=arr[2]; };

    	inline void get( TYPE& fx, TYPE& fy, TYPE& fz ) { fx=x;  fy=y;  fz=z;           };
	inline void get( TYPE* arr                    ) { arr[0]=x; arr[1]=y; arr[2]=z; };

	inline void add( TYPE f ) { x+=f; y+=f; z+=f; };
	inline void mul( TYPE f ) { x*=f; y*=f; z*=f; };

	inline void add( const VEC&  v ) { x+=v.x; y+=v.y; z+=v.z; };
    inline void sub( const VEC&  v ) { x-=v.x; y-=v.y; z-=v.z; };
    inline void mul( const VEC&  v ) { x*=v.x; y*=v.y; z*=v.z; };
    inline void div( const VEC&  v ) { x/=v.x; y/=v.y; z/=v.z; };

    inline void add( TYPE fx, TYPE fy, TYPE fz ) { x+=fx; y+=fy; z+=fz; };
    inline void sub( TYPE fx, TYPE fy, TYPE fz ) { x-=fx; y-=fy; z-=fz; };
    inline void mul( TYPE fx, TYPE fy, TYPE fz ) { x*=fx; y*=fy; z*=fz; };
    inline void div( TYPE fx, TYPE fy, TYPE fz ) { x/=fx; y/=fy; z/=fz; };

	inline void set_add( const VEC& a, TYPE f ){ x=a.x+f; y=a.y+f; z=a.z+f; };
	inline void set_mul( const VEC& a, TYPE f ){ x=a.x*f; y=a.y*f; z=a.z*f; };
	inline void set_mul( const VEC& a, const VEC& b, TYPE f ){ x=a.x*b.x*f; y=a.y*b.y*f; z=a.z*b.z*f; };

	inline void set_add( const VEC& a, const VEC& b ){ x=a.x+b.x; y=a.y+b.y; z=a.z+b.z; };
	inline void set_sub( const VEC& a, const VEC& b ){ x=a.x-b.x; y=a.y-b.y; z=a.z-b.z; };
	inline void set_mul( const VEC& a, const VEC& b ){ x=a.x*b.x; y=a.y*b.y; z=a.z*b.z; };
	inline void set_div( const VEC& a, const VEC& b ){ x=a.x/b.x; y=a.y/b.y; z=a.z/b.z; };

	inline void add_mul( const VEC& a, TYPE f                ){ x+=a.x*f;     y+=a.y*f;     z+=a.z*f;   };
	inline void add_mul( const VEC& a, const VEC& b          ){ x+=a.x*b.x;   y+=a.y*b.y;   z+=a.z*b.z; };
	inline void sub_mul( const VEC& a, const VEC& b          ){ x-=a.x*b.x;   y-=a.y*b.y;   z-=a.z*b.z; };
	inline void sub_mul( TYPE f, const VEC& b          ){ x-=f*b.x;   y-=f*b.y;   z-=f*b.z; };
	inline void add_mul( const VEC& a, const VEC& b, TYPE f  ){ x+=a.x*b.x*f; y+=a.y*b.y*f; z+=a.z*b.z*f;   };

	inline void set_add_mul( const VEC& a, const VEC& b, TYPE f ){ x= a.x + f*b.x;     y= a.y + f*b.y;     z= a.z + f*b.z;  };

	inline void set_lincomb( TYPE fa, const VEC& a, TYPE fb, const VEC& b ){ x = fa*a.x + fb*b.x;  y = fa*a.y + fb*b.y;  z = fa*a.z + fb*b.z; };
	inline void add_lincomb( TYPE fa, const VEC& a, TYPE fb, const VEC& b ){ x+= fa*a.x + fb*b.x;  y+= fa*a.y + fb*b.y;  z+= fa*a.z + fb*b.z; };

	inline void set_lincomb( TYPE fa, TYPE fb, TYPE fc, const VEC& a, const VEC& b, const VEC& c ){ x = fa*a.x + fb*b.x + fc*c.x;  y = fa*a.y + fb*b.y + fc*c.y;  z = fa*a.z + fb*b.z + fc*c.z; };
	inline void add_lincomb( TYPE fa, TYPE fb, TYPE fc, const VEC& a, const VEC& b, const VEC& c ){ x+= fa*a.x + fb*b.x + fc*c.x;  y+= fa*a.y + fb*b.y + fc*c.y;  z+= fa*a.z + fb*b.z + fc*c.z; };


	inline void  set_cross( const VEC& a, const VEC& b ){
		x = a.y * b.z - a.z * b.y;
		y = a.z * b.x - a.x * b.z; 
		z = a.x * b.y - a.y * b.x;
	};
	
	inline void  add_cross( const VEC& a, const VEC& b ){ x+=a.y*b.z-a.z*b.y; y+=a.z*b.x-a.x*b.z; z+=a.x*b.y-a.y*b.x; };

    inline VEC operator+ ( TYPE f   ) const { VEC vo; vo.x=x+f; vo.y=y+f; vo.z=z+f; return vo; };
    inline VEC operator* ( TYPE f   ) const { VEC vo; vo.x=x*f; vo.y=y*f; vo.z=z*f; return vo; };

    inline VEC operator+ ( const VEC& vi ) const { VEC vo; vo.x=x+vi.x; vo.y=y+vi.y; vo.z=z+vi.z; return vo; };
    inline VEC operator- ( const VEC& vi ) const { VEC vo; vo.x=x-vi.x; vo.y=y-vi.y; vo.z=z-vi.z; return vo; };
    inline VEC operator* ( const VEC& vi ) const { VEC vo; vo.x=x*vi.x; vo.y=y*vi.y; vo.z=z*vi.z; return vo; };
    inline VEC operator/ ( const VEC& vi ) const { VEC vo; vo.x=x/vi.x; vo.y=y/vi.y; vo.z=z/vi.z; return vo; };

	inline TYPE dot  ( const VEC& a ) const { return x*a.x + y*a.y + z*a.z;  };
	inline TYPE norm2(              ) const { return x*x + y*y + z*z;        };

	inline TYPE norm ( ) const { return  sqrt( x*x + y*y + z*z ); };
    inline TYPE normalize() {
		TYPE norm  = sqrt( x*x + y*y + z*z );
		TYPE inVnorm = 1.0d/norm;
		x *= inVnorm;    y *= inVnorm;    z *= inVnorm;
		return norm;
    };

	inline void getSomeOrtho( VEC& v1, VEC& v2 ){
		if(x<y){ 
//			x : y*vz - z*vy; 
//			y : z*vx - x*vz; 
//			z : x*vy - y*vx; 
//			x : y*0 - z*0 ;
//			y : z*1 - x*0 ; 
//			z : x*0 - y*1 ;
//			float vx = 0; float vy = z; float vz =-y;
			v1.x =  -y*y -z*z;
			v1.y =  x*y;
			v1.z =  x*z;
		}else{ 
//			x : y*0 - z*1; 
//			y : z*0 - x*0; 
//			z : x*1 - y*0; 
//			float vx = -z; float vy = 0; float vz = x;
			v1.x =  y*x;
			v1.y =  -z*z -x*x;
			v1.z =  y*z;
		}
		v2.x = y*v1.z - z*v1.y; 
		v2.y = z*v1.x - x*v1.z; 
		v2.z = x*v1.y - y*v1.x;
	}

	// Rodrigues rotation formula: v' = cosa*v + sina*(uaxis X v) + (1-cosa)*(uaxis . v)*uaxis
	inline void rotate( TYPE angle, const VEC& axis  ){
		VEC uaxis;
		uaxis.set( axis * axis.norm() );
		TYPE ca   = cos(angle);
		TYPE sa   = sin(angle);
 		rotate_csa( ca, sa, uaxis );
	};

	inline void rotate_csa( TYPE ca, TYPE sa, const VEC& uaxis ){
		TYPE cu = (1-ca)*dot(uaxis);
		TYPE utx  = uaxis.y*z - uaxis.z*y; 
		TYPE uty  = uaxis.z*x - uaxis.x*z; 
		TYPE utz  = uaxis.x*y - uaxis.y*x;
		TYPE x_ = ca*x + sa*utx + cu*uaxis.x;
		TYPE y_ = ca*y + sa*uty + cu*uaxis.y;
		       z  = ca*z + sa*utz + cu*uaxis.z;
		x = x_; y = y_; 
	};

	void findProjectionOntoPlane( VEC vecToProj, VEC planeNormal ){
	// returns projection of vecToProj onto the plane whose normal is planeNormal
	// input vectors are assumed to be NORMALIZED!!!
	
		VEC proj;
		proj.set( vecToProj );
		TYPE scal_prod_minus_norm = (-1.0) * vecToProj.dot( planeNormal );
		proj.add_mul( planeNormal, scal_prod_minus_norm );
		
		x = proj.x; y = proj.y; z = proj.z;
				
	}
	
};

/*
class Vec3i : public Vec3TYPE< int,    Vec3i > {  };
class Vec3f : public Vec3TYPE< float,  Vec3f > {  };
class Vec3d : public Vec3TYPE< double, Vec3d > {  };
*/
using Vec3i = Vec3TYPE<int>;
using Vec3f = Vec3TYPE<float>;
using Vec3d = Vec3TYPE<double>;

void convert( const Vec3f& from, Vec3d& to ){ to.x=from.x;        to.y=from.y;        to.z=from.z; };
void convert( const Vec3d& from, Vec3f& to ){ to.x=(float)from.x; to.y=(float)from.y; to.z=(float)from.z; };

#endif


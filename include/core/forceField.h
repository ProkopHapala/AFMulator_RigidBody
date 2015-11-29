
#ifndef _FORCE_FIELD_
#define _FORCE_FIELD_

#define PI 3.1415926

// =============== Forcefield setup

void makeLJparams( int ntypes, double * Rs, double * Es, double*& C6s, double*& C12s ){
	int n2 = ntypes*ntypes;
	C6s  = new double[n2];
	C12s = new double[n2]; 
	for( int i=0; i<ntypes; i++ ){
		for( int j=0; j<=i; j++ ){
			double r   = Rs[i] + Rs[j];
			double e   = sqrt( Es[i] * Es[j]);
			double r6  = r*r*r; r6*=r6;
			double c6  = -2*e*r6; 
			double c12 =    e*r6*r6;
			C6s [ i*ntypes + j ] = c6; 
			C12s[ i*ntypes + j ] = c12;
			C6s [ j*ntypes + i ] = c6; 
			C12s[ j*ntypes + i ] = c12;
		}
	}
}

// =============== inter-Atomic Force-Field

inline void forceLJ( const Vec3d& dR, double c6, double c12, Vec3d& f ){
	double ir2  = 1/dR.norm2( ); 
	double ir6  = ir2*ir2*ir2;
	double ir12 = ir6*ir6;
	f.set_mul( dR , ( 6*ir6*c6 + 12*ir12*c12 ) * ir2   );
}

inline void forceLJE( const Vec3d& dR, double c6, double c12, double qq, Vec3d& f ){
	const double kcoulomb   = 14.3996448915;   //   (e/Angstrome) /(4*pi*epsilon0)  =  ( (1.60217657e-19/1e-10)^2)/(4*pi*8.85418782e-12)
	double ir2  = 1/dR.norm2( ); 
	double ir6  = ir2*ir2*ir2;
	double ir12 = ir6*ir6;
	double fr = 6*ir6*c6 + 12*ir12*c12;
	if( fabs(qq) > 1e-8 ){ fr += kcoulomb*qq*sqrt(ir2); };
	f.set_mul( dR ,  fr * ir2 );

	//printf( " %f %f %f %f %f %f \n", qq, sqrt(1/ir2), sqrt(ir2), ir2*sqrt(ir2),  kcoulomb*qq*ir2*sqrt(ir2)*dR.x,    f.x );
}


// =============== molecule-surface plane

inline void forceMolSurf( 
	double z0, double zMin, double 	Emin, const Vec3d& hat,
	int n,  Vec3d * Rs, Vec3d * Fs
){
	for(int i=0; i<n; i++){
		Vec3d dR,f;
		double  z   = hat.dot( Rs[i] ) - z0;
		double iz   = 1/z;
		double ir   = zMin*iz;
		double ir3  = ir*ir*ir;
		double v    = Emin*( ir3*ir3 - 2*ir3 );
		Fs[i].add_mul( hat, v * iz );
		//Fs[i].add_mul( hat, -z*0.1  );
//		printf( "v*iz = %f\n", v*iz );
	}
}

void forceMolSurfPerp( int npi, Vec3d* Tps_i, Vec3d* fs_i ){
// force on a molecule originating in its position in a plane
		
	double xforce, yforce;
	double amp = 5e-3;
//	double amp = 5e-4;
	double xamp = 8e-2;
	double yamp = 8e-2;
//	double xcoef = 5.99;
//	double ycoef = 5.99;
	double xcoef = 2*PI/1.0;
	double ycoef = 2*PI/1.0;
	double zcoef = -3;
	
	Vec3d xhat, yhat; // these vectors should be derived with help of hat vector!!!
	xhat.set( 1, 0, 0 );
	yhat.set( 0, 1, 0 );

//	printf( "Tps_i[0].z = %f\n", Tps_i[0].z );		
	for( int i = 0; i < npi; i++ ){
//		printf( "Tps_i[i].z = %f\n", Tps_i[i].z );
		xforce = xamp*sin( xcoef*Tps_i[i].x )*exp( zcoef*Tps_i[i].z );
		yforce = yamp*sin( ycoef*Tps_i[i].y )*exp( zcoef*Tps_i[i].z );
//		xforce = xamp*sin( xcoef*Tps_i[i].x )*exp( zcoef*Tps_i[i].z ) - (xamp*zcoef/xcoef)*cos( xcoef*Tps_i[i].x )*exp( zcoef*Tps_i[i].z );
//		yforce = yamp*sin( ycoef*Tps_i[i].y )*exp( zcoef*Tps_i[i].z ) - (yamp*zcoef/ycoef)*cos( ycoef*Tps_i[i].y )*exp( zcoef*Tps_i[i].z );
//		xforce = 0;
//		yforce = 0;
//		xforce = xamp*sin( xcoef*Tps_i[i].x );
//		yforce = yamp*sin( ycoef*Tps_i[i].y );
//		xforce = -amp*xcoef*sin( xcoef*Tps_i[i].x )*cos( ycoef*Tps_i[i].y );
//		yforce = -amp*ycoef*cos( xcoef*Tps_i[i].x )*sin( ycoef*Tps_i[i].y );
//		xforce = -amp*xcoef*sin( xcoef*Tps_i[i].x )*cos( ycoef*Tps_i[i].y )*exp( zcoef*fabs( Tps_i[i].z ) );
//		yforce = -amp*ycoef*cos( xcoef*Tps_i[i].x )*sin( ycoef*Tps_i[i].y )*exp( zcoef*fabs( Tps_i[i].z ) );
//		xforce = -amp*xcoef*sin( xcoef*Tps_i[i].x )*cos( ycoef*Tps_i[i].y )*cos( zcoef*Tps_i[i].z );
//		yforce = -amp*ycoef*cos( xcoef*Tps_i[i].x )*sin( ycoef*Tps_i[i].y )*cos( zcoef*Tps_i[i].z );

//		xforce = -amp*xcoef*sin( xcoef*Tps_i[i].x )*cos( ycoef*Tps_i[i].y )*exp( zcoef*Tps_i[i].z );
//		yforce = -amp*ycoef*cos( xcoef*Tps_i[i].x )*sin( ycoef*Tps_i[i].y )*exp( zcoef*Tps_i[i].z );

		fs_i[i].add_mul( xhat, xforce );
		fs_i[i].add_mul( yhat, yforce );
	}
	
	glColor3f( 1, 1, 1 );
	glBegin( GL_LINES );
//		glVertex( 0, 0, 0 ); glVertex( Tps_i[0].x, Tps_i[0].y, Tps_i[0].z );
		glVertex3f( 0, 0, 0 ); glVertex3f( Tps_i[0].x, 0, 0 );
	glEnd();
//	printf( "z_part_force = %f\n", exp( zcoef*Tps_i[0].z ) );
//	printf( "xforce = %f\n", xforce );
			
}

// =============== inter-Molecular Force-Field

inline void interMolForceLJE( 
	int na, int * atypes, double * Qas, Vec3d * aRs, Vec3d * aFs, 
	int nb, int * btypes, double * Qbs, Vec3d * bRs, Vec3d * bFs,
	int ntypes, double * C6s, double * C12s
){
	//printf( "---------------------\n" );
	//Vec3d fsum; fsum.set(0.0);
	for(int ia=0; ia<na; ia++){
		int atyp  = atypes[ia];
		int ityp0 = ntypes*atyp;
		double qa = Qas[ia];
		for(int ib=0; ib<nb; ib++){
			int btyp   = btypes[ib];
			int ityp   = ityp0 + btyp;
			double C6  = C6s [ ityp ];
			double C12 = C12s[ ityp ];
			Vec3d dR,f;
			dR.set_sub( aRs[ia], bRs[ib] );
			//printf( " %i %i %i %i ", ia, ib, atyp, btyp );
			forceLJE( dR, C6, C12, qa*Qbs[ib], f );
			aFs[ia].add( f ); bFs[ib].sub( f );
			//aFs[ia].sub( f ); bFs[ib].add( f );
			
			//printf( " %i %i %i %i   %f   %f   %f %f %f \n", ia, ib, atyp, btyp,    qa*Qbs[ib], f.dot(dR),  f.x, f.y, f.z ); 
			//printf( " %i %i   %f %f    %f %f %f \n", ia, ib, C6, C12,    f.x, f.y, f.z );
			//fsum.add( f );
		}
	}
	//printf( "fsum %f %f %f \n", fsum.x, fsum.y, fsum.z );
}

inline void interMolForceLJ( 
	int na, int * atypes, Vec3d * aRs, Vec3d * aFs, 
	int nb, int * btypes, Vec3d * bRs, Vec3d * bFs,
	int ntypes, double * C6s, double * C12s
){
	//printf( "---------------------\n" );
	for(int ia=0; ia<na; ia++){
		int atyp  = atypes[ia];
		int ityp0 = ntypes*atyp;
		for(int ib=0; ib<nb; ib++){
			int btyp  = btypes[ib];
			int ityp  = ityp0 + btyp;
			double C6  = C6s[ ityp ];
			double C12 = C12s[ ityp ];
			Vec3d dR,f;
			dR.set_sub( aRs[ia], bRs[ib] );
			forceLJ( dR, C6, C12, f );
			aFs[ia].add( f ); bFs[ib].sub( f );
			//aFs[ia].sub( f ); bFs[ib].add( f );
			//printf( " %i %i   %f %f    %f %f %f \n", ia, ib, C6, C12,    f.x, f.y, f.z );
		}
	}
}

// =============== tip forces

void forceSpring( double kcoeff, Vec3d dR, Vec3d& f ){
// spring force, i.e. f = -(kcoeff/2.0)*dR^2

	double norm = dR.norm();
	double rad = 3; // suitable guess
//	double rad = 0.09;
//	double rad = 0;
	Vec3d dRdiff;
	dRdiff.set( dR );
	dRdiff.normalize();
	dRdiff.mul( rad );
	dRdiff.set_sub( dR, dRdiff );

	if( norm > rad ){
		f.set_mul( dR, (-1)*(kcoeff/2.0)*(dRdiff.norm2())/norm );
//		printf( "(-1)*(kcoeff/2.0)*(dR.norm2()) = %f\n", (-1)*(kcoeff/2.0)*(dR.norm2()) );
	} else if( norm < rad && norm > 1e-3 ){
		f.set_mul( dR, (+1)*(kcoeff/2.0)*(dRdiff.norm2())/norm );
	} else {
		f.set( 0, 0, 0 );
	}

}

void forceSpringRotCoordSys( double kcoeffrot, Vec3d axisSys, Vec3d axisMol, Vec3d& force ){
// spring rotation force, molecule coordinate system is used for calculation of the force

	double cosphi = axisSys.dot( axisMol );
	if( cosphi < -1 + 1e-5 ) cosphi = -1 + 1e-5;
	double xsq = (double) (1 - cosphi) / (1 + cosphi);
	double forceMag = (kcoeffrot/2.0)*xsq;	

	Vec3d fzrp;
	fzrp.set_sub( axisSys, axisMol );
	fzrp.findProjectionOntoPlane( fzrp, axisMol );
	fzrp.normalize();
	fzrp.mul( forceMag );
	force.set( fzrp );
	
}

void forceSpringRotCoordSysRadius( double kcoeffrot, Vec3d axisSys, Vec3d axisMol, Vec3d& force ){
// spring rotation force, molecule coordinate system is used for calculation of the force
// axisMol is not normalized here, it is a radius vector

	Vec3d axisMolAux;
	axisMolAux.set( axisMol );
	if( axisMolAux.norm() < 1e-5 ){
		force.set( 0, 0, 0 );
		return;
	}
	axisMolAux.normalize();
	double cosphi = axisSys.dot( axisMolAux );
	if( cosphi < -1 + 1e-5 ) cosphi = -1 + 1e-5;
	double xsq = axisMol.norm2() * (double) (1 - cosphi) / (1 + cosphi);
	double forceMag = (kcoeffrot/2.0)*xsq;	

	Vec3d fzrp;
	fzrp.set_sub( axisSys, axisMolAux );
	fzrp.findProjectionOntoPlane( fzrp, axisMolAux );
	fzrp.normalize();
	fzrp.mul( forceMag );
	force.set( fzrp );
	
}

#endif
